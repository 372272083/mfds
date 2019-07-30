#ifndef PARKTRANSFORMATIONWIDGET_H
#define PARKTRANSFORMATIONWIDGET_H

#include "constants.h"
#include <QMap>
#include <QVector>
#include <QDateTime>
#include "constants.h"

#include "mdisubwidget.h"

class QLabel;
class QCPCurve;
class QCustomPlot;
class QResizeEvent;
class QWidget;

class ParkTransformationWidget : public MDISubWidget
{
    Q_OBJECT
public:
    explicit ParkTransformationWidget(MDISubWidget *parent = 0);
    ~ParkTransformationWidget();

    void setTitle(QString title);
    void setOfflineData(QString data,QString sample="16384",QString interval="1");

    QString title();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    QCustomPlot *myPlot_tmp;

    QCustomPlot *myPlot_park_u;
    QCustomPlot *myPlot_park_i;
    QCustomPlot *myPlot_park_u_freq;
    QCustomPlot *myPlot_park_i_freq;

    QWidget* park_u_widget;
    QWidget* park_i_widget;

    QWidget* park_u_f_widget;
    QWidget* park_i_f_widget;

    QLabel* titleLabel;
    QLabel* msgLabel;

    QLabel* park_u_msg;
    QLabel* park_i_msg;
    QLabel* park_u_f_msg;
    QLabel* park_i_f_msg;

    QMap<QString,QCPCurve*> curves;

    void setChartTitle(QCustomPlot *myPlot,QString titlestr);
    void setAutoRange(QCustomPlot *myPlot,bool xAixs,bool yAixs);

    void addGraph(QCustomPlot *myPlot,QString title,QPen pen);
    void setupWindowsLabel(QCustomPlot *myPlot,QString xlabel,QString ylabel);

    void setupXRange(QCustomPlot *myPlot,double min,double max);
    void setupYRange(QCustomPlot *myPlot,double min,double max);
    void setData(QCustomPlot *myPlot,QString graph,QVector<double> x,QVector<double> y);
private slots:

    void my_mouseMove(QMouseEvent* event);
    void resetZoomGraph();
    void contextMenuRequest(const QPoint &pos);
    void timeUpdate();
};

#endif // PARKTRANSFORMATIONWIDGET_H
