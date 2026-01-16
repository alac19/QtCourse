#ifndef STOCKOPERATIONDIALOG_H
#define STOCKOPERATIONDIALOG_H

#include <QDialog>

namespace Ui
{
class StockOperationDialog;
}

class StockOperationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StockOperationDialog(QWidget *parent, const QString &medicineId,
                                  const QString &medicineName, const QString &operationType);
    ~StockOperationDialog();

    int getQuantity() const;
    QString getRemark() const;

private slots:
    void on_btConfirm_clicked();

    void on_btCancel_clicked();

private:
    Ui::StockOperationDialog *ui;
    QString medicineId;
    QString medicineName;
    QString operationType;
};

#endif // STOCKOPERATIONDIALOG_H
