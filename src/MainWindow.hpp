#pragma once

#include <string>

#include <QMainWindow>

#include "DirectoryChooserWidget.hpp"
#include "RsyncRunner.hpp"

class QPushButton;
class QTextEdit;
class QProgressBar;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(const std::string& icon_name);

private:
    void apply_stylesheet();
    void append_output(const std::string& text);
    void set_running_state(bool running);

    void on_sync_clicked();

    bool confirm_synchronize();
    bool validate_inputs(std::string& origin, std::string& destination);
    void show_error(const std::string& message, const std::string& title = "Error");

    DirectoryChooserWidget* origin_chooser_;
    DirectoryChooserWidget* destination_chooser_;
    QPushButton* sync_button_;
    QTextEdit* output_view_;
    QProgressBar* progress_bar_;
    int last_progress_percent_;
    bool cancel_requested_;

    RsyncRunner runner_;
};
