#include "mainmenuview.h"
#include "ui_mainmenuview.h"

MainMenuView::MainMenuView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMenuView)
{
    ui->setupUi(this);
}

MainMenuView::~MainMenuView()
{
    delete ui;
}

void MainMenuView::on_btManagement_clicked()
{
    emit goManagementView();
}


void MainMenuView::on_btAppointment_clicked()
{
    emit goAppointmentView();
}

