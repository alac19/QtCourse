#include "loginview.h"
#include "ui_loginview.h"
#include "idatabase.h"
#include <QMessageBox>
#include "registerview.h"

LoginView::LoginView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginView)
{
    ui->setupUi(this);
}

LoginView::~LoginView()
{
    delete ui;
}

void LoginView::on_btSignIn_clicked()
{
    QString status = IDatabase::getInstance().userLogin(ui->inputUserName->text(), ui->inputUserPassword->text());

    if (status == "loginOk") {
        emit loginSuccess();
    } else if (status == "wrongPassword") {
        QMessageBox::warning(this, "登录失败", "密码错误！");
    } else if (status == "wrongUsername") {
        QMessageBox::warning(this, "登录失败", "用户名不存在！");
    }
}


void LoginView::on_btSignUp_clicked()
{
    emit goRegisterView();
}

