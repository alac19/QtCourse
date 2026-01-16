#ifndef MEDICINEEDITVIEW_H
#define MEDICINEEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>

namespace Ui
{
class MedicineEditView;
}

class MedicineEditView : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineEditView(QWidget *parent = nullptr, int index = 0);
    ~MedicineEditView();

private slots:
    void on_btSave_clicked();

    void on_btCancel_clicked();

private:
    Ui::MedicineEditView *ui;
    QDataWidgetMapper *dataMapper;   // 数据映射

signals:
    void goPreviousView();
};

#endif // MEDICINEEDITVIEW_H
