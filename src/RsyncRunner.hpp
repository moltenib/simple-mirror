#pragma once

#include <functional>
#include <regex>
#include <string>
#include <vector>

#include <glibmm.h>

class RsyncRunner {
public:
    using OutputCallback = std::function<void(const std::string&)>;
    using ProgressCallback = std::function<void(int)>;
    using FinishedCallback = std::function<void(int, bool)>;

    void set_output_callback(OutputCallback callback);
    void set_progress_callback(ProgressCallback callback);
    void set_finished_callback(FinishedCallback callback);

    bool start(const std::string& origin, const std::string& destination, std::string& error);
    bool ensure_rsync_available(std::string& error);
    void cancel();
    bool is_running() const;

private:
    bool on_stdout_io(Glib::IOCondition condition);
    void on_child_exit(Glib::Pid pid, int status);
    void reset_handles();
    void emit_progress_if_present(const std::string& line);
    bool resolve_rsync_executable(std::string& error);
    std::string normalize_rsync_path(const std::string& path) const;

    OutputCallback output_callback_;
    ProgressCallback progress_callback_;
    FinishedCallback finished_callback_;

    bool running_ = false;
    Glib::Pid child_pid_ = 0;
    int stdout_fd_ = -1;
    Glib::RefPtr<Glib::IOChannel> stdout_channel_;
    sigc::connection io_watch_connection_;
    std::string rsync_executable_;

    const std::regex percent_regex_{R"((\d{1,3})%)"};
};
