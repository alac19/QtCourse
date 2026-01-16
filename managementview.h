#ifndef MANAGEMENTVIEW_H
#define MANAGEMENTVIEW_H

#include <QWidget>

namespace Ui
{
class ManagementView;
}

class ManagementView : public QWidget
{
    Q_OBJECT

public:
    explicit ManagementView(QWidget *parent = nullptr);
    ~ManagementView();

private slots:
    void on_btDepartment_clicked();

    void on_btDoctor_clicked();

    void on_btPatient_clicked();

    void on_btPrescription_clicked();

signals:
    void goDepartmentView();
    void goDoctorView();
    void goPatientView();
    void goPrescriptionView();

private:
    Ui::ManagementView *ui;
};

#endif // MANAGEMENTVIEW_H
