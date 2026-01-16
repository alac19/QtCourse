#ifndef PRESCRIPTIONMODEL_H
#define PRESCRIPTIONMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QtSql>
#include <QSqlDatabase>

class PrescriptionModel : public QObject
{
    Q_OBJECT
public:
    explicit PrescriptionModel(QObject *parent = nullptr);

    // 初始化模型
    bool initPrescriptionModel();

    // 处方操作
    int addNewPrescription(const QString &visitRecordId);
    bool deleteCurrentPrescription();
    bool submitPrescriptionEdit();
    void revertPrescriptionEdit();
    bool searchPrescription(const QString &filter);

    // 处方明细操作
    bool addPrescriptionDetail(const QString &prescriptionId, const QString &medicineId,
                               int quantity, double unitPrice, const QString &dosage = "",
                               const QString &usage = "", const QString &frequency = "",
                               const QString &duration = "");
    bool deletePrescriptionDetail(const QString &detailId);
    QList<QVariantMap> getPrescriptionDetails(const QString &prescriptionId);

    // 模型访问器
    QSqlTableModel* prescriptionModel() const
    {
        return m_prescriptionTabModel;
    }
    QSqlRelationalTableModel* detailModel() const
    {
        return m_detailModel;
    }
    QItemSelectionModel* selectionModel() const
    {
        return m_prescriptionSelection;
    }

    // 业务方法
    double calculateTotalAmount(const QString &prescriptionId);
    bool updatePaymentStatus(const QString &prescriptionId, int status, const QString &method = "");
    QVariantMap getPrescriptionInfo(const QString &prescriptionId);

private:
    void initDetailModel(const QString &prescriptionId = "");

    QSqlTableModel *m_prescriptionTabModel;
    QSqlRelationalTableModel *m_detailModel;
    QItemSelectionModel *m_prescriptionSelection;
};

#endif // PRESCRIPTIONMODEL_H
