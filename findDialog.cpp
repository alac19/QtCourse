#include "findDialog.h"
#include "ui_findDialog.h"

findDialog::findDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::findDialog)
{
    ui->setupUi(this);
}

findDialog::~findDialog()
{
    delete ui;
}
