#include "masterview.h"
#include "ui_masterview.h"
#include "idatabase.h"
#include "permissionmanager.h"
#include <QMessageBox>

MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    goLoginView();

    IDatabase::getInstance();
}

MasterView::~MasterView()
{
    delete ui;
}

void MasterView::goLoginView()
{
    loginView = new LoginView(this);

    pushWidgetToStackView(loginView);

    connect(loginView, SIGNAL(loginSuccess()), this, SLOT(goMainMenuView()));
    connect(loginView, SIGNAL(goRegisterView()), this, SLOT(goRegisterView()));
}

void MasterView::goRegisterView()
{
    registerView = new RegisterView(this);

    pushWidgetToStackView(registerView);

    connect(registerView, SIGNAL(goBackToLogin()), this, SLOT(goLoginView()));
}

void MasterView::goMainMenuView()
{
    mainMenuView = new MainMenuView(this);

    pushWidgetToStackView(mainMenuView);

    connect(mainMenuView, SIGNAL(goManagementView()), this, SLOT(goManagementView()));
    connect(mainMenuView, SIGNAL(goAppointmentView()), this, SLOT(goAppointmentView()));
}

void MasterView::goManagementView()
{
    managementView = new ManagementView(this);

    pushWidgetToStackView(managementView);

    connect(managementView, SIGNAL(goDepartmentView()), this, SLOT(goDepartmentView()));
    connect(managementView, SIGNAL(goDoctorView()), this, SLOT(goDoctorView()));
    connect(managementView, SIGNAL(goPatientView()), this, SLOT(goPatientView()));
    connect(managementView, SIGNAL(goMedicineView()), this, SLOT(goMedicineView()));
}

void MasterView::goAppointmentView()
{
    appointmentView = new AppointmentView(this);

    pushWidgetToStackView(appointmentView);

    connect(appointmentView, SIGNAL(goVisitView()), this, SLOT(goVisitView()));
    connect(appointmentView, SIGNAL(goAppointmentManageView()), this, SLOT(goAppointmentManageView()));
    connect(appointmentView, SIGNAL(goScheduleView()), this, SLOT(goScheduleView()));
}

// void MasterView::goWelcomeView()
// {
//     welcomeView = new WelcomeView(this);

//     pushWidgetToStackView(welcomeView);

//     connect(welcomeView, SIGNAL(goDepartmentView()), this, SLOT(goDepartmentView()));
//     connect(welcomeView, SIGNAL(goDoctorView()), this, SLOT(goDoctorView()));
//     connect(welcomeView, SIGNAL(goPatientView()), this, SLOT(goPatientView()));
//     connect(welcomeView, SIGNAL(goVisitView()), this, SLOT(goVisitView()));
// }

void MasterView::goVisitView()
{
    if (!PermissionManager::instance().checkPermission("visit")) {
        QMessageBox::warning(this, "权限不足",
                             PermissionManager::instance().getPermissionDeniedMessage());
        return;
    }

    visitView = new VisitView(this);

    pushWidgetToStackView(visitView);

    connect(visitView, SIGNAL(goVisitEditView(int)), this, SLOT(goVisitEditView(int)));
}

void MasterView::goVisitEditView(int rowNo)
{
    visitEditView = new VisitEditView(this, rowNo);

    pushWidgetToStackView(visitEditView);

    connect(visitEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));

}

void MasterView::goDepartmentView()
{
    if (!PermissionManager::instance().checkPermission("department")) {
        QMessageBox::warning(this, "权限不足",
                             PermissionManager::instance().getPermissionDeniedMessage());
        return;
    }

    departmentView = new DepartmentView(this);

    pushWidgetToStackView(departmentView);

    connect(departmentView, SIGNAL(goDepartmentEditView(int)), this, SLOT(goDepartmentEditView(int)));
}

void MasterView::goDepartmentEditView(int rowNo)
{
    departmentEditView = new DepartmentEditView(this, rowNo);

    pushWidgetToStackView(departmentEditView);

    connect(departmentEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goDoctorView()
{
    if (!PermissionManager::instance().checkPermission("doctor")) {
        QMessageBox::warning(this, "权限不足",
                             PermissionManager::instance().getPermissionDeniedMessage());
        return;
    }

    doctorView = new DoctorView(this);

    pushWidgetToStackView(doctorView);

    connect(doctorView, SIGNAL(goDoctorEditView(int)), this, SLOT(goDoctorEditView(int)));
}

void MasterView::goDoctorEditView(int rowNo)
{
    doctorEditView = new class doctorEditView(this, rowNo);

    pushWidgetToStackView(doctorEditView);

    connect(doctorEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goPatientView()
{
    if (!PermissionManager::instance().checkPermission("patient")) {
        QMessageBox::warning(this, "权限不足",
                             PermissionManager::instance().getPermissionDeniedMessage());
        return;
    }

    patientView = new PatientView(this);

    pushWidgetToStackView(patientView);

    connect(patientView, SIGNAL(goPatientEditView(int)), this, SLOT(goPatientEditView(int)));
}

void MasterView::goPatientEditView(int rowNo)
{
    patientEditView = new PatientEditView(this, rowNo);

    pushWidgetToStackView(patientEditView);

    connect(patientEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
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
    } else {
        ui->btBack->setEnabled(false);
    }

    QString title = ui->stackedWidget->currentWidget()->windowTitle();

    if (title == "欢迎") {
        ui->btLogout->setEnabled(true);
        ui->btBack->setEnabled(false);
    } else {
        ui->btLogout->setEnabled(false);
    }
}

