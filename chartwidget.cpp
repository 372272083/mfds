#include "chartwidget.h"
#include "qcustomplot.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QLabel>
#include <QMenu>

#include "constants.h"

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent)
{
    xMin = 0;
    xMax = 200;
    yMin = 0;
    yMax = 5;

    myPlot = new QCustomPlot(this);
    myPlot->legend->setVisible(true);
    myPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                            QCP::iSelectLegend | QCP::iSelectPlottables);
    //myPlot->setMinimumSize(paintArea->size().width()-100,paintArea->size().height()-150);
    myPlot->setFixedWidth(this->size().width());
    myPlot->setFixedHeight(this->size().height()-200);

    myPlot->xAxis2->setVisible(false);
    myPlot->yAxis2->setVisible(false);

    // setup policy and connect slot for context menu popup:
    myPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    //xAxis
    //myPlot->axisRect()->setupFullAxesBox();
    point_size = 0;
    myPlot->replot();
}

void ChartWidget::addGraph(QString title,QPen pen)
{
    QCPGraph* graph = myPlot->addGraph();
    graph->setPen(pen);
    graph->setName(title);
    myPlot->replot();

    if(xAxise.contains(title))
    {
        xAxise[title].clear();
    }
    if(yAxise.contains(title))
    {
        yAxise[title].clear();
    }
}

void ChartWidget::deleteGraph(QString graph)
{
    int len = myPlot->graphCount();
    for(int i=0;i<len;i++)
    {
        QCPGraph* item = myPlot->graph(i);
        QString name = item->name();
        if(name.compare(graph) == 0)
        {
            myPlot->removeGraph(item);
            break;
        }
    }

    myPlot->replot();

    if(xAxise.contains(graph))
    {
        xAxise.remove(graph);
    }
    if(yAxise.contains(graph))
    {
        yAxise.remove(graph);
    }
}

void ChartWidget::deleteAllGraph()
{
    int len = myPlot->graphCount();
    for(int i=len-1;i>=0;i--)
    {
        QCPGraph* item = myPlot->graph(i);
        myPlot->removeGraph(item);
    }
}

bool ChartWidget::containsGraph(QString graph)
{
    int len = myPlot->graphCount();
    for(int i=0;i<len;i++)
    {
        QCPGraph* item = myPlot->graph(i);
        QString name = item->name();
        if(name.compare(graph) == 0)
        {
            return true;
        }
    }

    return false;
}

void ChartWidget::setTitle(QString titlestr)
{
    myPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(myPlot, titlestr, QFont("sans", 17, QFont::Bold));
    myPlot->plotLayout()->addElement(0, 0, title);
}

void ChartWidget::resizeEvent(QResizeEvent* event)
{
    myPlot->setFixedWidth(this->size().width());
    myPlot->setFixedHeight(this->size().height());
    QWidget::resizeEvent(event);
}

void ChartWidget::clearGraph(QString graph)
{
    int len = myPlot->graphCount();
    for(int i=0;i<len;i++)
    {
        QCPGraph* item = myPlot->graph(i);
        QString name = item->name();
        if(name.compare(graph) == 0)
        {
            item->data().data()->clear();
            break;
        }
    }

    myPlot->replot();

    if(xAxise.contains(graph))
    {
        xAxise[graph].clear();
    }
    if(yAxise.contains(graph))
    {
        yAxise[graph].clear();
    }
}

int ChartWidget::graphs() const
{
    return myPlot->graphCount();
}

void ChartWidget::setMaxPoint(int maxPoint)
{
    max_point = maxPoint;
}

void ChartWidget::setupXRange(double min,double max)
{
    myPlot->xAxis->setRange(min, max);
}

void ChartWidget::setupYRange(double min,double max)
{
    myPlot->yAxis->setRange(min, max);
}

void ChartWidget::addData(QString graph,int x,float y)
{
    if(xAxise.contains(graph))
    {
        xAxise[graph].push_back(x);
    }
    else
    {
        QVector<double> xValues;
        xValues.push_back(x);
        xAxise[graph] = xValues;
    }
    if(yAxise.contains(graph))
    {
        yAxise[graph].push_back(y);
    }
    else
    {
        QVector<double> yValues;
        yValues.push_back(y);
        yAxise[graph] = yValues;
    }

    point_size = xAxise[graph].size();

    if (point_size > max_point)
    {
        xAxise[graph].pop_front();
        yAxise[graph].pop_front();
    }

    int len = myPlot->graphCount();
    for(int i=0;i<len;i++)
    {
        QCPGraph* item = myPlot->graph(i);
        QString name = item->name();
        if(name.compare(graph) == 0)
        {
            item->setData(xAxise[graph],yAxise[graph]);
            break;
        }
    }

    myPlot->replot();
}

void ChartWidget::setData(QString graph,QVector<double> x,QVector<double> y)
{
    int len = myPlot->graphCount();
    for(int i=0;i<len;i++)
    {
        QCPGraph* item = myPlot->graph(i);
        QString name = item->name();
        if(name.compare(graph) == 0)
        {
            item->setData(x,y);
            break;
        }
    }

    xAxise[graph] = x;
    yAxise[graph] = y;

    myPlot->replot();
}

void ChartWidget::setAutoRange(bool xAixs,bool yAixs)
{
    xAutoRange = xAixs;
    yAutoRange = yAixs;
}

void ChartWidget::setupWindowsLabel(QString xlabel,QString ylabel)
{
    myPlot->xAxis->setLabel(xlabel);
    myPlot->yAxis->setLabel(ylabel);
}

void ChartWidget::contextMenuRequest(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (myPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {

    } else  // general context menu on graphs requested
    {
        menu->addAction(tr("reset zoom"), this, SLOT(resetZoomGraph()));
        //if (myPlot->selectedGraphs().size() > 0)
        //    menu->addAction(tr("Remove selected graph"), this, SLOT(removeSelectedGraph()));
        //if (myPlot->graphCount() > 0)
        //    menu->addAction(tr("Remove all graphs"), this, SLOT(removeAllGraphs()));
    }

    menu->popup(myPlot->mapToGlobal(pos));
}

void ChartWidget::resetZoomGraph()
{
    myPlot->rescaleAxes();
    myPlot->replot();
}
