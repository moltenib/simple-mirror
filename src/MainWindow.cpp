#include "MainWindow.hpp"

#include <filesystem>

#include <QHBoxLayout>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSizePolicy>
#include <QScreen>
#include <QFontMetrics>
#include <QTextEdit>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QWidget>

namespace {

const char* kAppStyle = R"QSS(
#main-window {
  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0b0b0b, stop:1 #160707);
}

QLabel#origin-label,
QLabel#destination-label {
  color: #ffd6d6;
}

QLineEdit#origin-edit,
QLineEdit#destination-edit {
  color: #ffe7e7;
  background-color: #2a0d0d;
  border: 1px solid #6f2020;
  border-radius: 6px;
  padding: 4px;
}

QPushButton#browse-origin,
QPushButton#browse-destination,
QPushButton#sync-button {
  color: #ffe7e7;
  background-color: #2a0d0d;
  border: 1px solid #6f2020;
  border-radius: 6px;
  padding: 6px 10px;
}

QPushButton#browse-origin[syncRunning="true"],
QPushButton#browse-destination[syncRunning="true"] {
  color: #a9a9a9;
  background-color: #2b2b2b;
  border: 1px solid #4a4a4a;
}

QPushButton#browse-origin:disabled,
QPushButton#browse-destination:disabled {
  color: #a9a9a9;
  background-color: #2b2b2b;
  border: 1px solid #4a4a4a;
}

QPushButton#sync-button {
  font-weight: 700;
  background-color: #b51616;
  border: 1px solid #ff4d4d;
  color: #fff1f1;
}

QTextEdit#output-view {
  background: #000000;
  color: #ffffff;
  border: 1px solid #ffffff;
  border-radius: 8px;
  margin: 0;
  padding: 0;
}

QProgressBar#progress-bar {
  color: #ffe7e7;
  border: 1px solid #4a1414;
  border-radius: 6px;
  background: #1b1b1b;
  text-align: center;
  margin: 0;
  padding: 0;
}

QProgressBar#progress-bar::chunk {
  background: #cc1f1f;
  border-radius: 6px;
}
)QSS";

}  // namespace

MainWindow::MainWindow(const std::string& icon_name)
    : origin_chooser_(nullptr),
      destination_chooser_(nullptr),
      sync_button_(nullptr),
      output_view_(nullptr),
      progress_bar_(nullptr),
      last_progress_percent_(-1),
      cancel_requested_(false) {
    setObjectName("main-window");
    setWindowTitle("Quick Back-Up");
    setMinimumSize(800, 600);
    resize(800, 600);

    if (!icon_name.empty()) {
        setWindowIcon(QIcon::fromTheme(QString::fromStdString(icon_name)));
    }

    if (QScreen* screen = this->screen()) {
        const QRect available = screen->availableGeometry();
        move(
            available.center().x() - (width() / 2),
            available.center().y() - (height() / 2));
    }

    auto* central = new QWidget(this);
    auto* main_layout = new QVBoxLayout(central);
    main_layout->setContentsMargins(12, 12, 12, 12);
    main_layout->setSpacing(6);

    auto* top_row = new QHBoxLayout();
    top_row->setSpacing(6);

    auto* directory_column = new QVBoxLayout();
    directory_column->setSpacing(6);

    origin_chooser_ = new DirectoryChooserWidget(
        "Origin:",
        "Select origin folder",
        "origin-label",
        "origin-edit",
        "browse-origin",
        central);
    destination_chooser_ = new DirectoryChooserWidget(
        "Destination:",
        "Select destination folder",
        "destination-label",
        "destination-edit",
        "browse-destination",
        central);

    directory_column->addWidget(origin_chooser_);
    directory_column->addWidget(destination_chooser_);

    sync_button_ = new QPushButton("Synchronize", central);
    sync_button_->setObjectName("sync-button");
    sync_button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    const QFontMetrics metrics(sync_button_->font());
    const int min_button_width = std::max(
                                     metrics.horizontalAdvance("Synchronize"),
                                     metrics.horizontalAdvance("Cancel")) +
                                 32;
    sync_button_->setMinimumWidth(min_button_width);

    auto* sync_column = new QVBoxLayout();
    sync_column->setSpacing(6);
    sync_column->addWidget(sync_button_);

    top_row->addLayout(directory_column, 1);
    top_row->addLayout(sync_column);

    output_view_ = new QTextEdit(central);
    output_view_->setObjectName("output-view");
    output_view_->setReadOnly(true);

    progress_bar_ = new QProgressBar(central);
    progress_bar_->setObjectName("progress-bar");
    progress_bar_->setRange(0, 100);
    progress_bar_->setValue(0);
    progress_bar_->setFormat("Idle");

    main_layout->addLayout(top_row);
    main_layout->addWidget(output_view_, 1);
    main_layout->addWidget(progress_bar_);

    setCentralWidget(central);
    apply_stylesheet();

    QObject::connect(sync_button_, &QPushButton::clicked, [this]() { on_sync_clicked(); });

    runner_.set_output_callback([this](const std::string& text) { append_output(text); });
    runner_.set_progress_callback([this](int percent, const std::string& progress_line) {
        if (percent > last_progress_percent_) {
            last_progress_percent_ = percent;
            progress_bar_->setValue(last_progress_percent_);
        }
        progress_bar_->setFormat(QString::fromStdString(progress_line));
    });
    runner_.set_finished_callback([this](int exit_code, bool signaled) {
        if (cancel_requested_) {
            progress_bar_->setFormat("Cancelled");
        } else if (signaled) {
            progress_bar_->setFormat("Cancelled");
        } else if (exit_code == 0) {
            progress_bar_->setValue(100);
            progress_bar_->setFormat("Done");
        } else {
            progress_bar_->setFormat("Failed");
            show_error(
                "Synchronization failed with exit code " + std::to_string(exit_code) + ".",
                "Synchronization failed");
        }
        cancel_requested_ = false;
        set_running_state(false);
    });
}

void MainWindow::apply_stylesheet() {
    setStyleSheet(kAppStyle);
}

void MainWindow::append_output(const std::string& text) {
    output_view_->moveCursor(QTextCursor::End);
    output_view_->insertPlainText(QString::fromUtf8(text.c_str()));
    output_view_->moveCursor(QTextCursor::End);
}

void MainWindow::set_running_state(bool running) {
    sync_button_->setText(running ? "Cancel" : "Synchronize");
    origin_chooser_->setChooserEnabled(!running);
    destination_chooser_->setChooserEnabled(!running);

    if (running) {
        last_progress_percent_ = -1;
        progress_bar_->setValue(0);
        progress_bar_->setFormat("Running...");
    }
}

void MainWindow::on_sync_clicked() {
    if (runner_.is_running()) {
        cancel_requested_ = true;
        runner_.cancel();
        progress_bar_->setFormat("Stopping...");
        return;
    }

    std::string origin;
    std::string destination;
    if (!validate_inputs(origin, destination)) {
        return;
    }

    std::string rsync_error;
    if (!runner_.ensure_rsync_available(rsync_error)) {
        show_error(rsync_error, "rsync not found");
        return;
    }

    if (!confirm_synchronize()) {
        return;
    }

    output_view_->clear();
    cancel_requested_ = false;
    set_running_state(true);
    append_output(
        "$ rsync -avP --info=progress2 --delete \"" + origin + "\" \"" +
        destination + "\"\n\n");

    std::string start_error;
    if (!runner_.start(origin, destination, start_error)) {
        set_running_state(false);
        progress_bar_->setFormat("Error");
        append_output("Error: " + start_error + "\n");
        show_error(start_error, "Synchronization error");
    }
}

bool MainWindow::confirm_synchronize() {
    QMessageBox dialog(this);
    dialog.setWindowTitle("Notice");
    dialog.setIcon(QMessageBox::Warning);
    dialog.setText("Any files in the destination folder that do not exist in the origin will be deleted.");
    dialog.setInformativeText("This is to keep the destination folder up to date. Continue?");
    dialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    dialog.setDefaultButton(QMessageBox::Cancel);
    return dialog.exec() == QMessageBox::Ok;
}

bool MainWindow::validate_inputs(std::string& origin, std::string& destination) {
    origin = origin_chooser_->path().toStdString();
    destination = destination_chooser_->path().toStdString();

    if (origin.empty() || destination.empty()) {
        show_error("Please choose both origin and destination folders.");
        return false;
    }

    if (!std::filesystem::is_directory(origin)) {
        show_error("Origin folder does not exist: " + origin);
        return false;
    }

    if (!std::filesystem::is_directory(destination)) {
        show_error("Destination folder does not exist: " + destination);
        return false;
    }

    if (!origin.empty() && origin.back() != '/' && origin.back() != '\\') {
        origin.push_back('/');
    }
    if (!destination.empty() && destination.back() != '/' && destination.back() != '\\') {
        destination.push_back('/');
    }

    return true;
}

void MainWindow::show_error(const std::string& message, const std::string& title) {
    QMessageBox::critical(
        this,
        QString::fromStdString(title),
        QString::fromStdString(message));
}
