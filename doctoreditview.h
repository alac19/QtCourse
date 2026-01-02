#ifndef DOCTOREDITVIEW_H
#define DOCTOREDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>

namespace Ui
{
class doctorEditView;
}

class doctorEditView : public QWidget
{
    Q_OBJECT

public:
    explicit doctorEditView(QWidget *parent = nullptr, int index = 0);
    ~doctorEditView();

private slots:
    void on_btSave_clicked();

    void on_btCancel_clicked();

private:
    Ui::doctorEditView *ui;
    QDataWidgetMapper *dataMapper;   // 数据映射

signals:
    void goPreviousView();
};

#endif // DOCTOREDITVIEW_H
