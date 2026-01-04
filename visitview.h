#ifndef VISITVIEW_H
#define VISITVIEW_H

#include <QWidget>

namespace Ui
{
class VisitView;
}

class VisitView : public QWidget
{
    Q_OBJECT

public:
    explicit VisitView(QWidget *parent = nullptr);
    ~VisitView();

signals:
    void goVisitEditView(int idx);

private slots:
    void on_btSearch_clicked();

    void on_btAdd_clicked();

    void on_btDelete_clicked();

    void on_btModify_clicked();

private:
    Ui::VisitView *ui;
};

#endif // VISITVIEW_H
