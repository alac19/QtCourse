#include "prescription_detail_editview.h"
#include "ui_prescription_detail_editview.h"
#include <QMessageBox>
#include <QSqlQuery>

Prescription_Detail_EditView::Prescription_Detail_EditView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Prescription_Detail_EditView)
    , m_prescriptionId("")
    , m_isEditMode(true)
{
    ui->setupUi(this);

    // 设置窗口标题
    this->setWindowTitle(tr("处方编辑"));

    // 加载药品列表
    loadMedicineList();
}

Prescription_Detail_EditView::~Prescription_Detail_EditView()
{
    delete ui;
}

void Prescription_Detail_EditView::on_btSave_clicked()
{
    // 简化保存逻辑：只显示消息，不实际保存到数据库
    // TODO: 处方保存功能待实现（实验扩展）

    // 检查是否有药品
    QListWidget *listMedicines = this->findChild<QListWidget*>("listMedicines");
    if (!listMedicines || listMedicines->count() == 0) {
        QMessageBox::warning(this, "警告", "请至少添加一个药品！");
        return;
    }

    // 模拟保存成功
    QMessageBox::information(this, "成功", "处方保存成功！\n\n注意：此功能为演示版本，实际保存功能需后续实现。");
    emit goPreviousView();
}


void Prescription_Detail_EditView::on_btCancel_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("确认取消"),
                                  tr("确定要取消编辑吗？所有未保存的更改将会丢失。"),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        emit goPreviousView();
    }
}


void Prescription_Detail_EditView::on_btAddMedicine_clicked()
{
    // 使用findChild查找控件，避免编译错误
    QComboBox *comboMedicine = this->findChild<QComboBox*>("comboMedicine");
    if (!comboMedicine) {
        comboMedicine = this->findChild<QComboBox*>("comboBoxMedicine");
    }

    QListWidget *listMedicines = this->findChild<QListWidget*>("listMedicines");
    QSpinBox *spinQuantity = this->findChild<QSpinBox*>("spinQuantity");

    if (!comboMedicine || !listMedicines || !spinQuantity) {
        QMessageBox::warning(this, tr("错误"), tr("界面控件未正确初始化！"));
        return;
    }

    QString medicineName = comboMedicine->currentText();
    int quantity = spinQuantity->value();

    if (medicineName.isEmpty() || quantity <= 0) {
        QMessageBox::warning(this, tr("警告"), tr("请选择药品并输入数量！"));
        return;
    }

    // 添加到药品列表
    QString item = QString("%1 × %2").arg(medicineName).arg(quantity);
    listMedicines->addItem(item);

    // 清空输入
    spinQuantity->setValue(1);
}


void Prescription_Detail_EditView::on_btremoveMedicine_clicked()
{
    QListWidget *listMedicines = this->findChild<QListWidget*>("listMedicines");
    if (!listMedicines) {
        QMessageBox::warning(this, tr("错误"), tr("药品列表控件不存在！"));
        return;
    }

    // 删除选中的药品
    int row = listMedicines->currentRow();
    if (row >= 0) {
        delete listMedicines->takeItem(row);
    } else {
        QMessageBox::warning(this, tr("提示"), tr("请先选择要删除的药品！"));
    }
}


void Prescription_Detail_EditView::on_comboMedicine_currentIndexChanged(int index)
{
    QComboBox *comboMedicine = this->findChild<QComboBox*>("comboMedicine");
    if (!comboMedicine) {
        comboMedicine = this->findChild<QComboBox*>("comboBoxMedicine");
    }

    QLabel *labelMedicineInfo = this->findChild<QLabel*>("labelMedicineInfo");

    if (comboMedicine && labelMedicineInfo && index >= 0) {
        QString medicineName = comboMedicine->itemText(index);
        labelMedicineInfo->setText(QString("已选择: %1").arg(medicineName));
    }
}


void Prescription_Detail_EditView::on_spinQuantity_valueChanged(int arg1)
{
    QLabel *labelQuantityInfo = this->findChild<QLabel*>("labelQuantityInfo");
    if (labelQuantityInfo) {
        labelQuantityInfo->setText(QString("数量: %1").arg(arg1));
    }
}

void Prescription_Detail_EditView::loadMedicineList()
{
    // 从数据库加载药品列表
    QSqlQuery query("SELECT NAME FROM medicine ORDER BY NAME");
    ui->comboMedicine->clear();

    while (query.next()) {
        ui->comboMedicine->addItem(query.value(0).toString());
    }

    if (ui->comboMedicine->count() == 0) {
        ui->comboMedicine->addItem("测试药品1");
        ui->comboMedicine->addItem("测试药品2");
        ui->comboMedicine->addItem("测试药品3");
    }
}

void Prescription_Detail_EditView::updateMedicineTable()
{

}

