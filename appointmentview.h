#ifndef APPOINTMENTVIEW_H
#define APPOINTMENTVIEW_H

#include <QWidget>

namespace Ui
{
class AppointmentView;
}

class AppointmentView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentView(QWidget *parent = nullptr);
    ~AppointmentView();

private slots:
    void on_btVisit_clicked();

    void on_btAppointment_clicked();

    void on_btSchedule_clicked();

signals:
    void goVisitView();
    void goAppointmentManageView();
    void goScheduleView();

private:
    Ui::AppointmentView *ui;
};

#endif // APPOINTMENTVIEW_H
