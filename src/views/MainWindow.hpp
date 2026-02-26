#pragma once

#include <string>

#include <QCoreApplication>
#include <QMainWindow>

#include "controllers/RsyncRunner.hpp"
#include "views/DirectoryChooserWidget.hpp"

class QPushButton;
class QProgressBar;
class QLabel;
class QStatusBar;
class QString;

class MainWindow : public QMainWindow {
public:
    Q_DECLARE_TR_FUNCTIONS(MainWindow)
public:
    explicit MainWindow(const std::string& icon_name);
    ~MainWindow() override;

private:
    void apply_stylesheet();
    void set_status_text(const QString& text);
    void show_current_file(const std::string& text);
    void set_running_state(bool running);

    void on_sync_clicked();

    bool confirm_synchronize();
    bool validate_inputs(std::string& origin, std::string& destination);
    void show_error(const QString& message, const QString& title = QString());

    DirectoryChooserWidget* origin_chooser_;
    DirectoryChooserWidget* destination_chooser_;
    QPushButton* sync_button_;
    QStatusBar* status_bar_;
    QLabel* status_label_;
    QString pending_status_text_;
    bool status_update_scheduled_;
    QProgressBar* progress_bar_;
    int last_progress_percent_;
    bool stop_requested_;

    RsyncRunner runner_;
};
