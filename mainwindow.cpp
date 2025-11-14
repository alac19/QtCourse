#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutDialog.h"
#include "findDialog.h"
#include "replacedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    statusLabel.setMaximumWidth(150);
    statusLabel.setText("length:" + QString::number(0) + "   lines:" + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    statusCursorLabel.setMaximumWidth(150);
    statusCursorLabel.setText("LN:" + QString::number(0) + "   Col:" + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText("Alac");
    ui->statusbar->addPermanentWidget(author);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    aboutDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionFind_triggered()
{
    findDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionNew_triggered()
{
    ui->TextEdit->clear();

    this->setWindowTitle(tr("新建文本文件 -- 灵动编辑"));
}


void MainWindow::on_actionOpen_triggered()
{
    // 依旧先清理编辑器内容
    ui->TextEdit->clear();

    QString filename = QFileDialog::getOpenFileName(this, "打开文件", ".", tr("Text files(*.txt) ;; All(*.*)"));

    QFile file(filename);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "..", "打开文件失败！");
        return;
    }

    filePath = filename;

    // 读出文件
    QTextStream in(&file);
    QString text = in.readAll();
    ui->TextEdit->insertPlainText(text);
    file.close();

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());
}


void MainWindow::on_actionSave_triggered()
{
    QFile file(filePath);

    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "..", "打开文件失败！");

        QString filename = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text files(*.txt)"));

        QFile file(filename);

        if(!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "..", "打开保存文件失败！");
            return;
        }

        filePath = filename;
    }

    // 读入文件
    QTextStream out(&file);
    QString text = ui->TextEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
}


void MainWindow::on_actionSaveAs_triggered()
{

}

