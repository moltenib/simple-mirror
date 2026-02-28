#pragma once

class QWidget;
class QString;

namespace confirmation_dialog {

bool show(
    QWidget* parent,
    const QString& title,
    const QString& warning_text,
    const QString& body_text,
    const QString& accept_button_text);

}  // namespace confirmation_dialog
