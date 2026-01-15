#ifndef MAINMENUVIEW_H
#define MAINMENUVIEW_H

#include <QWidget>

namespace Ui
{
class MainMenuView;
}

class MainMenuView : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuView(QWidget *parent = nullptr);
    ~MainMenuView();

private slots:
    void on_btManagement_clicked();

    void on_btAppointment_clicked();

signals:
    void goManagementView();   // 进入管理视图
    void goAppointmentView();  // 进入就诊排班视图

private:
    Ui::MainMenuView *ui;
};

#endif // MAINMENUVIEW_H
