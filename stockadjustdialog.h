#ifndef STOCKADJUSTDIALOG_H
#define STOCKADJUSTDIALOG_H

#include <QDialog>

namespace Ui
{
class StockAdjustDialog;
}

class StockAdjustDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StockAdjustDialog(QWidget *parent, const QString &medicineId,
                               const QString &medicineName, int currentStock);
    ~StockAdjustDialog();

    int getNewStock() const;
    QString getRemark() const;

private slots:
    void on_btConfirm_clicked();

    void on_btCancel_clicked();

private:
    Ui::StockAdjustDialog *ui;
    QString medicineId;
    QString medicineName;
    int currentStock;
};

#endif // STOCKADJUSTDIALOG_H
