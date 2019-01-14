#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QMap>

//#include "chart.h"
//#include "chartview.h"
//#include <QtCharts/QLineSeries>
//#include <QtCharts/QSplineSeries>
//#include <QtCharts/QValueAxis>
//#include <QtCharts/QCategoryAxis>

//QT_CHARTS_USE_NAMESPACE

class QHBoxLayout;
class QVBoxLayout;
class QCustomPlot;
class QResizeEvent;

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = 0);

    void setupXRange(double min,double max);
    void setupYRange(double min,double max);

    void setMode(int mode);

    void setupWindowsLabel(QString xlabel,QString ylabel);
    void clearGraph(QString graph);
    void setMaxPoint(int maxPoint);

    void addData(QString graph,int x,float y);
    void setData(QString graph,QVector<double> x,QVector<double> y);
    void setAutoRange(bool xAixs,bool yAixs);

    void setTitle(QString title);
    void addGraph(QString title,QPen pen);
    void deleteGraph(QString graph);
    void deleteAllGraph();
    bool containsGraph(QString);

    int graphs() const;
protected:
    void resizeEvent(QResizeEvent* event);
private:
    int mode;
    int max_point;
    int point_size;
    int xAutoRange,yAutoRange;
    //QHBoxLayout* hbox;
    //QVBoxLayout* vbox;

    QCustomPlot *myPlot;
    //Chart *chart;
    //ChartView *chartView;
    float xMin,xMax;
    float yMin,yMax;

    QMap<QString,QVector<double>> xAxise;
    QMap<QString,QVector<double>> yAxise;

signals:
    void openWaveForm();
public slots:
    void contextMenuRequest(const QPoint &pos);

    void resetZoomGraph();
    void openWaveAnalyseForm();

    void my_mouseMove(QMouseEvent* event);
};

#endif // CHARTWIDGET_H
