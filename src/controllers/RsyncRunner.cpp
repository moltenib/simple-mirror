#include "controllers/RsyncRunner.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QTimer>

RsyncRunner::RsyncRunner() : process_(), running_(false) {
    process_.setProcessChannelMode(QProcess::MergedChannels);

    QObject::connect(&process_, &QProcess::readyReadStandardOutput, [this]() {
        on_ready_read();
    });

    QObject::connect(
        &process_,
        qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
        [this](int exit_code, QProcess::ExitStatus exit_status) {
            on_finished(exit_code, static_cast<int>(exit_status));
        });
}

RsyncRunner::~RsyncRunner() {
    file_callback_ = nullptr;
    progress_callback_ = nullptr;
    finished_callback_ = nullptr;
    QObject::disconnect(&process_, nullptr, nullptr, nullptr);

    if (process_.state() != QProcess::NotRunning) {
        process_.kill();
        process_.waitForFinished(1000);
    }
}

void RsyncRunner::set_file_callback(FileCallback callback) {
    file_callback_ = std::move(callback);
}

void RsyncRunner::set_progress_callback(ProgressCallback callback) {
    progress_callback_ = std::move(callback);
}

void RsyncRunner::set_finished_callback(FinishedCallback callback) {
    finished_callback_ = std::move(callback);
}

bool RsyncRunner::ensure_rsync_available(std::string& error) {
    if (!rsync_executable_.empty()) {
        return true;
    }
    return resolve_rsync_executable(error);
}

bool RsyncRunner::start(
    const std::string& origin,
    const std::string& destination,
    std::string& error) {
    if (running_) {
        error = QCoreApplication::translate(
                    "RsyncRunner",
                    "A synchronization is already running.")
                    .toStdString();
        return false;
    }

    if (!ensure_rsync_available(error)) {
        return false;
    }

    QStringList args;
    args << "-avh"
         << "--info=progress2"
         << "--delete"
         << QString::fromStdString(normalize_rsync_path(origin))
         << QString::fromStdString(normalize_rsync_path(destination));

    const QString rsync_path = QString::fromStdString(rsync_executable_);
#ifdef _WIN32
    configure_windows_process_environment(rsync_path);
#else
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LANG", "C");
    env.insert("LC_ALL", "C");
    process_.setProcessEnvironment(env);
#endif
    process_.start(rsync_path, args);
    if (!process_.waitForStarted()) {
        error = process_.errorString().toStdString();
        return false;
    }

    running_ = true;
    return true;
}

void RsyncRunner::stop() {
    if (!running_) {
        return;
    }
    process_.terminate();
    QTimer::singleShot(1200, &process_, [process = &process_]() {
        if (process->state() != QProcess::NotRunning) {
            process->kill();
        }
    });
}

bool RsyncRunner::stop_and_wait(int timeout_ms) {
    if (process_.state() == QProcess::NotRunning) {
        running_ = false;
        return true;
    }

    process_.terminate();
    if (process_.waitForFinished(std::max(timeout_ms, 0))) {
        running_ = false;
        return true;
    }

    process_.kill();
    const bool finished = process_.waitForFinished(1500);
    running_ = (process_.state() != QProcess::NotRunning);
    return finished;
}

bool RsyncRunner::is_running() const {
    return running_;
}

bool RsyncRunner::resolve_rsync_executable(std::string& error) {
    const QString env_value = qEnvironmentVariable("SIMPLE_MIRROR_RSYNC").trimmed();
#ifdef _WIN32
    if (!env_value.isEmpty()) {
        QFileInfo env_file(env_value);
        if (env_file.exists() && env_file.isFile()) {
            const QString env_path = env_file.absoluteFilePath();
            if (has_msys2_runtime(env_path)) {
                rsync_executable_ = env_path.toStdString();
                return true;
            }
            error = QCoreApplication::translate(
                        "RsyncRunner",
                        "SIMPLE_MIRROR_RSYNC points to rsync but MSYS2 runtime is missing "
                        "(msys-2.0.dll not found near rsync, app directory, or working directory): %1")
                        .arg(env_path)
                        .toStdString();
            return false;
        }
        error = QCoreApplication::translate(
                    "RsyncRunner",
                    "SIMPLE_MIRROR_RSYNC is set but does not point to a valid file: %1")
                    .arg(env_value)
                    .toStdString();
        return false;
    }
#else
    if (!env_value.isEmpty()) {
        QFileInfo env_file(env_value);
        if (env_file.exists() && env_file.isFile()) {
            rsync_executable_ = env_file.absoluteFilePath().toStdString();
            return true;
        }
        error = QCoreApplication::translate(
                    "RsyncRunner",
                    "SIMPLE_MIRROR_RSYNC is set but does not point to a valid file: %1")
                    .arg(env_value)
                    .toStdString();
        return false;
    }
#endif

    const QString cwd = QDir::currentPath();
    const QString app_dir = QCoreApplication::applicationDirPath();
    std::vector<QString> candidates;
#ifdef _WIN32
    candidates = {
        QDir(app_dir).filePath("runtime/msys2/usr/bin/rsync.exe"),
        QDir(cwd).filePath("runtime/msys2/usr/bin/rsync.exe"),
        QDir(app_dir).filePath("msys2/usr/bin/rsync.exe"),
        QDir(cwd).filePath("msys2/usr/bin/rsync.exe"),
    };
#else
    candidates = {
        QDir(app_dir).filePath("runtime/bin/rsync"),
        QDir(cwd).filePath("runtime/bin/rsync"),
        QDir(app_dir).filePath("bin/rsync"),
        QDir(cwd).filePath("bin/rsync"),
    };
#endif

    for (const QString& candidate : candidates) {
        QFileInfo info(candidate);
        if (info.exists() && info.isFile()) {
            const QString rsync_path = info.absoluteFilePath();
#ifdef _WIN32
            if (!has_msys2_runtime(rsync_path)) {
                continue;
            }
#endif
            rsync_executable_ = rsync_path.toStdString();
            return true;
        }
    }

#ifdef _WIN32
    error = QCoreApplication::translate(
                "RsyncRunner",
                "Could not find bundled MSYS2 rsync. Run \"make bundle-rsync\" or \"make windows-all\", "
                "or set SIMPLE_MIRROR_RSYNC to an MSYS2 rsync.exe.")
                .toStdString();
#else
    const QString from_path = QStandardPaths::findExecutable("rsync");
    if (!from_path.isEmpty()) {
        rsync_executable_ = from_path.toStdString();
        return true;
    }
    error = QCoreApplication::translate(
                "RsyncRunner",
                "Could not find rsync. Set SIMPLE_MIRROR_RSYNC, add rsync to PATH, or bundle "
                "\"runtime/bin/rsync\".")
                .toStdString();
#endif
    return false;
}

#ifdef _WIN32
bool RsyncRunner::has_msys2_runtime(const QString& rsync_path) const {
    const QFileInfo rsync_info(rsync_path);
    if (!rsync_info.exists() || !rsync_info.isFile()) {
        return false;
    }

    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString cwd = QDir::currentPath();
    const QStringList runtime_dirs = {
        rsync_info.absolutePath(),
        app_dir,
        cwd,
        QDir(app_dir).filePath("runtime/msys2/usr/bin"),
        QDir(cwd).filePath("runtime/msys2/usr/bin"),
    };

    for (const QString& runtime_dir : runtime_dirs) {
        if (runtime_dir.isEmpty()) {
            continue;
        }
        const QString runtime_dll = QDir(runtime_dir).filePath("msys-2.0.dll");
        QFileInfo dll_info(runtime_dll);
        if (dll_info.exists() && dll_info.isFile()) {
            return true;
        }
    }

    return false;
}

void RsyncRunner::configure_windows_process_environment(const QString& rsync_path) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString cwd = QDir::currentPath();
    const QStringList prepend_dirs = {
        QFileInfo(rsync_path).absolutePath(),
        QDir(app_dir).filePath("runtime/msys2/usr/bin"),
        QDir(cwd).filePath("runtime/msys2/usr/bin"),
        app_dir,
        cwd,
    };

    QStringList path_entries;
    for (const QString& dir : prepend_dirs) {
        if (dir.isEmpty()) {
            continue;
        }
        QFileInfo dir_info(dir);
        if (!dir_info.exists() || !dir_info.isDir()) {
            continue;
        }
        if (!path_entries.contains(dir, Qt::CaseInsensitive)) {
            path_entries << dir;
        }
    }

    const QString current_path = env.value("PATH");
    for (const QString& dir : current_path.split(';', Qt::SkipEmptyParts)) {
        if (!path_entries.contains(dir, Qt::CaseInsensitive)) {
            path_entries << dir;
        }
    }

    env.insert("PATH", path_entries.join(';'));
    env.insert("MSYSTEM", "MSYS");
    env.insert("MSYS2_PATH_TYPE", "inherit");
    env.insert("CHERE_INVOKING", "1");
    env.insert("LANG", "C");
    env.insert("LC_ALL", "C");
    process_.setProcessEnvironment(env);
    process_.setWorkingDirectory(app_dir);
}
#endif

std::string RsyncRunner::normalize_rsync_path(const std::string& path) const {
#ifndef _WIN32
    return path;
#else
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    static const std::regex drive_pattern(R"(^([A-Za-z]):/(.*)$)");
    std::smatch match;
    if (std::regex_match(normalized, match, drive_pattern) && match.size() == 3) {
        std::string converted = "/cygdrive/";
        converted += static_cast<char>(std::tolower(match[1].str()[0]));
        converted += "/";
        converted += match[2].str();
        return converted;
    }
    return normalized;
#endif
}

namespace {

std::string ltrim_copy(const std::string& value) {
    const auto begin = std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    return std::string(begin, value.end());
}

}  // namespace

void RsyncRunner::on_ready_read() {
    const QByteArray data = process_.readAllStandardOutput();
    if (data.isEmpty()) {
        return;
    }

    pending_output_ += QString::fromUtf8(data).toStdString();

    std::size_t cursor = 0;
    while (true) {
        const std::size_t line_end = pending_output_.find_first_of("\r\n", cursor);
        if (line_end == std::string::npos) {
            break;
        }

        const std::string line = pending_output_.substr(cursor, line_end - cursor);

        std::size_t consume_end = line_end + 1;
        while (consume_end < pending_output_.size() &&
               (pending_output_[consume_end] == '\r' || pending_output_[consume_end] == '\n')) {
            ++consume_end;
        }
        cursor = consume_end;

        emit_filtered_line(line);
    }

    if (cursor > 0) {
        pending_output_.erase(0, cursor);
    }
}

void RsyncRunner::on_finished(int exit_code, int exit_status) {
    if (!pending_output_.empty()) {
        emit_filtered_line(pending_output_);
        pending_output_.clear();
    }

    running_ = false;

    const bool signaled = (exit_status == static_cast<int>(QProcess::CrashExit));
    if (finished_callback_) {
        finished_callback_(exit_code, signaled);
    }
}

void RsyncRunner::emit_filtered_line(const std::string& line) {
    if (line.empty()) {
        return;
    }

    int percent = 0;
    std::string display_line;
    if (parse_overall_progress_line(line, percent, display_line)) {
        if (progress_callback_) {
            progress_callback_(percent, display_line);
        }
        return;
    }

    if (is_filelist_progress_noise(line)) {
        return;
    }

    std::string file_name;
    if (parse_current_file_line(line, file_name) && file_callback_) {
        file_callback_(file_name);
    }
}

bool RsyncRunner::parse_overall_progress_line(
    const std::string& line,
    int& percent,
    std::string& display_line) const {
    const std::string trimmed = ltrim_copy(line);
    if (trimmed.empty()) {
        return false;
    }

    std::smatch match;
    if (!std::regex_match(trimmed, match, overall_progress_regex_) || match.size() < 3) {
        return false;
    }

    percent = std::stoi(match[1].str());

    std::string speed = ltrim_copy(match[2].str());

    display_line = std::to_string(percent) + "%";
    if (!speed.empty()) {
        display_line += " (" + speed + ")";
    }
    return true;
}

bool RsyncRunner::is_filelist_progress_noise(const std::string& line) const {
    return std::regex_search(line, filelist_progress_noise_regex_);
}

bool RsyncRunner::parse_current_file_line(
    const std::string& line,
    std::string& file_name) const {
    const std::string trimmed = ltrim_copy(line);
    if (trimmed.empty()) {
        return false;
    }

    if (std::regex_search(trimmed, summary_line_regex_)) {
        return false;
    }

    if (std::regex_search(trimmed, error_line_regex_)) {
        return false;
    }

    if (trimmed == "./") {
        return false;
    }

    if (trimmed.rfind("deleting ", 0) == 0) {
        const QString deleting_path = QString::fromStdString(trimmed.substr(9));
        file_name = QCoreApplication::translate("RsyncRunner", "Deleting %1")
                        .arg(deleting_path)
                        .toStdString();
        return true;
    }

    file_name = trimmed;
    return true;
}
