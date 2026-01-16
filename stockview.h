#ifndef STOCKVIEW_H
#define STOCKVIEW_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>

namespace Ui
{
class StockView;
}

class StockView : public QWidget
{
    Q_OBJECT

public:
    explicit StockView(QWidget *parent = nullptr);
    ~StockView();

private slots:
    void on_btSearch_clicked();

    void on_btRefresh_clicked();

    void on_btStockIn_clicked();

    void on_btStockOut_clicked();

    void on_btAdjust_clicked();

    void on_btViewLog_clicked();

    void on_cbFilterType_currentIndexChanged(int index);

    // 表格双击事件（查看药品详情）
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::StockView *ui;
    QSqlTableModel *inventoryModel;
    QSortFilterProxyModel *proxyModel;

    void setupTableView();
    void loadInventoryData();
    void updateSummary();

    // 库存操作函数
    bool performStockOperation(const QString &medicineId, int quantity,
                               const QString &operationType, const QString &remark = "");
};

#endif // STOCKVIEW_H
