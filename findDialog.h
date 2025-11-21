#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>

namespace Ui {
class findDialog;
}

class findDialog : public QDialog
{
    Q_OBJECT

public:
    explicit findDialog(QWidget *parent = nullptr, QPlainTextEdit* textEdit = nullptr);
    ~findDialog();

private slots:
    void on_btFindNext_clicked();

    void on_btCancel_clicked();

private:
    Ui::findDialog *ui;
    QPlainTextEdit* pTextEdit;
};

#endif // FINDDIALOG_H
