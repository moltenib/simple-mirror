#pragma once

#include <QProgressBar>
#include <QPushButton>
#include <QString>
#include <QStatusBar>

class QLabel;
class QMainWindow;

class MainWindowStyle {
public:
    MainWindowStyle();

    void applyTo(QMainWindow& window) const;

private:
    QString stylesheet_;
};

class SyncButton : public QPushButton {
public:
    explicit SyncButton(QWidget* parent = nullptr);
    void setRunningState(bool running, bool combine_mode);

private:
    void recomputeMinimumWidth();
    void updateVisualState();

    QString synchronize_text_;
    QString combine_text_;
    QString stop_text_;
    bool running_;
    bool combine_mode_;
};

class ProgressBarWidget : public QProgressBar {
public:
    explicit ProgressBarWidget(QWidget* parent = nullptr);
};

class StatusBarWidget : public QStatusBar {
public:
    explicit StatusBarWidget(QWidget* parent = nullptr);

    void setStatusText(const QString& text);
    QString statusText() const;

private:
    QLabel* label_;
};
