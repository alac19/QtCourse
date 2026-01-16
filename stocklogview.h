#ifndef STOCKLOGVIEW_H
#define STOCKLOGVIEW_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>

namespace Ui
{
class StockLogView;
}

class StockLogView : public QWidget
{
    Q_OBJECT

public:
    explicit StockLogView(QWidget *parent = nullptr);
    ~StockLogView();

private slots:
    void on_btSearch_clicked();

    void on_btRefresh_clicked();

    void on_btExport_clicked();

    void on_btClose_clicked();

private:
    Ui::StockLogView *ui;
    QSqlTableModel *logModel;
    QSortFilterProxyModel *proxyModel;

    void setupTableView();
    void loadLogData();
    QString getOperationTypeText(const QString &operationType);
};

#endif // STOCKLOGVIEW_H
