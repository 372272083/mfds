#ifndef SURFACECHART_H
#define SURFACECHART_H

#include "constants.h"
#include <QMap>
#include <QVector>
#include <QDateTime>
#include "constants.h"

#include "mdisubwidget.h"

class QLabel;
class VibrateAnyseDataLib;
class ElectricAnyseDataLib;

class QCustomPlot;
class QResizeEvent;
//class SqliteDB;
class ChartBannerWidget;
class QCPBars;

class SurfaceChart : public MDISubWidget
{
    Q_OBJECT
public:
    explicit SurfaceChart(MDISubWidget *parent = 0);
    ~SurfaceChart();

    void setTitle(QString title);
    void setDeviceCode(QVector<QString> dcodes);
    void setChannelCode(QVector<QString> ccodes);
    void setChartType(TREENODETYPE chart_type);
    TREENODETYPE getChartType();

    void setLegendBase(QString legend);

    void setOfflineData(QString data,QString sample="16384",QString interval="1");

    QString title();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    QCustomPlot *myPlot;
    QWidget* chartwidget;

    QDateTime initTime;

    QLabel* titleLabel;
    TREENODETYPE chart_type;

    QString legend_b;

    QVector<QString> device_code;
    QVector<QString> channel_code;
    //QVector<QString> legends;

    QMap<QString,QString> legends;

    QCPBars *pre_bar;

    QVector<VibrateAnyseDataLib> vibtimedatas;
    QVector<ElectricAnyseDataLib> elctimedatas;

    void setupWindowsLabel(QString xlabel,QString ylabel);

    bool curState;

private slots:

    //void my_mouseMove(QMouseEvent* event);
    void resetZoomGraph();
    void contextMenuRequest(const QPoint &pos);
    void timeUpdate();
};

#endif // SURFACECHART_H
