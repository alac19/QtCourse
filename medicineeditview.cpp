#include "medicineeditview.h"
#include "ui_medicineeditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QMessageBox>

MedicineEditView::MedicineEditView(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::MedicineEditView)
{
    ui->setupUi(this);

    // 设置数据映射
    dataMapper = new QDataWidgetMapper(this);
    QSqlTableModel *tabModel = IDatabase::getInstance().medicineTabModel;
    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    // 映射字段到界面控件
    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("MEDICINE_ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditCategory, tabModel->fieldIndex("CATEGORY"));
    dataMapper->addMapping(ui->dbEditSpecification, tabModel->fieldIndex("SPECIFICATION"));
    dataMapper->addMapping(ui->dbEditUnit, tabModel->fieldIndex("UNIT"));
    dataMapper->addMapping(ui->dbEditPrice, tabModel->fieldIndex("PRICE"));
    dataMapper->addMapping(ui->dbEditStock, tabModel->fieldIndex("STOCK_QUANTITY"));
    dataMapper->addMapping(ui->dbEditMinStock, tabModel->fieldIndex("MIN_STOCK"));
    dataMapper->addMapping(ui->dbEditManu, tabModel->fieldIndex("MANUFACTURER"));
    dataMapper->addMapping(ui->dateEditPro, tabModel->fieldIndex("PRODUCTION_DATE"));
    dataMapper->addMapping(ui->dateEditExp, tabModel->fieldIndex("EXPIRY_DATE"));
    dataMapper->addMapping(ui->dbEditIs, tabModel->fieldIndex("IS_PRESCRIPTION"));
    dataMapper->addMapping(ui->dateTimeCre, tabModel->fieldIndex("CREATEDTIMESTAMP"));
    dataMapper->addMapping(ui->dateTimeUpd, tabModel->fieldIndex("UPDATEDTIMESTAMP"));

    dataMapper->setCurrentIndex(index);

    // 重要：对于新增记录，手动设置创建日期
    if (index >= 0 && index < tabModel->rowCount()) {
        // 修改记录：检查是否有有效的日期，如果没有则设置默认值
        QDate createDate = tabModel->record(index).value("CREATEDTIMESTAMP").toDate();
        if (!createDate.isValid()) {
            ui->dateTimeCre->setDateTime(QDateTime::currentDateTime());
            ui->dateTimeUpd->setDateTime(QDateTime::currentDateTime());
        }
    }
}

MedicineEditView::~MedicineEditView()
{
    delete ui;
}

void MedicineEditView::on_btSave_clicked()
{
    // 获取当前行
    int row = dataMapper->currentIndex();
    QSqlTableModel *model = IDatabase::getInstance().medicineTabModel;

    if (row >= 0) {
        // 更新修改时间
        QModelIndex idx = model->index(row, model->fieldIndex("UPDATEDTIMESTAMP"));
        model->setData(idx, QDateTime::currentDateTime());

        // 检查必填字段
        QString name = ui->dbEditName->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, "警告", "药品名称不能为空");
            return;
        }

        // 检查生产日期和到期日期的合理性
        QDate productionDate = ui->dateEditPro->date();
        QDate expiryDate = ui->dateEditExp->date();

        if (productionDate.isValid() && expiryDate.isValid() && productionDate > expiryDate) {
            QMessageBox::warning(this, "警告", "生产日期不能晚于到期日期");
            return;
        }

        // 检查库存和价格
        double price = ui->dbEditPrice->text().toDouble();
        if (price < 0) {
            QMessageBox::warning(this, "警告", "价格不能为负数");
            return;
        }

        int stock = ui->dbEditStock->text().toInt();
        int minStock = ui->dbEditMinStock->text().toInt();
        if (stock < 0 || minStock < 0) {
            QMessageBox::warning(this, "警告", "库存和最低库存不能为负数");
            return;
        }
    }

    // 手动提交QDataWidgetMapper的更改
    dataMapper->submit();

    // 再提交更改到数据库
    if (IDatabase::getInstance().submitMedicineEdit()) {
        qDebug() << "药品信息保存成功";
        emit goPreviousView();
    } else {
        QMessageBox::warning(this, "错误", "保存失败，请检查数据格式");
    }
}


void MedicineEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertMedicineEdit();

    emit goPreviousView();
}

