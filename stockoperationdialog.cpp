#include "stockoperationdialog.h"
#include "ui_stockoperationdialog.h"
#include <QMessageBox>

StockOperationDialog::StockOperationDialog(QWidget *parent, const QString &medicineId,
        const QString &medicineName, const QString &operationType) :
    QDialog(parent),
    ui(new Ui::StockOperationDialog),
    medicineId(medicineId),
    medicineName(medicineName),
    operationType(operationType)
{
    ui->setupUi(this);

    // 设置窗口标题
    QString title;
    if (operationType == "IN") {
        title = "药品入库";
        ui->lbOperation->setText("入库数量：");
    } else if (operationType == "OUT") {
        title = "药品出库";
        ui->lbOperation->setText("出库数量：");
    } else {
        title = "库存操作";
    }
    this->setWindowTitle(title);

    // 显示药品信息
    ui->lbMedicineInfo->setText(QString("药品信息：%1 (ID: %2)").arg(medicineName).arg(medicineId));

    // 设置数量范围
    if (ui->spnQuantity) {
        ui->spnQuantity->setMinimum(1);
        ui->spnQuantity->setMaximum(10000);
        ui->spnQuantity->setValue(1);
    }
}

StockOperationDialog::~StockOperationDialog()
{
    delete ui;
}

int StockOperationDialog::getQuantity() const
{
    if (ui->spnQuantity) {
        return ui->spnQuantity->value();
    }
    return 0;
}

QString StockOperationDialog::getRemark() const
{
    if (ui->txtRemark) {
        return ui->txtRemark->toPlainText().trimmed();
    }
    return QString();
}

void StockOperationDialog::on_btConfirm_clicked()
{
    int quantity = getQuantity();

    if (quantity <= 0) {
        QMessageBox::warning(this, "警告", "数量必须大于0");
        return;
    }

    accept();
}


void StockOperationDialog::on_btCancel_clicked()
{
    reject();
}

