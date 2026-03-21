#include "views/MainWindowMisc.hpp"

#include <algorithm>

#include <QCoreApplication>
#include <QFont>
#include <QFontMetrics>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QSizePolicy>
#include <QStyle>
#include <QString>

MainWindowStyle::MainWindowStyle()
    : stylesheet_(R"QSS(
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
)QSS") {}

void MainWindowStyle::applyTo(QMainWindow& window) const {
    window.setStyleSheet(stylesheet_);
}

SyncButton::SyncButton(QWidget* parent)
    : QPushButton(parent),
      synchronize_text_(QCoreApplication::translate("MainWindow", "Synchronize")),
      stop_text_(QCoreApplication::translate("MainWindow", "Stop")),
      running_(false) {
    setObjectName("sync-button");
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    recomputeMinimumWidth();
    updateVisualState();
}

void SyncButton::setRunningState(bool running) {
    running_ = running;
    updateVisualState();
}

void SyncButton::recomputeMinimumWidth() {
    const QFontMetrics metrics(font());
    const int minimum_width = std::max(
                                  metrics.horizontalAdvance(synchronize_text_),
                                  metrics.horizontalAdvance(stop_text_)) +
                              32;
    setMinimumWidth(minimum_width);
}

void SyncButton::updateVisualState() {
    if (running_) {
        setText(stop_text_);
        return;
    }
    setText(synchronize_text_);
}

ProgressBarWidget::ProgressBarWidget(QWidget* parent)
    : QProgressBar(parent) {
    setObjectName("progress-bar");
    setRange(0, 100);
    setValue(0);
    setFormat(QCoreApplication::translate("MainWindow", "Idle"));
}

StatusBarWidget::StatusBarWidget(QWidget* parent)
    : QStatusBar(parent),
      label_(new QLabel(this)) {
    setObjectName("status-bar");
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setContentsMargins(0, 0, 0, 0);

    if (QLayout* status_layout = layout()) {
        status_layout->setContentsMargins(0, 0, 0, 0);
        status_layout->setSpacing(0);
    }

    label_->setObjectName("status-label");
    label_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label_->setContentsMargins(0, 0, 0, 0);

    QFont status_font = label_->font();
    status_font.setPointSizeF(status_font.pointSizeF() * 1.1);
    label_->setFont(status_font);

    addWidget(label_, 1);
}

void StatusBarWidget::setStatusText(const QString& text) {
    label_->setText(text);
}

QString StatusBarWidget::statusText() const {
    return label_->text();
}
