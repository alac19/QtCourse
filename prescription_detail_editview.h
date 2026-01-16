#ifndef PRESCRIPTION_DETAIL_EDITVIEW_H
#define PRESCRIPTION_DETAIL_EDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>

namespace Ui
{
class Prescription_Detail_EditView;
}

class Prescription_Detail_EditView : public QWidget
{
    Q_OBJECT

public:
    explicit Prescription_Detail_EditView(QWidget *parent = nullptr);
    ~Prescription_Detail_EditView();

signals:
    void goPreviousView();
    void prescriptionSaved(const QString &prescriptionId);

private slots:
    void on_btSave_clicked();

    void on_btCancel_clicked();

    void on_btAddMedicine_clicked();

    void on_btremoveMedicine_clicked();

    void on_comboMedicine_currentIndexChanged(int index);

    void on_spinQuantity_valueChanged(int arg1);

private:
    Ui::Prescription_Detail_EditView *ui;
    QString m_prescriptionId;
    bool m_isEditMode;

    void loadMedicineList();
    // 添加实际会实现的函数
    void updateMedicineTable();
};

#endif // PRESCRIPTION_DETAIL_EDITVIEW_H
