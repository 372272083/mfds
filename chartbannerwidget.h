#ifndef CHARTBANNERWIDGET_H
#define CHARTBANNERWIDGET_H

#include <QWidget>
class QLabel;
class QResizeEvent;

class ChartBannerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartBannerWidget(QWidget *parent = nullptr);

    void setRMS(QString);
    void setRMSVisible(bool);

    void setPKPK(QString);
    void setPKPKVisible(bool);

private:
    QLabel* s_time;
    QLabel* position;
    QLabel* rms;
    QLabel* pkpk;
signals:

public slots:
};

#endif // CHARTBANNERWIDGET_H
