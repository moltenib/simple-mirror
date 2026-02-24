#pragma once

#include <functional>
#include <regex>
#include <string>

#include <QProcess>

class RsyncRunner {
public:
    using OutputCallback = std::function<void(const std::string&)>;
    using ProgressCallback = std::function<void(int, const std::string&)>;
    using FinishedCallback = std::function<void(int, bool)>;

    RsyncRunner();
    ~RsyncRunner();
    RsyncRunner(const RsyncRunner&) = delete;
    RsyncRunner& operator=(const RsyncRunner&) = delete;
    RsyncRunner(RsyncRunner&&) = delete;
    RsyncRunner& operator=(RsyncRunner&&) = delete;

    void set_output_callback(OutputCallback callback);
    void set_progress_callback(ProgressCallback callback);
    void set_finished_callback(FinishedCallback callback);

    bool ensure_rsync_available(std::string& error);
    bool start(const std::string& origin, const std::string& destination, std::string& error);
    void stop();
    bool is_running() const;

private:
    bool resolve_rsync_executable(std::string& error);
    std::string normalize_rsync_path(const std::string& path) const;
    void handle_ready_read();
    void handle_finished(int exit_code, int exit_status);
    void emit_filtered_line(const std::string& line);
    bool parse_overall_progress_line(
        const std::string& line,
        int& percent,
        std::string& display_line) const;
    bool is_filelist_progress_noise(const std::string& line) const;

    QProcess process_;
    bool running_;
    std::string rsync_executable_;
    std::string pending_output_;

    OutputCallback output_callback_;
    ProgressCallback progress_callback_;
    FinishedCallback finished_callback_;

    const std::regex percent_regex_{R"((\d{1,3})%)"};
    const std::regex speed_regex_{R"(([0-9][0-9.,]*\s*[kMGTPE]?i?B/s))"};
    const std::regex overall_with_checks_regex_{
        R"(\(xfr#\d+,\s*(ir-chk|to-chk)=\d+/\d+\))"};
    const std::regex filelist_progress_noise_regex_{
        R"(\(xfr#0,\s*(ir-chk|to-chk)=)"};
};
