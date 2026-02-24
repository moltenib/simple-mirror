#pragma once

#include <string>

#include <QCoreApplication>
#include <QMainWindow>

#include "controllers/RsyncRunner.hpp"
#include "views/DirectoryChooserWidget.hpp"

class QPushButton;
class QTextEdit;
class QProgressBar;
class QString;

class MainWindow : public QMainWindow {
public:
    Q_DECLARE_TR_FUNCTIONS(MainWindow);
public:

    explicit MainWindow(const std::string& icon_name);
    ~MainWindow() override;

private:
    void apply_stylesheet();
    void append_output(const std::string& text);
    void set_running_state(bool running);

    void on_sync_clicked();

    bool confirm_synchronize();
    bool validate_inputs(std::string& origin, std::string& destination);
    void show_error(const QString& message, const QString& title = QString());

    DirectoryChooserWidget* origin_chooser_;
    DirectoryChooserWidget* destination_chooser_;
    QPushButton* sync_button_;
    QTextEdit* output_view_;
    QProgressBar* progress_bar_;
    int last_progress_percent_;
    bool stop_requested_;

    RsyncRunner runner_;
};
