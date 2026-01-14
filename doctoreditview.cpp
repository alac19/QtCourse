#include "doctoreditview.h"
#include "ui_doctoreditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>

doctorEditView::doctorEditView(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::doctorEditView),
    currentIndex(index)
{
    ui->setupUi(this);

    // 初始化下拉框
    initComboBoxes();

    // 设置工号输入框的限制
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(
        QRegularExpression("\\d{0,4}"), ui->dbEditEmployeeNo);
    ui->dbEditEmployeeNo->setValidator(validator);

    // 限制最大长度
    ui->dbEditEmployeeNo->setMaxLength(4);

    // 获取模型
    QSqlTableModel *tabModel = IDatabase::getInstance().doctorTabModel;

    if (index >= 0 && index < tabModel->rowCount()) {
        // 加载现有记录
        loadDoctorData(index);
    } else {
        // 新增记录，设置默认值
        // 设置默认科室
        if (ui->comboDepartment->count() > 0) {
            ui->comboDepartment->setCurrentIndex(0);
        }
    }

    // 连接科室选择信号
    connect(ui->comboDepartment, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onDepartmentChanged(int)));
}

doctorEditView::~doctorEditView()
{
    delete ui;
}


void doctorEditView::loadDoctorData(int row)
{
    QSqlTableModel *tabModel = IDatabase::getInstance().doctorTabModel;

    // 获取当前行数据
    QSqlRecord record = tabModel->record(row);

    // 设置基本字段
    ui->dbEditID->setText(record.value("DOCTOR_ID").toString());
    ui->dbEditName->setText(record.value("NAME").toString());
    // 工号字段
    QString employeeNo = record.value("EMPLOYEENO").toString();
    ui->dbEditEmployeeNo->setText(employeeNo);

    // 设置科室 - 使用字符串ID
    QString deptId = record.value("DEPARTMENT_ID").toString();

    // 设置科室下拉框
    int deptIndex = ui->comboDepartment->findData(deptId);
    if (deptIndex >= 0) {
        ui->comboDepartment->setCurrentIndex(deptIndex);
    } else {
        ui->comboDepartment->setCurrentIndex(0); // 默认选择第一个
    }
}

void doctorEditView::on_btSave_clicked()
{
    // 验证数据
    if (ui->dbEditName->text().isEmpty()) {
        QMessageBox::warning(this, "保存失败", "请输入医生姓名");
        return;
    }

    if (ui->comboDepartment->currentIndex() < 0) {
        QMessageBox::warning(this, "保存失败", "请选择科室");
        return;
    }

    // 新增：验证工号格式
    QString employeeNo = ui->dbEditEmployeeNo->text().trimmed();

    // 检查是否为空
    if (employeeNo.isEmpty()) {
        QMessageBox::warning(this, "保存失败", "工号不能为空");
        return;
    }

    QRegularExpression rx("^\\d{4}$");
    if (!rx.match(employeeNo).hasMatch()) {
        QMessageBox::warning(this, "保存失败", "工号必须为4位数字（例如：0001）");
        return;
    }

    // 获取当前模型
    QSqlTableModel *model = IDatabase::getInstance().doctorTabModel;

    // 判断是新增还是修改
    int row = currentIndex;

    // 获取选中的ID和名称
    QString departmentId = ui->comboDepartment->currentData().toString();
    QString departmentName = ui->comboDepartment->currentText();

    qDebug() << "选中的科室 - ID:" << departmentId << "名称:" << departmentName;

    // 验证科室是否存在 - 使用字符串查询
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM Department WHERE DEPARTMENT_ID = ?");
    checkQuery.addBindValue(departmentId);

    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() == 0) {
            QMessageBox::warning(this, "保存失败", "选择的科室不存在");
            return;
        }
    }

    // 手动设置所有字段到模型 - 使用字符串ID
    model->setData(model->index(row, model->fieldIndex("NAME")),
                   ui->dbEditName->text());
    model->setData(model->index(row, model->fieldIndex("EMPLOYEENO")),
                   ui->dbEditEmployeeNo->text());
    model->setData(model->index(row, model->fieldIndex("DEPARTMENT_ID")),
                   departmentId);

    // 设置科室名称字段
    model->setData(model->index(row, model->fieldIndex("DEPARTMENT_NAME")),
                   departmentName);

    // 调试：打印所有设置的值
    for (int i = 0; i < model->columnCount(); ++i) {
        QString fieldName = model->headerData(i, Qt::Horizontal).toString();
        QVariant value = model->data(model->index(row, i));
        qDebug() << fieldName << ":" << value.toString();
    }

    // 提交到数据库
    if (IDatabase::getInstance().submitDoctorEdit()) {
        qDebug() << "提交成功";
        QMessageBox::information(this, "成功", "医生信息保存成功");
        emit goPreviousView();
    } else {
        QString error = model->lastError().text();
        qDebug() << "提交失败:" << error;
        QMessageBox::warning(this, "保存失败", "保存医生信息时发生错误:\n" + error);
    }
}


void doctorEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertDoctorEdit();

    emit goPreviousView();
}


void doctorEditView::initComboBoxes()
{
    // 清空下拉框
    ui->comboDepartment->clear();

    // 加载科室数据 - ID是字符串
    QSqlQuery queryDept("SELECT DEPARTMENT_ID, NAME FROM Department ORDER BY NAME");
    while (queryDept.next()) {
        QString id = queryDept.value("DEPARTMENT_ID").toString();
        QString name = queryDept.value("NAME").toString();
        ui->comboDepartment->addItem(name, id);
    }
}


void doctorEditView::onDepartmentChanged(int index)
{
    Q_UNUSED(index);
    // 这里可以添加科室变更时的逻辑
}
