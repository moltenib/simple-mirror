#pragma once

#include <functional>
#include <regex>
#include <string>

class QProcess;

class RsyncRunner {
public:
    using OutputCallback = std::function<void(const std::string&)>;
    using ProgressCallback = std::function<void(int, const std::string&)>;
    using FinishedCallback = std::function<void(int, bool)>;

    RsyncRunner();
    ~RsyncRunner();

    void set_output_callback(OutputCallback callback);
    void set_progress_callback(ProgressCallback callback);
    void set_finished_callback(FinishedCallback callback);

    bool ensure_rsync_available(std::string& error);
    bool start(const std::string& origin, const std::string& destination, std::string& error);
    void cancel();
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

    QProcess* process_;
    bool running_;
    std::string rsync_executable_;
    std::string pending_output_;

    OutputCallback output_callback_;
    ProgressCallback progress_callback_;
    FinishedCallback finished_callback_;

    const std::regex percent_regex_{R"((\d{1,3})%)"};
    const std::regex overall_with_checks_regex_{
        R"(\(xfr#\d+,\s*(ir-chk|to-chk)=\d+/\d+\))"};
    const std::regex filelist_progress_noise_regex_{
        R"(\(xfr#0,\s*(ir-chk|to-chk)=)"};
};
