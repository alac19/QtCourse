#include "stockadjustdialog.h"
#include "ui_stockadjustdialog.h"
#include <QMessageBox>

StockAdjustDialog::StockAdjustDialog(QWidget *parent, const QString &medicineId,
                                     const QString &medicineName, int currentStock) :
    QDialog(parent),
    ui(new Ui::StockAdjustDialog),
    medicineId(medicineId),
    medicineName(medicineName),
    currentStock(currentStock)
{
    ui->setupUi(this);

    // 设置窗口标题
    this->setWindowTitle("调整库存");

    // 显示药品信息
    ui->lbMedicineInfo->setText(QString("药品：%1").arg(medicineName));
    ui->lbCurrentStock->setText(QString("当前库存：%1").arg(currentStock));

    // 设置新库存的范围
    ui->spnNewStock->setMinimum(0);
    ui->spnNewStock->setMaximum(1000000);
    ui->spnNewStock->setValue(currentStock);

    // 设置调整量的显示（初始为0）
    ui->lbAdjustment->setText("调整量：0");

    // 连接信号槽，当新库存值改变时更新调整量
    connect(ui->spnNewStock, QOverload<int>::of(&QSpinBox::valueChanged),
    [this](int value) {
        int adjustment = value - this->currentStock;
        QString adjustmentText = QString("调整量：%1").arg(adjustment);
        if (adjustment > 0) {
            adjustmentText += " (增加)";
        } else if (adjustment < 0) {
            adjustmentText += " (减少)";
        }
        ui->lbAdjustment->setText(adjustmentText);
    });
}

StockAdjustDialog::~StockAdjustDialog()
{
    delete ui;
}

int StockAdjustDialog::getNewStock() const
{
    if (ui->spnNewStock) {
        return ui->spnNewStock->value();
    }
    return currentStock;
}

QString StockAdjustDialog::getRemark() const
{
    if (ui->txtRemark) {
        return ui->txtRemark->toPlainText().trimmed();
    }
    return QString();
}

void StockAdjustDialog::on_btConfirm_clicked()
{
    int newStock = getNewStock();

    // 验证新库存值
    if (newStock < 0) {
        QMessageBox::warning(this, "警告", "库存不能为负数");
        return;
    }

    // 如果新库存与当前库存相同，提示用户
    if (newStock == currentStock) {
        int result = QMessageBox::question(this, "确认",
                                           "新库存值与当前库存相同，是否继续？",
                                           QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No) {
            return;
        }
    }

    accept();
}


void StockAdjustDialog::on_btCancel_clicked()
{
    reject();
}

