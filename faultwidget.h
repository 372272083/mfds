#ifndef FAULTWIDGET_H
#define FAULTWIDGET_H

#include "constants.h"
#include "mdisubwidget.h"
#include <QDateTime>
#include <QMap>

class QLabel;
class QProgressBar;
class QPushButton;

class FaultWidget : public MDISubWidget
{
    Q_OBJECT
public:
    explicit FaultWidget(MDISubWidget *parent = 0);

    void setTitle(QString title);

    void setOfflineData(QString data,QString sample="16384",QString interval="1");

    QString title();

private:
    QDateTime initTime;

    QLabel* titleLabel;

    QVector<QLabel*> fault_names_v;
    QVector<QProgressBar*> fault_percents_v;
    QVector<QPushButton*> fault_details_v;

    QVector<QLabel*> fault_names_e;
    QVector<QProgressBar*> fault_percents_e;
    QVector<QPushButton*> fault_details_e;
    //QVector<QString> legends;

    QMap<int,int> fault_map;
private slots:
    void timeUpdate();
    void valueChangedSlot(int value);

    void detailClicked(bool);
};

#endif // FAULTWIDGET_H
