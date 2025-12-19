#include "masterview.h"
#include "ui_masterview.h"

MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    goLoginView();
}

MasterView::~MasterView()
{
    delete ui;
}

void MasterView::goLoginView()
{
    loginView = new LoginView(this);

    pushWidgetToStackView(loginView);

    connect(loginView, SIGNAL(loginSuccess()), this, SLOT(goWelcomeView()));
}

void MasterView::goWelcomeView()
{
    welcomeView = new WelcomeView(this);

    pushWidgetToStackView(welcomeView);

    connect(welcomeView, SIGNAL(goDepartmentView()), this, SLOT(goDepartmentView()));
    connect(welcomeView, SIGNAL(goDoctorView()), this, SLOT(goDoctorView()));
    connect(welcomeView, SIGNAL(goPatientView()), this, SLOT(goPatientView()));
}

void MasterView::goDepartmentView()
{
    departmentView = new DepartmentView(this);

    pushWidgetToStackView(departmentView);
}

void MasterView::goDoctorView()
{
    doctorView = new DoctorView(this);

    pushWidgetToStackView(doctorView);
}

void MasterView::goPatientView()
{
    patientView = new PatientView(this);

    pushWidgetToStackView(patientView);

    connect(patientView, SIGNAL(goPatientEditView()), this, SLOT(goPatientEditView()));
}

void MasterView::goPatientEditView()
{
    patientEditView = new PatientEditView(this);

    pushWidgetToStackView(patientEditView);
}

void MasterView::goPreviousView()
{
    int count = ui->stackedWidget->count();

    if (count > 1) {
        // 页面数量>1时，正常返回上一页
        ui->stackedWidget->setCurrentIndex(count - 2);
        ui->labelTiltle->setText(ui->stackedWidget->currentWidget()->windowTitle());

        QWidget *widget = ui->stackedWidget->widget(count - 1);
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }
    // 新增：当页面只剩1个（登录页），点击返回直接关闭程序
    else if (count == 1) {
        this->close(); // 关闭当前窗口（即整个程序）
    }
}

void MasterView::pushWidgetToStackView(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
    int count = ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(count - 1);   // 总是显示最新加入的 view
    ui->labelTiltle->setText(widget->windowTitle());
}

void MasterView::on_btBack_clicked()
{
    goPreviousView();
}


void MasterView::on_btLogout_clicked()
{
    goPreviousView();
}


void MasterView::on_stackedWidget_currentChanged(int arg1)
{
    int count = ui->stackedWidget->count();

    if (count > 1) {
        ui->btBack->setEnabled(true);
    }
    else {
        ui->btBack->setEnabled(false);
    }

    QString title = ui->stackedWidget->currentWidget()->windowTitle();

    if (title == "欢迎") {
        ui->btLogout->setEnabled(true);
        ui->btBack->setEnabled(false);
    }
    else {
        ui->btLogout->setEnabled(false);
    }
}

