#include "views/MainWindow.hpp"

#include <algorithm>
#include <filesystem>

#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QEvent>
#include <QTimer>
#include <QCloseEvent>
#include <QEventLoop>
#include <QVBoxLayout>
#include <QWidget>

#include "views/ConfirmationDialog.hpp"
#include "views/MainWindowMisc.hpp"
#include "views/WelcomeDialog.hpp"
#include "utils/AppSetup.hpp"
#include "utils/DurationFormat.hpp"
#include "utils/Settings.hpp"

MainWindow::MainWindow(const std::string& icon_name)
    : origin_chooser_(nullptr),
      destination_chooser_(nullptr),
      sync_button_(nullptr),
      status_bar_(nullptr),
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

    Settings& settings = Settings::instance();
    const bool has_settings_file = std::filesystem::exists(settings.file_path());
    settings.load();
    if (!settings.origin().empty()) {
        origin_chooser_->setPath(QString::fromStdString(settings.origin()));
    }
    if (!settings.destination().empty()) {
        destination_chooser_->setPath(QString::fromStdString(settings.destination()));
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

    sync_button_ = new SyncButton(central);

    auto* sync_column = new QVBoxLayout();
    sync_column->setSpacing(6);
    sync_column->setContentsMargins(0, 0, 0, 0);
    sync_column->addWidget(sync_button_);

    top_row->addLayout(directory_column, 1);
    top_row->addLayout(sync_column);

    progress_bar_ = new ProgressBarWidget(central);
    progress_animation_ = new QPropertyAnimation(progress_bar_, "value", this);
    progress_animation_->setDuration(180);
    progress_animation_->setEasingCurve(QEasingCurve::OutCubic);

    status_bar_ = new StatusBarWidget(central);

    main_layout->addLayout(top_row);
    main_layout->addWidget(status_bar_, 1);
    main_layout->addWidget(progress_bar_);

    setCentralWidget(central);
    set_status_text(tr("Choose two directories to synchronize."));
    apply_stylesheet();
    installEventFilter(this);

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
        if (stop_requested_) {
            progress_bar_->setFormat(tr("Stopped"));
            set_status_text(tr("The program has stopped. Press the button to resume."));
        } else if (signaled) {
            progress_bar_->setFormat(tr("Crashed"));
            set_status_text(tr("The synchronization process crashed."));
            show_error(
                tr("The synchronization process crashed. On Windows this usually means the bundled rsync runtime is incomplete or stale."),
                tr("Synchronization crashed"));
        } else if (exit_code == 0) {
            QString elapsed_text = duration_format::to_hh_mm_ss(std::chrono::milliseconds::zero());
            if (sync_timing_active_) {
                const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - sync_started_at_);
                elapsed_text = duration_format::to_hh_mm_ss(elapsed);
            }
            Settings& settings = Settings::instance();
            settings.set_origin(origin_chooser_->path().toStdString());
            settings.set_destination(destination_chooser_->path().toStdString());
            settings.save();
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

    if (!has_settings_file) {
        QTimer::singleShot(0, this, [this]() {
            WelcomeDialog::show(this);
        });
    }
}

MainWindow::~MainWindow() {
    removeEventFilter(this);
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

// Detect Shift press
bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    Q_UNUSED(watched);
    if (!runner_.is_running() && event &&
        (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)) {
        auto* key_event = static_cast<QKeyEvent*>(event);
        if (key_event && key_event->key() == Qt::Key_Shift) {
            update_sync_button_text(false);
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::apply_stylesheet() {
    window_style_.applyTo(*this);
}

void MainWindow::set_status_text(const QString& text) {
    if (!status_bar_) {
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
        if (!status_bar_) {
            return;
        }
        if (status_bar_->statusText() != pending_status_text_) {
            status_bar_->setStatusText(pending_status_text_);
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
    update_sync_button_text(running);
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

void MainWindow::update_sync_button_text(bool running) {
    const bool combine_mode = !running && combine_mode_requested();
    sync_button_->setRunningState(running, combine_mode);
}

bool MainWindow::combine_mode_requested() const {
    return (QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier) != 0;
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

    const bool delete_extraneous = !combine_mode_requested();
    if (!confirm_synchronize(delete_extraneous)) {
        return;
    }

    stop_requested_ = false;
    set_running_state(true);

    std::string start_error;
    if (!runner_.start(origin, destination, delete_extraneous, start_error)) {
        set_running_state(false);
        progress_bar_->setFormat(tr("Error"));
        set_status_text(tr("Error"));
        show_error(QString::fromStdString(start_error), tr("Synchronization error"));
    }
}

bool MainWindow::confirm_synchronize(bool delete_extraneous) {
    if (!delete_extraneous) {
        return true;
    }
    return confirmation_dialog::show(this);
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
