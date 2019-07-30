#ifndef FEARTURECHARTWIDGET_H
#define FEARTURECHARTWIDGET_H

#include <QDateTime>
#include <QVector>
#include <QMap>
#include "constants.h"

#include "mdisubwidget.h"

class QCustomPlot;
class QResizeEvent;
//class SqliteDB;
class ChartBannerWidget;
class QLabel;
class QCPCurve;
class QAction;

class FeartureChartWidget : public MDISubWidget
{
    Q_OBJECT
public:
    FeartureChartWidget(/*SqliteDB *db, */MDISubWidget *parent = 0);
    ~FeartureChartWidget();

    void setTitle(QString title);
    void setDeviceCode(QVector<QString> dcodes);
    void setChannelCode(QVector<QString> ccodes);
    void setChartType(TREENODETYPE chart_type);
    TREENODETYPE getChartType();

    void setLegendBase(QString legend);

    void setOfflineData(QString data,QString sample="16384",QString interval="1",float rotate=3000);

    QString title();

protected:
    void resizeEvent(QResizeEvent* event);

public:
    QVector<QString> device_code;
    QVector<QString> channel_code;
private:
    QCustomPlot *myPlot;
    QWidget* chartwidget;

    QAction* autoRangeAction;

    QLabel *msg;
    QLabel *sample_time;

    ChartBannerWidget* banner;

    QString legend_b;

    //QVector<QString> legends;

    QMap<QString,QString> legends;

    QMap<QString,QCPCurve*> curves;

    //SqliteDB* m_db;

    QDateTime initTime;

    TREENODETYPE chart_type;

    int max_point;
    int point_size;
    int xAutoRange,yAutoRange;

    float xMin,xMax;
    float yMin,yMax;

    QMap<QString,QVector<double>> xAxise;
    QMap<QString,QVector<double>> yAxise;

    bool containsGraph(QString graph);
    void addData(QString graph,int x,float y);
    void setData(QString graph,QVector<double> x,QVector<double> y);

    void addGraph(QString title,QPen pen);

    void setMaxPoint(int maxPoint);
    void setAutoRange(bool xAixs,bool yAixs);

    void setupWindowsLabel(QString xlabel,QString ylabel);

    void setupXRange(double min,double max);
    void setupYRange(double min,double max);

    void setChartTitle(QString titlestr);

private slots:
    void my_mouseMove(QMouseEvent* event);
    void contextMenuRequest(const QPoint &pos);
    void resetZoomGraph();
    void autoRangeSlot();

    void timeUpdate();
};

#endif // FEARTURECHARTWIDGET_H
