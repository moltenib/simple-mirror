#include "views/MainWindow.hpp"

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
#include <QTextDocument>
#include <QVBoxLayout>
#include <QWidget>

#include "utils/AppSetup.hpp"

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

QLineEdit#origin-edit[placeholderStyled="true"],
QLineEdit#destination-edit[placeholderStyled="true"] {
  font-style: italic;
  font-size: 90%;
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

QPushButton#browse-origin:enabled:hover,
QPushButton#browse-destination:enabled:hover {
  background-color: #1c0606;
  border: 1px solid #531717;
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

QPushButton#sync-button:enabled:hover {
  background-color: #8f1010;
  border: 1px solid #d33b3b;
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
      stop_requested_(false) {
    setObjectName("main-window");
    setWindowTitle(tr("Simple Mirror"));
    setMinimumSize(800, 600);
    resize(800, 600);

    app_setup::apply_window_icon(*this, icon_name);

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
    top_row->setContentsMargins(0, 0, 0, 0);

    auto* directory_column = new QVBoxLayout();
    directory_column->setSpacing(6);
    directory_column->setContentsMargins(0, 0, 0, 0);

    origin_chooser_ = new DirectoryChooserWidget(
        tr("Origin:"),
        tr("The folder to be backed up."),
        tr("Select origin folder"),
        "origin-label",
        "origin-edit",
        "browse-origin",
        central);
    destination_chooser_ = new DirectoryChooserWidget(
        tr("Destination:"),
        tr("A folder inside a back-up medium."),
        tr("Select destination folder"),
        "destination-label",
        "destination-edit",
        "browse-destination",
        central);

    directory_column->addWidget(origin_chooser_);
    directory_column->addWidget(destination_chooser_);

    sync_button_ = new QPushButton(tr("Synchronize"), central);
    sync_button_->setObjectName("sync-button");
    sync_button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    const QFontMetrics metrics(sync_button_->font());
    const QString synchronize_text = tr("Synchronize");
    const QString stop_text = tr("Stop");
    const int min_button_width = std::max(
                                     metrics.horizontalAdvance(synchronize_text),
                                     metrics.horizontalAdvance(stop_text)) +
                                 32;
    sync_button_->setMinimumWidth(min_button_width);

    auto* sync_column = new QVBoxLayout();
    sync_column->setSpacing(6);
    sync_column->setContentsMargins(0, 0, 0, 0);
    sync_column->addWidget(sync_button_);

    top_row->addLayout(directory_column, 1);
    top_row->addLayout(sync_column);

    output_view_ = new QTextEdit(central);
    output_view_->setObjectName("output-view");
    output_view_->setReadOnly(true);
    output_view_->setUndoRedoEnabled(false);
    output_view_->document()->setMaximumBlockCount(5000);

    progress_bar_ = new QProgressBar(central);
    progress_bar_->setObjectName("progress-bar");
    progress_bar_->setRange(0, 100);
    progress_bar_->setValue(0);
    progress_bar_->setFormat(tr("Idle"));

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
        if (stop_requested_) {
            progress_bar_->setFormat(tr("Stopped"));
        } else if (signaled) {
            progress_bar_->setFormat(tr("Stopped"));
        } else if (exit_code == 0) {
            progress_bar_->setValue(100);
            progress_bar_->setFormat(tr("Done"));
        } else {
            progress_bar_->setFormat(tr("Failed"));
            show_error(
                tr("Synchronization failed with exit code %1.").arg(exit_code),
                tr("Synchronization failed"));
        }
        stop_requested_ = false;
        set_running_state(false);
    });
}

MainWindow::~MainWindow() {
    runner_.set_output_callback(nullptr);
    runner_.set_progress_callback(nullptr);
    runner_.set_finished_callback(nullptr);
    if (runner_.is_running()) {
        runner_.stop();
    }
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
    sync_button_->setText(running ? tr("Stop") : tr("Synchronize"));
    origin_chooser_->setChooserEnabled(!running);
    destination_chooser_->setChooserEnabled(!running);

    if (running) {
        last_progress_percent_ = -1;
        progress_bar_->setValue(0);
        progress_bar_->setFormat(tr("Running..."));
    }
}

void MainWindow::on_sync_clicked() {
    if (runner_.is_running()) {
        stop_requested_ = true;
        runner_.stop();
        progress_bar_->setFormat(tr("Stopping..."));
        return;
    }

    std::string origin;
    std::string destination;
    if (!validate_inputs(origin, destination)) {
        return;
    }

    std::string rsync_error;
    if (!runner_.ensure_rsync_available(rsync_error)) {
        show_error(QString::fromStdString(rsync_error), tr("rsync not found"));
        return;
    }

    if (!confirm_synchronize()) {
        return;
    }

    output_view_->clear();
    stop_requested_ = false;
    set_running_state(true);
    append_output(
        "$ rsync -av --info=progress2 --delete \"" + origin + "\" \"" +
        destination + "\"\n\n");

    std::string start_error;
    if (!runner_.start(origin, destination, start_error)) {
        set_running_state(false);
        progress_bar_->setFormat(tr("Error"));
        append_output(tr("Error: %1\n").arg(QString::fromStdString(start_error)).toStdString());
        show_error(QString::fromStdString(start_error), tr("Synchronization error"));
    }
}

bool MainWindow::confirm_synchronize() {
    QMessageBox dialog(this);
    dialog.setWindowTitle(tr("Notice"));
    dialog.setIcon(QMessageBox::Warning);
    dialog.setText(tr("Any files in the destination folder that do not exist in the origin will be deleted."));
    dialog.setInformativeText(tr("This is to keep the destination folder up to date. Continue?"));
    QPushButton* ok_button = dialog.addButton(QMessageBox::Ok);
    QPushButton* stop_button = dialog.addButton(tr("Stop"), QMessageBox::RejectRole);
    dialog.setDefaultButton(stop_button);
    dialog.exec();
    return dialog.clickedButton() == ok_button;
}

bool MainWindow::validate_inputs(std::string& origin, std::string& destination) {
    origin = origin_chooser_->path().toStdString();
    destination = destination_chooser_->path().toStdString();

    if (origin.empty() || destination.empty()) {
        show_error(tr("Please choose both origin and destination folders."));
        return false;
    }

    if (!std::filesystem::is_directory(origin)) {
        show_error(tr("Origin folder does not exist: %1").arg(QString::fromStdString(origin)));
        return false;
    }

    if (!std::filesystem::is_directory(destination)) {
        show_error(tr("Destination folder does not exist: %1").arg(QString::fromStdString(destination)));
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

void MainWindow::show_error(const QString& message, const QString& title) {
    QMessageBox::critical(
        this,
        title.isEmpty() ? tr("Error") : title,
        message);
}
