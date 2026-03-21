#pragma once

#include <chrono>
#include <string>

#include <QCoreApplication>
#include <QMainWindow>

#include "controllers/RsyncRunner.hpp"
#include "views/DirectoryChooserWidget.hpp"
#include "views/MainWindowMisc.hpp"

class QPropertyAnimation;
class QString;
class QCloseEvent;
class ProgressBarWidget;
class StatusBarWidget;
class SyncButton;

class MainWindow : public QMainWindow {
public:
    Q_DECLARE_TR_FUNCTIONS(MainWindow)
public:
    explicit MainWindow(const std::string& icon_name);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void apply_stylesheet();
    void set_status_text(const QString& text);
    void show_current_file(const std::string& text);
    void set_running_state(bool running);

    void on_sync_clicked();

    bool validate_inputs(std::string& origin, std::string& destination);
    void show_error(const QString& message, const QString& title = QString());

    MainWindowStyle window_style_;
    DirectoryChooserWidget* origin_chooser_;
    DirectoryChooserWidget* destination_chooser_;
    SyncButton* sync_button_;
    StatusBarWidget* status_bar_;
    QString pending_status_text_;
    bool status_update_scheduled_;
    ProgressBarWidget* progress_bar_;
    QPropertyAnimation* progress_animation_;
    int last_progress_percent_;
    bool stop_requested_;
    std::chrono::steady_clock::time_point sync_started_at_;
    bool sync_timing_active_;

    RsyncRunner runner_;
};
