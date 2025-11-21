#include "replacedialog.h"
#include "ui_replacedialog.h"
#include <QMessageBox>

ReplaceDialog::ReplaceDialog(QWidget *parent, QPlainTextEdit* textEdit)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    pTextEdit = textEdit;

    ui->rbDown->setChecked(true);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::on_btFindNext_clicked()
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


void ReplaceDialog::on_btReplace_clicked()
{
    QString target = ui->findText->text();
    QString to = ui->replaceText->text();

    // 校验1：查找目标为空
    if (target.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入查找目标");
        return;
    }
    // 校验2：替换内容为空
    if (to.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入替换内容");
        return;
    }

    if ((pTextEdit != nullptr)) {
        QString selText = pTextEdit->textCursor().selectedText();

        if (selText == target) {
            pTextEdit->insertPlainText(to);
        }

        on_btFindNext_clicked();
    }
}


void ReplaceDialog::on_btReplaceAll_clicked()
{
    QString target = ui->findText->text();
    QString to = ui->replaceText->text();

    // 校验1：查找目标为空
    if (target.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入查找目标");
        return;
    }
    // 校验2：替换内容为空
    if (to.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入替换内容");
        return;
    }

    if ((pTextEdit != nullptr)) {
        // 保存原始文本的副本，基于副本做替换
        QString originalText = pTextEdit->toPlainText();
        QString newText = originalText;

        newText.replace(target, to, ui->casesensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

        pTextEdit->clear();

        pTextEdit->insertPlainText(newText);
    }
}


void ReplaceDialog::on_btCancel_clicked()
{
    accept();
}

