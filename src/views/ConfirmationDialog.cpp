#include "views/ConfirmationDialog.hpp"

#include <QAbstractButton>
#include <QMessageBox>
#include <QString>
#include <QWidget>

namespace confirmation_dialog {

bool show(
    QWidget* parent,
    const QString& title,
    const QString& warning_text,
    const QString& body_text,
    const QString& accept_button_text) {
    QMessageBox dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setIcon(QMessageBox::Warning);
    dialog.setTextFormat(Qt::RichText);
    QString body_html = body_text.toHtmlEscaped();
    body_html.replace(QLatin1Char('\n'), QStringLiteral("<br>"));
    dialog.setText(
        QStringLiteral("<span style=\"color:#b51616; font-weight:700;\">%1</span><br><br>%2")
            .arg(warning_text.toHtmlEscaped(), body_html));
    dialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    dialog.setDefaultButton(QMessageBox::Cancel);

    if (QAbstractButton* ok_button = dialog.button(QMessageBox::Ok)) {
        ok_button->setText(accept_button_text);
        ok_button->setStyleSheet(
            "background-color: #b51616;"
            "color:#fff1f1;"
            "font-weight:700;");
    }

    return dialog.exec() == QMessageBox::Ok;
}

}  // namespace confirmation_dialog
