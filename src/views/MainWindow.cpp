#include "views/MainWindow.hpp"

#include <algorithm>
#include <filesystem>

#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QProgressBar>
#include <QPushButton>
#include <QSizePolicy>
#include <QScreen>
#include <QFontMetrics>
#include <QLayout>
#include <QStatusBar>
#include <QTimer>
#include <QCloseEvent>
#include <QEventLoop>
#include <QVBoxLayout>
#include <QWidget>

#include "views/ConfirmationDialog.hpp"
#include "utils/AppSetup.hpp"
#include "utils/DurationFormat.hpp"
#include "utils/Settings.hpp"

namespace {

const char* kAppStyle = R"QSS(
#main-window {
  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0b0f13, stop:1 #071622);
}

QLabel#origin-label,
QLabel#destination-label {
  color: #d6ecff;
}

QLineEdit#origin-edit,
QLineEdit#destination-edit {
  color: #e7f3ff;
  background-color: #0d1f2a;
  border: 1px solid #20506f;
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
  color: #e7f3ff;
  background-color: #0d1f2a;
  border: 1px solid #20506f;
  border-radius: 6px;
  padding: 6px 10px;
}

QPushButton#browse-origin,
QPushButton#browse-destination {
  color: #e7f3ff;
  background-color: #0d1f2a;
  border: 1px solid #20506f;
}

QPushButton#browse-origin[syncRunning="true"],
QPushButton#browse-destination[syncRunning="true"] {
  color: #a9a9a9;
  background-color: #2b2b2b;
  border: 1px solid #4a4a4a;
}

QPushButton#browse-origin:enabled:hover,
QPushButton#browse-destination:enabled:hover {
  color: #e7f3ff;
  background-color: #06121c;
  border: 1px solid #173d53;
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

QProgressBar#progress-bar {
  color: #e7f3ff;
  border: 1px solid #14344a;
  border-radius: 6px;
  background: #1b1b1b;
  text-align: center;
  margin: 0;
  padding: 0;
}

QProgressBar#progress-bar::chunk {
  background: #b51616;
  border-radius: 6px;
}

QStatusBar#status-bar {
  color: #e7f3ff;
  background: transparent;
  border: none;
  padding: 0;
  margin: 0;
}

QStatusBar#status-bar::item {
  border: none;
  padding: 0;
  margin: 0;
}

QLabel#status-label {
  color: #e7f3ff;
  background: transparent;
  font-size: 112%;
  padding: 0;
  margin: 0;
}
)QSS";

}  // namespace

MainWindow::MainWindow(const std::string& icon_name)
    : origin_chooser_(nullptr),
      destination_chooser_(nullptr),
      sync_button_(nullptr),
      status_bar_(nullptr),
      status_label_(nullptr),
      pending_status_text_(),
      status_update_scheduled_(false),
      progress_bar_(nullptr),
      progress_animation_(nullptr),
      last_progress_percent_(-1),
      stop_requested_(false),
      sync_started_at_(),
      sync_timing_active_(false) {
    setObjectName("main-window");
    setWindowTitle(tr("Simple Mirror"));
    setMinimumSize(700, 350);
    setFixedSize(750, 350);

    app_setup::apply_window_icon(*this, icon_name);

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

    const settings::Settings last_settings = settings::load();
    if (!last_settings.origin.empty()) {
        origin_chooser_->setPath(QString::fromStdString(last_settings.origin));
    }
    if (!last_settings.destination.empty()) {
        destination_chooser_->setPath(QString::fromStdString(last_settings.destination));
    }

    auto* origin_label = origin_chooser_->findChild<QLabel*>("origin-label");
    auto* destination_label = destination_chooser_->findChild<QLabel*>("destination-label");
    if (origin_label && destination_label) {
        const int label_width = std::max(
            origin_label->sizeHint().width(),
            destination_label->sizeHint().width());
        origin_label->setFixedWidth(label_width);
        destination_label->setFixedWidth(label_width);
    }

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

    progress_bar_ = new QProgressBar(central);
    progress_bar_->setObjectName("progress-bar");
    progress_bar_->setRange(0, 100);
    progress_bar_->setValue(0);
    progress_bar_->setFormat(tr("Idle"));
    progress_animation_ = new QPropertyAnimation(progress_bar_, "value", this);
    progress_animation_->setDuration(180);
    progress_animation_->setEasingCurve(QEasingCurve::OutCubic);

    status_bar_ = new QStatusBar(central);
    status_bar_->setObjectName("status-bar");
    status_bar_->setSizeGripEnabled(false);
    status_bar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    status_bar_->setContentsMargins(0, 0, 0, 0);
    if (QLayout* status_layout = status_bar_->layout()) {
        status_layout->setContentsMargins(0, 0, 0, 0);
        status_layout->setSpacing(0);
    }

    status_label_ = new QLabel(status_bar_);
    status_label_->setObjectName("status-label");
    status_label_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    status_label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QFont status_font = status_label_->font();
    status_font.setPointSizeF(status_font.pointSizeF() * 1.1);
    status_label_->setFont(status_font);
    status_label_->setContentsMargins(0, 0, 0, 0);
    status_bar_->addWidget(status_label_, 1);

    main_layout->addLayout(top_row);
    main_layout->addWidget(status_bar_, 1);
    main_layout->addWidget(progress_bar_);

    setCentralWidget(central);
    set_status_text(tr("Choose two directories to synchronize."));
    apply_stylesheet();

    QObject::connect(sync_button_, &QPushButton::clicked, [this]() { on_sync_clicked(); });

    runner_.set_file_callback([this](const std::string& text) { show_current_file(text); });
    runner_.set_progress_callback([this](int percent, const std::string& progress_line) {
        if (percent > last_progress_percent_) {
            last_progress_percent_ = percent;
        }
        const int target_percent = std::clamp(last_progress_percent_, 0, 100);
        if (progress_animation_ && progress_bar_->value() != target_percent) {
            const int current_percent = progress_bar_->value();
            const int delta = std::abs(target_percent - current_percent);
            const int animation_duration = std::clamp(90 + (delta * 20), 90, 300);
            progress_animation_->stop();
            progress_animation_->setDuration(animation_duration);
            progress_animation_->setStartValue(current_percent);
            progress_animation_->setEndValue(target_percent);
            progress_animation_->start();
        } else {
            progress_bar_->setValue(target_percent);
        }

        QString format = QString::fromStdString(progress_line);
        const int percent_sign = format.indexOf('%');
        if (percent_sign > 0) {
            bool ok = false;
            format.left(percent_sign).toInt(&ok);
            if (ok) {
                format = QString::number(last_progress_percent_) + "%" + format.mid(percent_sign + 1);
            }
        }
        progress_bar_->setFormat(format);
    });
    runner_.set_finished_callback([this](int exit_code, bool signaled) {
        if (progress_animation_) {
            progress_animation_->stop();
        }
        if (stop_requested_ || signaled) {
            progress_bar_->setFormat(tr("Stopped"));
            set_status_text(tr("The program has stopped. Press the button to resume."));
        } else if (exit_code == 0) {
            QString elapsed_text = duration_format::to_hh_mm_ss(std::chrono::milliseconds::zero());
            if (sync_timing_active_) {
                const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - sync_started_at_);
                elapsed_text = duration_format::to_hh_mm_ss(elapsed);
            }
            settings::save(settings::Settings{
                origin_chooser_->path().toStdString(),
                destination_chooser_->path().toStdString()});
            progress_bar_->setValue(100);
            progress_bar_->setFormat(tr("Done") + " (" + elapsed_text + ")");
            set_status_text(tr("Mirroring complete."));
        } else {
            progress_bar_->setFormat(tr("Failed"));
            set_status_text(tr("Failed"));
            show_error(
                tr("Synchronization failed with exit code %1.").arg(exit_code),
                tr("Synchronization failed"));
        }
        stop_requested_ = false;
        set_running_state(false);
    });
}

MainWindow::~MainWindow() {
    runner_.set_file_callback(nullptr);
    runner_.set_progress_callback(nullptr);
    runner_.set_finished_callback(nullptr);
    if (runner_.is_running()) {
        runner_.stop_and_wait();
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (!runner_.is_running()) {
        QMainWindow::closeEvent(event);
        return;
    }

    stop_requested_ = true;
    if (progress_animation_) {
        progress_animation_->stop();
    }
    progress_bar_->setFormat(tr("Stopping..."));
    set_status_text(tr("Stopping..."));
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    runner_.stop_and_wait();
    event->accept();
}

void MainWindow::apply_stylesheet() {
    setStyleSheet(kAppStyle);
}

void MainWindow::set_status_text(const QString& text) {
    if (!status_label_) {
        return;
    }

    if (pending_status_text_ == text) {
        return;
    }

    pending_status_text_ = text;

    if (status_update_scheduled_) {
        return;
    }

    status_update_scheduled_ = true;
    QTimer::singleShot(0, this, [this]() {
        status_update_scheduled_ = false;
        if (!status_label_) {
            return;
        }
        if (status_label_->text() != pending_status_text_) {
            status_label_->setText(pending_status_text_);
        }
    });
}

void MainWindow::show_current_file(const std::string& text) {
    if (text.empty()) {
        return;
    }
    set_status_text(QString::fromStdString(text));
}

void MainWindow::set_running_state(bool running) {
    sync_button_->setText(running ? tr("Stop") : tr("Synchronize"));
    origin_chooser_->setChooserEnabled(!running);
    destination_chooser_->setChooserEnabled(!running);

    if (running) {
        sync_started_at_ = std::chrono::steady_clock::now();
        sync_timing_active_ = true;
        last_progress_percent_ = -1;
        if (progress_animation_) {
            progress_animation_->stop();
        }
        progress_bar_->setValue(0);
        progress_bar_->setFormat(tr("Running..."));
        set_status_text(tr("Comparing the two folders..."));
        return;
    }

    sync_timing_active_ = false;
}

void MainWindow::on_sync_clicked() {
    if (runner_.is_running()) {
        stop_requested_ = true;
        runner_.stop();
        progress_bar_->setFormat(tr("Stopping..."));
        set_status_text(tr("Stopping..."));
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

    stop_requested_ = false;
    set_running_state(true);

    std::string start_error;
    if (!runner_.start(origin, destination, start_error)) {
        set_running_state(false);
        progress_bar_->setFormat(tr("Error"));
        set_status_text(tr("Error"));
        show_error(QString::fromStdString(start_error), tr("Synchronization error"));
    }
}

bool MainWindow::confirm_synchronize() {
    const QString body_text = tr(
        "Any files in the destination folder that do not exist in the origin will be deleted.\n\n"
        "This is to keep the destination folder up to date. The origin folder is never altered.\n"
        "Continue?");
    return confirmation_dialog::show(
        this,
        tr("Notice"),
        tr("WARNING: POSSIBLE DATA LOSS!"),
        body_text,
        tr("I understand the risk"));
}

bool MainWindow::validate_inputs(std::string& origin, std::string& destination) {
    origin = origin_chooser_->path().toStdString();
    destination = destination_chooser_->path().toStdString();

    if (origin.empty() || destination.empty()) {
        show_error(tr("Please choose both origin and destination folders."));
        return false;
    }

    std::error_code origin_error;
    const bool origin_is_directory = std::filesystem::is_directory(origin, origin_error);
    if (origin_error) {
        show_error(tr("Cannot access origin folder: %1").arg(QString::fromStdString(origin)));
        return false;
    }
    if (!origin_is_directory) {
        show_error(tr("Origin folder does not exist: %1").arg(QString::fromStdString(origin)));
        return false;
    }

    std::error_code destination_error;
    const bool destination_is_directory = std::filesystem::is_directory(destination, destination_error);
    if (destination_error) {
        show_error(tr("Cannot access destination folder: %1").arg(QString::fromStdString(destination)));
        return false;
    }
    if (!destination_is_directory) {
        show_error(tr("Destination folder does not exist: %1").arg(QString::fromStdString(destination)));
        return false;
    }

    constexpr char path_separator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

    if (!origin.empty() && origin.back() != '/' && origin.back() != '\\') {
        origin.push_back(path_separator);
    }
    if (!destination.empty() && destination.back() != '/' && destination.back() != '\\') {
        destination.push_back(path_separator);
    }

    return true;
}

void MainWindow::show_error(const QString& message, const QString& title) {
    QMessageBox::critical(
        this,
        title.isEmpty() ? tr("Error") : title,
        message);
}
