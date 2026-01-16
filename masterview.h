#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include "loginview.h"
#include "registerview.h"
#include "mainmenuview.h"
#include "managementview.h"
#include "appointmentview.h"
// #include "welcomeview.h"
#include "visitview.h"
#include "visiteditview.h"
#include "departmentview.h"
#include "departmenteditview.h"
#include "doctorview.h"
#include "doctoreditview.h"
#include "patientview.h"
#include "patienteditview.h"
#include "prescriptionview.h"
#include "medicineview.h"
#include "medicineeditview.h"
#include "stockview.h"
#include "prescription_detail_view.h"

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
    void goRegisterView();
    void goMainMenuView();
    void goManagementView();
    void goAppointmentView();
    // void goWelcomeView();
    void goVisitView();
    void goVisitEditView(int rowNo);
    void goDepartmentView();
    void goDepartmentEditView(int rowNo);
    void goDoctorView();
    void goDoctorEditView(int rowNo);
    void goPatientView();
    void goPatientEditView(int rowNo);
    void goPrescriptionView();
    void goMedicineView();
    void goMedicineEditView(int rowNo);
    void goStockView();
    void goPrescription_Detail_View();
    void goPreviousView();

private slots:
    void on_btBack_clicked();

    void on_btLogout_clicked();

    void on_stackedWidget_currentChanged(int arg1);

private:
    void pushWidgetToStackView(QWidget *widget);

    Ui::MasterView *ui;

    LoginView *loginView;
    RegisterView *registerView;
    MainMenuView *mainMenuView;
    ManagementView *managementView;
    AppointmentView *appointmentView;
    // WelcomeView *welcomeView;
    VisitView *visitView;
    VisitEditView *visitEditView;
    DepartmentView *departmentView;
    DepartmentEditView *departmentEditView;
    DoctorView *doctorView;
    doctorEditView *doctorEditView;
    PatientView *patientView;
    PatientEditView *patientEditView;
    PrescriptionView *prescriptionView;
    MedicineView *medicineView;
    MedicineEditView *medicineEditView;
    StockView *stockView;
    Prescription_Detail_View *prescription_Detail_View;
};
#endif // MASTERVIEW_H
