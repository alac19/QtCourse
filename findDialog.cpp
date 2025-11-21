#include "findDialog.h"
#include "ui_findDialog.h"
#include <QMessageBox>

findDialog::findDialog(QWidget *parent, QPlainTextEdit* textEdit)
    : QDialog(parent)
    , ui(new Ui::findDialog)
{
    ui->setupUi(this);
    pTextEdit = textEdit;
    ui->rbUp->setChecked(true);
}

findDialog::~findDialog()
{
    delete ui;
}

void findDialog::on_btFindNext_clicked()
{
    QString target = ui->findText->text();

    if (target == "" || pTextEdit == nullptr) {
        return;
    }

    QString text = pTextEdit->toPlainText();
    QTextCursor c = pTextEdit->textCursor();
    int index = -1;

    if (ui->rbDown->isChecked()) {
        // 先从当前光标位置往后查找
        index = text.indexOf(target, c.position(), ui->casesensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

        // 如果没找到，就回到文本开头继续找
        if (index == -1) {
            index = text.indexOf(target, 0, ui->casesensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
            // 如果找到的位置就是当前光标位置（全文只有一处且已选中），提示已到末尾
            if (index != -1 && index == c.position()) {
                QMessageBox::information(this, "记事本", "已查找至文本末尾，已回到开头");
            }
        }

        if (index >= 0) {
            c.setPosition(index);
            c.setPosition(index + target.length(), QTextCursor::KeepAnchor);

            pTextEdit->setTextCursor(c);
        }
    }
    else if (ui->rbUp->isChecked()) {
        // index = text.lastIndexOf(target, c.position() - text.length() - 1, ui->casesensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

        // 修正起始位置，从当前光标位置的前一个字符开始往前找（避免重复查找当前位置）
        index = text.lastIndexOf(target, c.position() - 1, ui->casesensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

        // 如果没找到，就回到文本末尾继续找
        if (index == -1) {
            index = text.lastIndexOf(target, text.length() - 1, ui->casesensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
            // 如果找到的位置就是当前光标位置 -1（全文只有一处且已选中），提示已到首部
            if (index != -1 && index == c.position() - 1) {
                QMessageBox::information(this, "记事本", "已查找至文本首部，已回到末尾");
            }
        }

        if (index >= 0) {
            c.setPosition(index + target.length());
            c.setPosition(index, QTextCursor::KeepAnchor);

            pTextEdit->setTextCursor(c);
        }
    }

    if (index == -1) {
        QMessageBox msg(this);
        msg.setWindowTitle("记事本");
        msg.setText(QString("找不到") + target);
        msg.setWindowFlag(Qt::Drawer);
        msg.setIcon(QMessageBox::Information);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}


void findDialog::on_btCancel_clicked()
{
    accept();
}

