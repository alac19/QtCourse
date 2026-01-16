#include "prescriptionmodel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUuid>

PrescriptionModel::PrescriptionModel(QObject *parent)
    : QObject{parent}
    , m_prescriptionTabModel(nullptr)
    , m_detailModel(nullptr)
    , m_prescriptionSelection(nullptr)
{

}

bool PrescriptionModel::initPrescriptionModel()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database not open";
        return false;
    }

    // 初始化处方模型
    m_prescriptionTabModel = new QSqlTableModel(this, db);
    m_prescriptionTabModel->setTable("prescription");
    m_prescriptionTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_prescriptionTabModel->setSort(m_prescriptionTabModel->fieldIndex("PRESCRIPTIONDATE"), Qt::DescendingOrder);

    // 设置表头
    m_prescriptionTabModel->setHeaderData(m_prescriptionTabModel->fieldIndex("PRESCRIPTION_ID"), Qt::Horizontal, tr("处方ID"));
    m_prescriptionTabModel->setHeaderData(m_prescriptionTabModel->fieldIndex("PATIENT_ID"), Qt::Horizontal, tr("患者ID"));
    m_prescriptionTabModel->setHeaderData(m_prescriptionTabModel->fieldIndex("DOCTOR_ID"), Qt::Horizontal, tr("医生ID"));
    m_prescriptionTabModel->setHeaderData(m_prescriptionTabModel->fieldIndex("PRESCRIPTIONDATE"), Qt::Horizontal, tr("开具日期"));
    m_prescriptionTabModel->setHeaderData(m_prescriptionTabModel->fieldIndex("TOTALAMOUNT"), Qt::Horizontal, tr("总金额"));
    m_prescriptionTabModel->setHeaderData(m_prescriptionTabModel->fieldIndex("PAYMENTSTATUS"), Qt::Horizontal, tr("支付状态"));

    if (!m_prescriptionTabModel->select()) {
        return false;
    }

    m_prescriptionSelection = new QItemSelectionModel(m_prescriptionTabModel);

    return true;
}

int PrescriptionModel::addNewPrescription(const QString &visitRecordId)
{
    if (!m_prescriptionTabModel) return -1;

    // 插入新处方
    int row = m_prescriptionTabModel->rowCount();
    m_prescriptionTabModel->insertRow(row);

    QModelIndex index = m_prescriptionTabModel->index(row, m_prescriptionTabModel->fieldIndex("ID"));
    m_prescriptionTabModel->setData(index, QUuid::createUuid().toString(QUuid::WithoutBraces));

    index = m_prescriptionTabModel->index(row, m_prescriptionTabModel->fieldIndex("PRESCRIPTION_DATE"));
    m_prescriptionTabModel->setData(index, QDateTime::currentDateTime().toString("yyyy-MM-dd"));

    index = m_prescriptionTabModel->index(row, m_prescriptionTabModel->fieldIndex("CREATED_TIME"));
    m_prescriptionTabModel->setData(index, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    // 如果有就诊记录ID，关联患者和医生
    if (!visitRecordId.isEmpty()) {
        QSqlQuery query;
        query.prepare("SELECT PATIENT_ID, DOCTOR_ID FROM visit_record WHERE ID = ?");
        query.addBindValue(visitRecordId);

        if (query.exec() && query.next()) {
            QString patientId = query.value("PATIENT_ID").toString();
            QString doctorId = query.value("DOCTOR_ID").toString();

            index = m_prescriptionTabModel->index(row, m_prescriptionTabModel->fieldIndex("VISIT_RECORD_ID"));
            m_prescriptionTabModel->setData(index, visitRecordId);

            index = m_prescriptionTabModel->index(row, m_prescriptionTabModel->fieldIndex("PATIENT_ID"));
            m_prescriptionTabModel->setData(index, patientId);

            index = m_prescriptionTabModel->index(row, m_prescriptionTabModel->fieldIndex("DOCTOR_ID"));
            m_prescriptionTabModel->setData(index, doctorId);
        }
    }

    return row;
}

bool PrescriptionModel::deleteCurrentPrescription()
{
    if (!m_prescriptionTabModel || !m_prescriptionSelection) {
        return false;
    }

    // 获取当前选中的行
    QModelIndex currentIndex = m_prescriptionSelection->currentIndex();
    if (!currentIndex.isValid()) {
        return false;
    }

    // 获取处方ID
    QString prescriptionId = m_prescriptionTabModel->data(
                                 m_prescriptionTabModel->index(currentIndex.row(),
                                     m_prescriptionTabModel->fieldIndex("ID"))).toString();

    // 删除处方明细
    QSqlQuery detailQuery;
    detailQuery.prepare("DELETE FROM prescription_detail WHERE PRESCRIPTION_ID = ?");
    detailQuery.addBindValue(prescriptionId);
    if (!detailQuery.exec()) {
        qDebug() << "Failed to delete prescription details:" << detailQuery.lastError();
    }

    // 删除处方
    bool success = m_prescriptionTabModel->removeRow(currentIndex.row());
    if (success) {
        success = m_prescriptionTabModel->submitAll();
        m_prescriptionTabModel->select(); // 刷新数据
    }

    return success;
}

bool PrescriptionModel::submitPrescriptionEdit()
{
    if (!m_prescriptionTabModel) return false;
    return m_prescriptionTabModel->submitAll();
}

void PrescriptionModel::revertPrescriptionEdit()
{
    if (!m_prescriptionTabModel) return;
    m_prescriptionTabModel->revertAll();
}

bool PrescriptionModel::searchPrescription(const QString &filter)
{
    if (!m_prescriptionTabModel) return false;

    if (filter.isEmpty()) {
        m_prescriptionTabModel->setFilter("");
    } else {
        m_prescriptionTabModel->setFilter(filter);
    }

    return m_prescriptionTabModel->select();
}

bool PrescriptionModel::addPrescriptionDetail(const QString &prescriptionId, const QString &medicineId, int quantity, double unitPrice, const QString &dosage, const QString &usage, const QString &frequency, const QString &duration)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO prescription_detail (ID, PRESCRIPTION_ID, MEDICINE_ID, MEDICINE_NAME, "
        "QUANTITY, DOSAGE_PER_TIME, USAGE_METHOD, FREQUENCY, DURATION, UNIT_PRICE, SUBTOTAL) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );

    // 获取药品名称
    QSqlQuery medicineQuery;
    medicineQuery.prepare("SELECT NAME FROM medicine WHERE ID = ?");
    medicineQuery.addBindValue(medicineId);
    QString medicineName = "";
    if (medicineQuery.exec() && medicineQuery.next()) {
        medicineName = medicineQuery.value("NAME").toString();
    } else {
        medicineName = "未知药品";
    }

    query.addBindValue(QUuid::createUuid().toString(QUuid::WithoutBraces));
    query.addBindValue(prescriptionId);
    query.addBindValue(medicineId);
    query.addBindValue(medicineName);
    query.addBindValue(quantity);
    query.addBindValue(dosage);
    query.addBindValue(usage);
    query.addBindValue(frequency);
    query.addBindValue(duration);
    query.addBindValue(unitPrice);
    query.addBindValue(quantity * unitPrice);

    if (!query.exec()) {
        qDebug() << "Failed to add prescription detail:" << query.lastError();
        return false;
    }

    // 更新处方总金额
    double totalAmount = calculateTotalAmount(prescriptionId);
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE prescription SET TOTAL_AMOUNT = ? WHERE ID = ?");
    updateQuery.addBindValue(totalAmount);
    updateQuery.addBindValue(prescriptionId);
    if (!updateQuery.exec()) {
        qDebug() << "Failed to update prescription total amount:" << updateQuery.lastError();
    }

    return true;
}

bool PrescriptionModel::deletePrescriptionDetail(const QString &detailId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM prescription_detail WHERE ID = ?");
    query.addBindValue(detailId);

    if (!query.exec()) {
        qDebug() << "Failed to delete prescription detail:" << query.lastError();
        return false;
    }

    return true;
}

QList<QVariantMap> PrescriptionModel::getPrescriptionDetails(const QString &prescriptionId)
{
    QList<QVariantMap> details;

    QSqlQuery query;
    query.prepare(
        "SELECT pd.*, m.NAME as MEDICINE_NAME, m.SPECIFICATION "
        "FROM prescription_detail pd "
        "LEFT JOIN medicine m ON pd.MEDICINE_ID = m.ID "
        "WHERE pd.PRESCRIPTION_ID = ?"
    );
    query.addBindValue(prescriptionId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap detail;
            for (int i = 0; i < query.record().count(); ++i) {
                detail[query.record().fieldName(i)] = query.value(i);
            }
            details.append(detail);
        }
    }

    return details;
}

double PrescriptionModel::calculateTotalAmount(const QString &prescriptionId)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(SUBTOTAL) as total FROM prescription_detail WHERE PRESCRIPTION_ID = ?");
    query.addBindValue(prescriptionId);

    if (query.exec() && query.next()) {
        return query.value("total").toDouble();
    }

    return 0.0;
}

bool PrescriptionModel::updatePaymentStatus(const QString &prescriptionId, int status, const QString &method)
{
    QSqlQuery query;
    query.prepare("UPDATE prescription SET PAYMENT_STATUS = ?, PAYMENT_METHOD = ? WHERE ID = ?");
    query.addBindValue(status);
    query.addBindValue(method);
    query.addBindValue(prescriptionId);

    if (!query.exec()) {
        qDebug() << "Failed to update payment status:" << query.lastError();
        return false;
    }

    // 刷新模型
    if (m_prescriptionTabModel) {
        m_prescriptionTabModel->select();
    }

    return true;
}

QVariantMap PrescriptionModel::getPrescriptionInfo(const QString &prescriptionId)
{
    QVariantMap info;
    QSqlQuery query;

    query.prepare(
        "SELECT p.*, "
        "pt.NAME as PATIENT_NAME, pt.ID_CARD as PATIENT_ID_CARD, "
        "d.NAME as DOCTOR_NAME, d.DEPARTMENT_ID as DEPARTMENT_ID, "
        "dept.NAME as DEPARTMENT_NAME "
        "FROM prescription p "
        "LEFT JOIN patient pt ON p.PATIENT_ID = pt.ID "
        "LEFT JOIN doctor d ON p.DOCTOR_ID = d.ID "
        "LEFT JOIN department dept ON d.DEPARTMENT_ID = dept.ID "
        "WHERE p.ID = ?"
    );
    query.addBindValue(prescriptionId);

    if (query.exec() && query.next()) {
        for (int i = 0; i < query.record().count(); ++i) {
            info[query.record().fieldName(i)] = query.value(i);
        }
    }

    return info;
}

void PrescriptionModel::initDetailModel(const QString &prescriptionId)
{
    // 如果有处方ID，初始化明细模型
    if (!prescriptionId.isEmpty()) {
        QSqlDatabase db = QSqlDatabase::database();
        m_detailModel = new QSqlRelationalTableModel(this, db);
        m_detailModel->setTable("prescription_detail");
        m_detailModel->setFilter(QString("PRESCRIPTION_ID = '%1'").arg(prescriptionId));
        m_detailModel->select();
    }
}
