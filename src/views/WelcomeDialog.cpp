#include "views/WelcomeDialog.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QGridLayout>
#include <QIcon>
#include <QMessageBox>
#include <QSpacerItem>
#include <QString>
#include <QWidget>

void WelcomeDialog::show(QWidget* parent) {
    QMessageBox dialog(parent);
    QIcon dialog_icon;
    if (parent && !parent->windowIcon().isNull()) {
        dialog_icon = parent->windowIcon();
    } else {
        dialog_icon = QApplication::windowIcon();
    }

    if (!dialog_icon.isNull()) {
        dialog.setWindowIcon(dialog_icon);
        dialog.setIconPixmap(dialog_icon.pixmap(48, 48));
    } else {
        dialog.setIcon(QMessageBox::Information);
    }

    dialog.setWindowTitle(QCoreApplication::translate("WelcomeDialog", "Welcome!"));
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setTextFormat(Qt::RichText);
    const QString accent_style =
        "<style>.accent { color: #2f8adf; font-weight: 600; }</style>";
    dialog.setText(accent_style + QCoreApplication::translate(
        "WelcomeDialog",
        "<p>It appears that no backups have been performed with this installation yet. How it works:</p>"
        "<p><span class='accent'>1)</span> Choose an origin. This is the folder to be backed up. It will not be modified by this "
        "program.</p>"
        "<p><span class='accent'>2)</span> Insert a back-up medium, like an external drive. This will be the destination.</p>"
        "<p><span class='accent'>3)</span> Press <span class='accent'>&quot;Synchronize&quot;</span> to turn the destination into a mirror of the origin. This is "
        "irreversible. Use with care!</p>"
        "<ul style='margin-top: 0px;'>"
        "<li>The two folders are compared.</li>"
        "<li>Files in the destination that do not exist in the origin will be <span style='color: #b51616; "
        "font-weight: 600;'>permanently deleted</span>.</li>"
        "<li>Files in the origin that are missing in the destination are copied from the origin.</li>"
        "</ul>"
        "<p>This message will disappear after the first run.</p>"));
    if (QGridLayout* layout = qobject_cast<QGridLayout*>(dialog.layout())) {
        auto* width_spacer = new QSpacerItem(650, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(width_spacer, layout->rowCount(), 0, 1, layout->columnCount());
    }
    dialog.setStandardButtons(QMessageBox::Ok);
    dialog.setDefaultButton(QMessageBox::Ok);

    if (parent && parent->isVisible()) {
        dialog.ensurePolished();
        dialog.adjustSize();
        QRect dialog_geometry = dialog.frameGeometry();
        dialog_geometry.moveCenter(parent->frameGeometry().center());
        dialog.move(dialog_geometry.topLeft());
    }

    dialog.exec();
}
