#ifndef MEDICINEVIEW_H
#define MEDICINEVIEW_H

#include <QWidget>
#include <QtSql>
#include <QSqlDatabase>

namespace Ui
{
class MedicineView;
}

class MedicineView : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineView(QWidget *parent = nullptr);
    ~MedicineView();

signals:
    void goMedicineEditView(int idx);

private slots:
    void on_btSearch_clicked();

    void on_btAdd_clicked();

    void on_btDelete_clicked();

    void on_btModify_clicked();

    void on_btImport_clicked();

    void on_btExport_clicked();

    void on_btStockAlert_clicked();


private:
    Ui::MedicineView *ui;
};

#endif // MEDICINEVIEW_H
