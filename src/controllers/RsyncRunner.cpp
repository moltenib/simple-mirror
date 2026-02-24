#include "controllers/RsyncRunner.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>

RsyncRunner::RsyncRunner() : process_(), running_(false) {
    process_.setProcessChannelMode(QProcess::MergedChannels);

    QObject::connect(&process_, &QProcess::readyReadStandardOutput, [this]() {
        handle_ready_read();
    });

    QObject::connect(
        &process_,
        qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
        [this](int exit_code, QProcess::ExitStatus exit_status) {
            handle_finished(exit_code, static_cast<int>(exit_status));
        });
}

RsyncRunner::~RsyncRunner() {
    output_callback_ = nullptr;
    progress_callback_ = nullptr;
    finished_callback_ = nullptr;
    QObject::disconnect(&process_, nullptr, nullptr, nullptr);

    if (process_.state() != QProcess::NotRunning) {
        process_.kill();
        process_.waitForFinished(1000);
    }
}

void RsyncRunner::set_output_callback(OutputCallback callback) {
    output_callback_ = std::move(callback);
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

bool RsyncRunner::start(const std::string& origin, const std::string& destination, std::string& error) {
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
    args << "-a"
         << "-v"
         << "--info=progress2"
         << "--delete"
         << QString::fromStdString(normalize_rsync_path(origin))
         << QString::fromStdString(normalize_rsync_path(destination));

    process_.start(QString::fromStdString(rsync_executable_), args);
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

bool RsyncRunner::is_running() const {
    return running_;
}

bool RsyncRunner::resolve_rsync_executable(std::string& error) {
    const QString env_value = qEnvironmentVariable("SIMPLE_MIRROR_RSYNC").trimmed();
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

    const QString cwd = QDir::currentPath();
    const QString app_dir = QCoreApplication::applicationDirPath();
    std::vector<QString> candidates;
#ifdef _WIN32
    candidates = {
        QDir(app_dir).filePath("runtime/msys2/usr/bin/rsync.exe"),
        QDir(cwd).filePath("runtime/msys2/usr/bin/rsync.exe"),
        QDir(app_dir).filePath("runtime/bin/rsync.exe"),
        QDir(cwd).filePath("runtime/bin/rsync.exe"),
        QDir(app_dir).filePath("msys2/usr/bin/rsync.exe"),
        QDir(cwd).filePath("msys2/usr/bin/rsync.exe"),
        QDir(app_dir).filePath("bin/rsync.exe"),
        QDir(cwd).filePath("bin/rsync.exe"),
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
            rsync_executable_ = info.absoluteFilePath().toStdString();
            return true;
        }
    }

#ifdef _WIN32
    QString from_path = QStandardPaths::findExecutable("rsync.exe");
    if (from_path.isEmpty()) {
        from_path = QStandardPaths::findExecutable("rsync");
    }
#else
    const QString from_path = QStandardPaths::findExecutable("rsync");
#endif

    if (!from_path.isEmpty()) {
        rsync_executable_ = from_path.toStdString();
        return true;
    }

    error = QCoreApplication::translate(
                "RsyncRunner",
                "Could not find rsync. Set SIMPLE_MIRROR_RSYNC, add rsync to PATH, or bundle "
                "\"runtime/bin/rsync\" on Linux, or \"runtime/msys2/usr/bin/rsync.exe\" on Windows.")
                .toStdString();
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

namespace {

std::string ltrim_copy(const std::string& value) {
    const auto begin = std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    return std::string(begin, value.end());
}

}  // namespace

void RsyncRunner::handle_ready_read() {
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

void RsyncRunner::handle_finished(int exit_code, int exit_status) {
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

    if (output_callback_) {
        output_callback_(line + "\n");
    }
}

bool RsyncRunner::parse_overall_progress_line(
    const std::string& line,
    int& percent,
    std::string& display_line) const {
    // Parse `progress2` lines with ir-chk/to-chk as progress-only UI updates
    if (!std::regex_search(line, overall_with_checks_regex_)) {
        return false;
    }

    std::smatch match;
    if (!std::regex_search(line, match, percent_regex_) || match.size() < 2) {
        return false;
    }

    percent = std::stoi(match[1].str());

    std::string speed;
    std::smatch speed_match;
    if (std::regex_search(line, speed_match, speed_regex_) && speed_match.size() >= 2) {
        speed = speed_match[1].str();
        speed = ltrim_copy(speed);

        const auto alpha_it = std::find_if(speed.begin(), speed.end(), [](unsigned char ch) {
            return std::isalpha(ch) != 0;
        });
        if (alpha_it != speed.end() && alpha_it != speed.begin()) {
            const std::size_t alpha_index =
                static_cast<std::size_t>(std::distance(speed.begin(), alpha_it));
            if (speed[alpha_index - 1] != ' ') {
                speed.insert(alpha_index, " ");
            }
        }
    }

    display_line = std::to_string(percent) + "%";
    if (!speed.empty()) {
        display_line += " (" + speed + ")";
    }
    return true;
}

bool RsyncRunner::is_filelist_progress_noise(const std::string& line) const {
    return std::regex_search(line, filelist_progress_noise_regex_);
}
