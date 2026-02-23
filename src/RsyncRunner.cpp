#include "RsyncRunner.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <regex>
#include <sys/wait.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <csignal>
#include <unistd.h>
#endif

void RsyncRunner::set_output_callback(OutputCallback callback) {
    output_callback_ = std::move(callback);
}

void RsyncRunner::set_progress_callback(ProgressCallback callback) {
    progress_callback_ = std::move(callback);
}

void RsyncRunner::set_finished_callback(FinishedCallback callback) {
    finished_callback_ = std::move(callback);
}

bool RsyncRunner::start(const std::string& origin, const std::string& destination, std::string& error) {
    if (running_) {
        error = "A synchronization is already running.";
        return false;
    }

    if (!ensure_rsync_available(error)) {
        return false;
    }

    std::vector<std::string> argv{
        rsync_executable_,
        "-aP",
        "--delete",
        normalize_rsync_path(origin),
        normalize_rsync_path(destination),
    };

    int stdin_fd = -1;
    int stderr_fd = -1;

    try {
        Glib::spawn_async_with_pipes(
            ".",
            argv,
            Glib::SpawnFlags::SPAWN_SEARCH_PATH | Glib::SpawnFlags::SPAWN_DO_NOT_REAP_CHILD,
            sigc::slot<void()>(),
            &child_pid_,
            &stdin_fd,
            &stdout_fd_,
            &stderr_fd);
    } catch (const Glib::Error& exc) {
        error = exc.what();
        return false;
    }

    if (stdin_fd >= 0) {
        ::close(stdin_fd);
    }
    if (stderr_fd >= 0) {
        ::close(stderr_fd);
    }

    stdout_channel_ = Glib::IOChannel::create_from_fd(stdout_fd_);
    stdout_channel_->set_encoding("");
    stdout_channel_->set_buffered(false);

    io_watch_connection_ = Glib::signal_io().connect(
        sigc::mem_fun(*this, &RsyncRunner::on_stdout_io),
        stdout_channel_,
        Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR);

    Glib::signal_child_watch().connect(sigc::mem_fun(*this, &RsyncRunner::on_child_exit), child_pid_);

    running_ = true;
    return true;
}

void RsyncRunner::cancel() {
    if (!running_ || child_pid_ == 0) {
        return;
    }
#ifdef _WIN32
    ::TerminateProcess(reinterpret_cast<HANDLE>(child_pid_), 1);
#else
    ::kill(child_pid_, SIGTERM);
#endif
}

bool RsyncRunner::is_running() const {
    return running_;
}

bool RsyncRunner::on_stdout_io(Glib::IOCondition condition) {
    if ((condition & Glib::IO_ERR) == Glib::IO_ERR) {
        return false;
    }

    if ((condition & Glib::IO_IN) == Glib::IO_IN) {
        Glib::ustring line;
        Glib::IOStatus status = stdout_channel_->read_line(line);
        if (status == Glib::IO_STATUS_NORMAL) {
            if (output_callback_) {
                output_callback_(line.raw());
            }
            emit_progress_if_present(line.raw());
        }
    }

    if ((condition & Glib::IO_HUP) == Glib::IO_HUP) {
        return false;
    }

    return true;
}

void RsyncRunner::on_child_exit(Glib::Pid pid, int status) {
    running_ = false;

    bool signaled = WIFSIGNALED(status);
    int exit_code = -1;
    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    }

    if (finished_callback_) {
        finished_callback_(exit_code, signaled);
    }

    if (pid != 0) {
        Glib::spawn_close_pid(pid);
    }

    reset_handles();
}

void RsyncRunner::reset_handles() {
    if (io_watch_connection_.connected()) {
        io_watch_connection_.disconnect();
    }

    stdout_channel_.reset();

    if (stdout_fd_ >= 0) {
        ::close(stdout_fd_);
    }

    stdout_fd_ = -1;
    child_pid_ = 0;
}

void RsyncRunner::emit_progress_if_present(const std::string& line) {
    std::smatch match;
    if (!std::regex_search(line, match, percent_regex_) || match.size() < 2) {
        return;
    }

    int percent = std::stoi(match[1].str());
    if (progress_callback_) {
        progress_callback_(percent);
    }
}

bool RsyncRunner::ensure_rsync_available(std::string& error) {
    if (!rsync_executable_.empty()) {
        return true;
    }
    return resolve_rsync_executable(error);
}

bool RsyncRunner::resolve_rsync_executable(std::string& error) {
    const char* env_value = std::getenv("QUICK_BACKUP_RSYNC");
    if (env_value != nullptr && *env_value != '\0') {
        const std::string candidate = env_value;
        if (std::filesystem::exists(candidate)) {
            rsync_executable_ = candidate;
            return true;
        }
        error = "QUICK_BACKUP_RSYNC is set but does not point to a valid file: " + candidate;
        return false;
    }

    const std::string cwd = Glib::get_current_dir();
    std::vector<std::string> bundled_candidates;
#ifdef _WIN32
    bundled_candidates = {
        Glib::build_filename(cwd, "runtime/bin/rsync.exe"),
        Glib::build_filename(cwd, "msys2/usr/bin/rsync.exe"),
        Glib::build_filename(cwd, "bin/rsync.exe"),
    };
#else
    bundled_candidates = {
        Glib::build_filename(cwd, "runtime/bin/rsync"),
        Glib::build_filename(cwd, "bin/rsync"),
    };
#endif

    for (const auto& candidate : bundled_candidates) {
        if (std::filesystem::exists(candidate)) {
            rsync_executable_ = candidate;
            return true;
        }
    }

#ifdef _WIN32
    std::string from_path = Glib::find_program_in_path("rsync.exe");
    if (from_path.empty()) {
        from_path = Glib::find_program_in_path("rsync");
    }
#else
    const std::string from_path = Glib::find_program_in_path("rsync");
#endif

    if (!from_path.empty()) {
        rsync_executable_ = from_path;
        return true;
    }

    error = "Could not find rsync. Set QUICK_BACKUP_RSYNC, add rsync to PATH, or bundle "
            "\"runtime/bin/rsync\" (or \"runtime/bin/rsync.exe\" on Windows).";
    return false;
}

std::string RsyncRunner::normalize_rsync_path(const std::string& path) const {
#ifndef _WIN32
    return path;
#else
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    static const std::regex drive_pattern(R"(^([A-Za-z]):/(.*)$)");
    std::smatch match;
    if (std::regex_match(normalized, match, drive_pattern) && match.size() == 3) {
        std::string converted = "/";
        converted += static_cast<char>(std::tolower(match[1].str()[0]));
        converted += "/";
        converted += match[2].str();
        return converted;
    }
    return normalized;
#endif
}
