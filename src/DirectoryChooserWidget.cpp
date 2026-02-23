#include "DirectoryChooserWidget.hpp"

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStyle>

DirectoryChooserWidget::DirectoryChooserWidget(
    const QString& label_text,
    const QString& chooser_title,
    const QString& label_object_name,
    const QString& line_edit_object_name,
    const QString& browse_button_object_name,
    QWidget* parent)
    : QWidget(parent),
      chooser_title_(chooser_title),
      label_(new QLabel(label_text, this)),
      path_edit_(new QLineEdit(this)),
      browse_button_(new QPushButton("Select", this)) {
    label_->setObjectName(label_object_name);
    path_edit_->setObjectName(line_edit_object_name);
    path_edit_->setReadOnly(true);
    browse_button_->setObjectName(browse_button_object_name);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->addWidget(label_);
    layout->addWidget(path_edit_, 1);
    layout->addWidget(browse_button_);

    QObject::connect(browse_button_, &QPushButton::clicked, [this]() { open_dialog(); });
}

QString DirectoryChooserWidget::path() const {
    return path_edit_->text();
}

void DirectoryChooserWidget::setPath(const QString& path) {
    path_edit_->setText(path);
}

void DirectoryChooserWidget::setChooserEnabled(bool enabled) {
    path_edit_->setEnabled(enabled);
    browse_button_->setEnabled(enabled);
    browse_button_->setProperty("syncRunning", !enabled);
    browse_button_->style()->unpolish(browse_button_);
    browse_button_->style()->polish(browse_button_);
    browse_button_->update();
}

void DirectoryChooserWidget::open_dialog() {
    const QString selected = QFileDialog::getExistingDirectory(this, chooser_title_, path_edit_->text());
    if (!selected.isEmpty()) {
        path_edit_->setText(QDir::toNativeSeparators(selected));
    }
}
