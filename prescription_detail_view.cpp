#include "prescription_detail_view.h"
#include "ui_prescription_detail_view.h"
#include "prescription_detail_editview.h"
#include <QSqlQuery>
#include <QMessageBox>
// #include <QPrintDialog>
// #include <QPrinter>
// #include <QPainter>

Prescription_Detail_View::Prescription_Detail_View(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Prescription_Detail_View)
    , m_model(new PrescriptionModel(this))
{
    ui->setupUi(this);

    // 设置窗口标题
    this->setWindowTitle(tr("处方管理"));

    // 初始化模型
    if (!m_model->initPrescriptionModel()) {
        QMessageBox::critical(this, tr("错误"), tr("初始化处方模型失败！"));
        return;
    }

    // 设置表格
    setupTableView();

    // 设置连接
    setupConnections();
}

Prescription_Detail_View::~Prescription_Detail_View()
{
    delete ui;
}

void Prescription_Detail_View::on_btSearch_clicked()
{
    QString filter = "";
    QString keyword = ui->txtSearch->text().trimmed();

    if (!keyword.isEmpty()) {
        // 支持按患者姓名、医生姓名、处方ID搜索
        filter = QString(
                     "ID IN (SELECT p.ID FROM prescription p "
                     "LEFT JOIN patient pt ON p.PATIENT_ID = pt.ID "
                     "LEFT JOIN doctor d ON p.DOCTOR_ID = d.ID "
                     "WHERE p.ID LIKE '%%1%' OR pt.NAME LIKE '%%1%' OR d.NAME LIKE '%%1%')"
                 ).arg(keyword);
    }

    m_model->searchPrescription(filter);
}


void Prescription_Detail_View::on_btAdd_clicked()
{
    // 创建新处方
    int row = m_model->addNewPrescription("");
    if (row >= 0) {
        // 跳转到编辑界面
        emit goPrescriptionDetailView(m_model->prescriptionModel()->data(
                                          m_model->prescriptionModel()->index(row,
                                                  m_model->prescriptionModel()->fieldIndex("ID"))).toString());
    }
}


void Prescription_Detail_View::on_btDelete_clicked()
{
    QModelIndex currentIndex = m_model->selectionModel()->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择要删除的处方！"));
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("确认删除"),
                                  tr("确定要删除选中的处方吗？"),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (m_model->deleteCurrentPrescription()) {
            QMessageBox::information(this, tr("成功"), tr("处方删除成功！"));
        } else {
            QMessageBox::warning(this, tr("失败"), tr("处方删除失败！"));
        }
    }

}


void Prescription_Detail_View::on_btModify_clicked()
{
    QModelIndex currentIndex = m_model->selectionModel()->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择要修改的处方！"));
        return;
    }

    QString prescriptionId = m_model->prescriptionModel()->data(
                                 m_model->prescriptionModel()->index(currentIndex.row(),
                                     m_model->prescriptionModel()->fieldIndex("ID"))).toString();

    emit goPrescriptionDetailView(prescriptionId);
}


void Prescription_Detail_View::on_btView_clicked()
{
    QModelIndex currentIndex = m_model->selectionModel()->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择要查看的处方！"));
        return;
    }

    QString prescriptionId = m_model->prescriptionModel()->data(
                                 m_model->prescriptionModel()->index(currentIndex.row(),
                                     m_model->prescriptionModel()->fieldIndex("ID"))).toString();

    // 获取处方信息并显示
    QVariantMap info = m_model->getPrescriptionInfo(prescriptionId);
    QString message = QString("处方ID: %1\n患者: %2\n医生: %3\n总金额: %4")
                      .arg(info["ID"].toString())
                      .arg(info["PATIENT_NAME"].toString())
                      .arg(info["DOCTOR_NAME"].toString())
                      .arg(info["TOTAL_AMOUNT"].toDouble());

    QMessageBox::information(this, tr("处方详情"), message);
}


void Prescription_Detail_View::on_btPrint_clicked()
{
    QMessageBox::information(this, tr("提示"), tr("打印功能待实现"));
}


void Prescription_Detail_View::on_tableView_doubleClicked(const QModelIndex &index)
{
    // 双击查看详情
    if (index.isValid()) {
        QString prescriptionId = m_model->prescriptionModel()->data(
                                     m_model->prescriptionModel()->index(index.row(),
                                         m_model->prescriptionModel()->fieldIndex("ID"))).toString();

        emit goPrescriptionDetailView(prescriptionId);
    }
}

void Prescription_Detail_View::updateStatistics()
{
    // 暂时不实现统计功能
}

void Prescription_Detail_View::setupTableView()
{
    // 设置表格属性
    ui->tableView->setModel(m_model->prescriptionModel());
    ui->tableView->setSelectionModel(m_model->selectionModel());
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    // 调整列宽
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    // 设置列宽度
    ui->tableView->setColumnWidth(0, 150); // ID
    ui->tableView->setColumnWidth(1, 100); // 患者ID
    ui->tableView->setColumnWidth(2, 100); // 医生ID
    ui->tableView->setColumnWidth(3, 120); // 日期
    ui->tableView->setColumnWidth(4, 80);  // 总金额
    ui->tableView->setColumnWidth(5, 80);  // 支付状态
}

void Prescription_Detail_View::setupConnections()
{
    // 按钮连接已经在UI中设置，这里只添加额外连接
}

