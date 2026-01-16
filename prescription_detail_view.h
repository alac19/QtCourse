#ifndef PRESCRIPTION_DETAIL_VIEW_H
#define PRESCRIPTION_DETAIL_VIEW_H

#include <QWidget>
#include "prescriptionmodel.h"

namespace Ui
{
class Prescription_Detail_View;
}

class Prescription_Detail_View : public QWidget
{
    Q_OBJECT

public:
    explicit Prescription_Detail_View(QWidget *parent = nullptr);
    ~Prescription_Detail_View();

signals:
    void goPrescriptionDetailView(const QString &prescriptionId);
    void goPrescriptionCreateView(const QString &visitRecordId = "");

private slots:
    void on_btSearch_clicked();

    void on_btAdd_clicked();

    void on_btDelete_clicked();

    void on_btModify_clicked();

    void on_btView_clicked();

    void on_btPrint_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void updateStatistics();

private:
    Ui::Prescription_Detail_View *ui;
    PrescriptionModel *m_model;

    void setupTableView();
    void setupConnections();
    void loadStatistics();
};

#endif // PRESCRIPTION_DETAIL_VIEW_H
