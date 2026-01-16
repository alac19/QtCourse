#include "medicineview.h"
#include "ui_medicineview.h"
#include "idatabase.h"
#include <QMessageBox>

MedicineView::MedicineView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MedicineView)
{
    ui->setupUi(this);

    // 设置表格属性
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    // 初始化药品模型
    IDatabase &iDatabase = IDatabase::getInstance();
    if(iDatabase.initMedicineModel()) {
        ui->tableView->setModel(iDatabase.medicineTabModel);
        ui->tableView->setSelectionModel(iDatabase.theMedicineSelection);
    }
}

MedicineView::~MedicineView()
{
    delete ui;
}

void MedicineView::on_btSearch_clicked()
{
    QString filter = QString("NAME like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchMedicine(filter);
}


void MedicineView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewMedicine();
    emit goMedicineEditView(currow);
}


void MedicineView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentMedicine();
}


void MedicineView::on_btModify_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().theMedicineSelection->currentIndex();
    if (curIndex.isValid()) {
        emit goMedicineEditView(curIndex.row());
    }
}


void MedicineView::on_btStockAlert_clicked()
{
    // // 简单显示库存警告：库存小于最低库存的药品
    // IDatabase &iDatabase = IDatabase::getInstance();
    // QString filter = "STOCK_QUANTITY < MIN_STOCK";
    // if (iDatabase.searchMedicine(filter)) {
    //     int lowStockCount = iDatabase.medicineTabModel->rowCount();
    //     QMessageBox::warning(this, "库存警告",
    //                          QString("有 %1 种药品库存低于最低库存").arg(lowStockCount));
    // } else {
    //     QMessageBox::information(this, "库存状态", "所有药品库存正常");
    // }
    // 获取数据库实例
    IDatabase &iDatabase = IDatabase::getInstance();

    // 查询库存低于最低库存的药品
    QString filter = "STOCK_QUANTITY < MIN_STOCK";
    if (iDatabase.searchMedicine(filter)) {
        int lowStockCount = iDatabase.medicineTabModel->rowCount();

        if (lowStockCount > 0) {
            // 构建包含具体药品名称的警告信息
            QString warningMessage = QString("有 %1 种药品库存低于最低库存：\n\n").arg(lowStockCount);

            // 遍历低库存药品，获取药品名称和库存信息
            for (int i = 0; i < lowStockCount; ++i) {
                // 获取药品名称
                QModelIndex nameIndex = iDatabase.medicineTabModel->index(i,
                                        iDatabase.medicineTabModel->fieldIndex("NAME"));
                QString medicineName = iDatabase.medicineTabModel->data(nameIndex).toString();

                // 获取当前库存
                QModelIndex stockIndex = iDatabase.medicineTabModel->index(i,
                                         iDatabase.medicineTabModel->fieldIndex("STOCK_QUANTITY"));
                int currentStock = iDatabase.medicineTabModel->data(stockIndex).toInt();

                // 获取最低库存
                QModelIndex minStockIndex = iDatabase.medicineTabModel->index(i,
                                            iDatabase.medicineTabModel->fieldIndex("MIN_STOCK"));
                int minStock = iDatabase.medicineTabModel->data(minStockIndex).toInt();

                // 添加药品信息到警告消息
                warningMessage += QString("%1. %2 (库存: %3, 最低库存: %4)\n")
                                  .arg(i + 1)
                                  .arg(medicineName)
                                  .arg(currentStock)
                                  .arg(minStock);
            }

            // 显示详细警告信息
            QMessageBox::warning(this, "库存警告", warningMessage);
        } else {
            QMessageBox::information(this, "库存状态", "所有药品库存正常");
        }
    } else {
        QMessageBox::information(this, "库存状态", "所有药品库存正常");
    }
}

