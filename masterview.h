#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include "loginview.h"
#include "welcomeview.h"
#include "departmentview.h"
#include "doctorview.h"
#include "doctoreditview.h"
#include "patientview.h"
#include "patienteditview.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MasterView;
}
QT_END_NAMESPACE

class MasterView : public QWidget
{
    Q_OBJECT

public:
    MasterView(QWidget *parent = nullptr);
    ~MasterView();

public slots:
    void goLoginView();
    void goWelcomeView();
    void goDepartmentView();
    void goDoctorView();
    void goDoctorEditView(int rowNo);
    void goPatientView();
    void goPatientEditView(int rowNo);
    void goPreviousView();

private slots:
    void on_btBack_clicked();

    void on_btLogout_clicked();

    void on_stackedWidget_currentChanged(int arg1);

private:
    void pushWidgetToStackView(QWidget *widget);

    Ui::MasterView *ui;

    LoginView *loginView;
    WelcomeView *welcomeView;
    DepartmentView *departmentView;
    DoctorView *doctorView;
    doctorEditView *doctorEditView;
    PatientView *patientView;
    PatientEditView *patientEditView;
};
#endif // MASTERVIEW_H
