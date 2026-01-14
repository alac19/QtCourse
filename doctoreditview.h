#ifndef DOCTOREDITVIEW_H
#define DOCTOREDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>

namespace Ui
{
class doctorEditView;
}

class doctorEditView : public QWidget
{
    Q_OBJECT

public:
    explicit doctorEditView(QWidget *parent = nullptr, int index = 0);
    ~doctorEditView();

private slots:
    void on_btSave_clicked();

    void on_btCancel_clicked();

    void onDepartmentChanged(int index);  // 新增

private:
    Ui::doctorEditView *ui;
    QDataWidgetMapper *dataMapper;   // 数据映射

    // 新增
    int currentIndex;

    void initComboBoxes();  // 初始化下拉框
    void loadDoctorData(int row);  // 加载医生数据

signals:
    void goPreviousView();
};

#endif // DOCTOREDITVIEW_H
