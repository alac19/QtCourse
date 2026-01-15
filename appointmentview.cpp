#include "appointmentview.h"
#include "ui_appointmentview.h"

AppointmentView::AppointmentView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppointmentView)
{
    ui->setupUi(this);
}

AppointmentView::~AppointmentView()
{
    delete ui;
}

void AppointmentView::on_btVisit_clicked()
{
    emit goVisitView();
}


void AppointmentView::on_btAppointment_clicked()
{
    emit goAppointmentManageView();
}


void AppointmentView::on_btSchedule_clicked()
{
    emit goScheduleView();
}

