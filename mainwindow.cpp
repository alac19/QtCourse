#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutDialog.h"
#include "findDialog.h"
#include "replacedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QColorDialog>
#include <QFontDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textChange = false;

    statusLabel.setMaximumWidth(150);
    statusLabel.setText("length:" + QString::number(0) + "   lines:" + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    statusCursorLabel.setMaximumWidth(150);
    statusCursorLabel.setText("LN:" + QString::number(0) + "   Col:" + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText("Alac");
    ui->statusbar->addPermanentWidget(author);

    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionPaste->setEnabled(false);

    QPlainTextEdit::LineWrapMode mode = ui->TextEdit->lineWrapMode();

    if (mode == QTextEdit::NoWrap) {
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        ui->actionTextWrap->setChecked(false);
    }
    else {
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actionTextWrap->setChecked(true);
    }
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
    if (!userEditConfirmed()) {
        return;
    }

    filePath = "";

    ui->TextEdit->clear();

    this->setWindowTitle(tr("新建文本文件 -- 灵动编辑"));

    textChange = false;
}


void MainWindow::on_actionOpen_triggered()
{
    if (!userEditConfirmed()) {
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this, "打开文件", ".", tr("Text files(*.txt) ;; All(*.*)"));

    // 检查用户是否取消了操作
    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "..", "打开文件失败！");
        return;
    }

    // 先清理编辑器内容
    ui->TextEdit->clear();

    filePath = filename;

    // 读出文件
    QTextStream in(&file);
    QString text = in.readAll();
    ui->TextEdit->insertPlainText(text);
    file.close();

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());

    textChange = false;
}


void MainWindow::on_actionSave_triggered()
{
    if (filePath == "") {
        QString filename = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text files(*.txt)"));

        // 检查用户是否取消了操作
        if (filename.isEmpty()) {
            return;
        }

        QFile file(filename);

        if(!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "..", "打开保存文件失败！");
            return;
        }

        file.close();
        filePath = filename;
    }
    // QFile file(filePath);

    // if(!file.open(QFile::WriteOnly | QFile::Text)) {
    //     QMessageBox::warning(this, "..", "打开文件失败！");

    //     QString filename = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text files(*.txt)"));

    //     QFile file(filename);

    //     if(!file.open(QFile::WriteOnly | QFile::Text)) {
    //         QMessageBox::warning(this, "..", "打开保存文件失败！");
    //         return;
    //     }

    //     filePath = filename;
    // }

    QFile file(filePath);

    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "..", "打开保存文件失败！");
        return;
    }

    // 读入文件
    QTextStream out(&file);
    QString text = ui->TextEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    textChange = false;
}


void MainWindow::on_actionSaveAs_triggered()
{
    // 获取当前文件的名称作为默认文件名
    QString defaultName;
    if (!filePath.isEmpty()) {
        // 如果已经有文件路径，提取文件名
        QFileInfo fileInfo(filePath);
        defaultName = fileInfo.fileName();
    } else {
        // 如果没有文件路径，使用默认名称
        defaultName = "无标题.txt";
    }

    // QString filename = QFileDialog::getSaveFileName(this, "另存文件", ".", tr("Text files(*.txt)"));

    QString filename = QFileDialog::getSaveFileName(this, "另存文件", defaultName, tr("Text files(*.txt)"));

    if (filename.isEmpty()) {
        return; // 用户取消了操作
    }

    QFile file(filename);

    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "..", "打开保存文件失败！");
        return;
    }

    // filePath = filename;

    // 读入文件
    QTextStream out(&file);
    QString text = ui->TextEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    // this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    // textChange = false;
}


void MainWindow::on_TextEdit_textChanged()
{
    if (!textChange) {
        QString currentTitle = this->windowTitle();
        // 检查标题是否已经有星号，避免重复添加
        if (!currentTitle.startsWith("*")) {
            this->setWindowTitle("*" + currentTitle);
        }

        textChange = true;
    }
}

bool MainWindow::userEditConfirmed()
{
    if (textChange) {
        QString path = (filePath != "") ? filePath : "无标题.txt";

        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("...");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存到\n") + "\"" + path + "\"?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        int r = msg.exec();

        switch (r) {
        case QMessageBox::Yes:
            on_actionSave_triggered();
            // 如果保存操作被取消，返回false
            if (textChange) {
                return false;
            }
            break;
        case QMessageBox::No:
            textChange = false;
            break;
        case QMessageBox::Cancel:
            return false;
        }
    }

    return true;
}

void MainWindow::on_actionUndo_triggered()
{
    ui->TextEdit->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->TextEdit->redo();
}


void MainWindow::on_actionCut_triggered()
{
    ui->TextEdit->cut();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionCopy_triggered()
{
    ui->TextEdit->copy();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionPaste_triggered()
{
    ui->TextEdit->paste();
}


void MainWindow::on_TextEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);
}


void MainWindow::on_TextEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}


void MainWindow::on_TextEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}


void MainWindow::on_actionTextWrap_triggered()
{
    QPlainTextEdit::LineWrapMode mode = ui->TextEdit->lineWrapMode();

    if (mode == QTextEdit::NoWrap) {
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        ui->actionTextWrap->setChecked(true);
    }
    else {
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actionTextWrap->setChecked(false);
    }
}


void MainWindow::on_actionFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, this);

    if (ok) {
        ui->TextEdit->setFont(font);
    }
}


void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");

    if(color.isValid()) {
        ui->TextEdit->setStyleSheet(QString("QPlainTextEdit {color: %1}").arg(color.name()));
    }
}


void MainWindow::on_actionEditorColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");

    if(color.isValid()) {
        ui->TextEdit->setStyleSheet(QString("QPlainTextEdit {background-color: %1}").arg(color.name()));
    }
}

