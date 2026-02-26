#pragma once

#include <functional>
#include <regex>
#include <string>

#include <QProcess>

class RsyncRunner {
public:
    using FileCallback = std::function<void(const std::string&)>;
    using ProgressCallback = std::function<void(int, const std::string&)>;
    using FinishedCallback = std::function<void(int, bool)>;

    RsyncRunner();
    ~RsyncRunner();
    RsyncRunner(const RsyncRunner&) = delete;
    RsyncRunner& operator=(const RsyncRunner&) = delete;
    RsyncRunner(RsyncRunner&&) = delete;
    RsyncRunner& operator=(RsyncRunner&&) = delete;

    void set_file_callback(FileCallback callback);
    void set_progress_callback(ProgressCallback callback);
    void set_finished_callback(FinishedCallback callback);

    bool ensure_rsync_available(std::string& error);
    bool start(const std::string& origin, const std::string& destination, std::string& error);
    void stop();
    bool stop_and_wait(int timeout_ms = 2500);
    bool is_running() const;

private:
    bool resolve_rsync_executable(std::string& error);
    std::string normalize_rsync_path(const std::string& path) const;
#ifdef _WIN32
    bool has_msys2_runtime(const QString& rsync_path) const;
    void configure_windows_process_environment(const QString& rsync_path);
#endif
    void handle_ready_read();
    void handle_finished(int exit_code, int exit_status);
    void emit_filtered_line(const std::string& line);
    bool parse_overall_progress_line(
        const std::string& line,
        int& percent,
        std::string& display_line) const;
    bool parse_current_file_line(
        const std::string& line,
        std::string& file_name) const;
    bool is_filelist_progress_noise(const std::string& line) const;

    QProcess process_;
    bool running_;
    std::string rsync_executable_;
    std::string pending_output_;

    FileCallback file_callback_;
    ProgressCallback progress_callback_;
    FinishedCallback finished_callback_;

    const std::regex overall_progress_regex_{
        R"(^[0-9][0-9.,]*[kKMGTPE]?i?[bB]?\s+([0-9]{1,3})%\s+([0-9][0-9.,]*\s*[kKMGTPE]?i?B/s)\s+[0-9]+:[0-9]{2}:[0-9]{2}(\s+\(xfr#[0-9]+,\s*(ir-chk|to-chk)=[0-9]+/[0-9]+\))?\s*$)"};
    const std::regex filelist_progress_noise_regex_{
        R"(\(xfr#0,\s*(ir-chk|to-chk)=)"};
    const std::regex summary_line_regex_{
        R"(^(sending incremental file list|receiving incremental file list|sent [0-9].*|total size is .*|speedup is .*))"};
    const std::regex error_line_regex_{
        R"(^(rsync:|rsync error:))"};
};
