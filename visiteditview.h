#ifndef VISITEDITVIEW_H
#define VISITEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>

namespace Ui
{
class VisitEditView;
}

class VisitEditView : public QWidget
{
    Q_OBJECT

public:
    explicit VisitEditView(QWidget *parent = nullptr, int index = 0);
    ~VisitEditView();

private slots:
    void on_btSave_clicked();

    void on_btCancel_clicked();

    void onDepartmentChanged(int index);

private:
    void initComboBoxes();  // 初始化下拉框
    void loadDoctorByDepartment(const QString &departmentId); // 根据科室加载医生
    void loadRecord(int row);

private:
    Ui::VisitEditView *ui;
    QDataWidgetMapper *dataMapper;   // 数据映射
    int currentIndex;

signals:
    void goPreviousView();
};

#endif // VISITEDITVIEW_H
