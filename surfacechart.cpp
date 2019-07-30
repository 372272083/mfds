#include "surfacechart.h"

#include "qcustomplot.h"

#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QtCore/qmath.h>
#include <QLabel>
#include <QFont>
#include <QTimer>

#include "globalvariable.h"

SurfaceChart::SurfaceChart(MDISubWidget *parent) : MDISubWidget(parent)
{
    chartwidget = new QWidget(this);

    myPlot = new QCustomPlot(chartwidget);
    myPlot->legend->setVisible(true);

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    //myPlot->setBackground(QBrush(gradient));

    myPlot->xAxis->setRange(0, GlobalVariable::freqlimit);
    myPlot->yAxis->setRange(0, 200);

    myPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

    // setup policy and connect slot for context menu popup:
    myPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    myPlot->replot();

    titleLabel = new QLabel(this);

    pre_bar = nullptr;

    //this->setAutoFillBackground(true);
    //this->setStyleSheet(tr("border:1px solid gray"));

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255, 255, 255));
    titleLabel->setAutoFillBackground(true);  //一定要这句，否则不行
    titleLabel->setPalette(palette);

    QFont ft = QFont("sans", 17, QFont::Bold);
    titleLabel->setFont(ft);

    QPalette pal(this->palette());

    //设置背景黑色
    pal.setColor(QPalette::Background, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    QVBoxLayout *hLayout = new QVBoxLayout(this);
    hLayout->setSpacing(0);
    hLayout->addWidget(titleLabel,0,Qt::AlignCenter);
    hLayout->addWidget(chartwidget, 1);

    /*
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    */

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer->start(BASE_INTERVAL/4);

    //fillSqrtSinProxy();
    initTime = QDateTime::currentDateTime();

    this->setLayout(hLayout);

    curState = false;
}

SurfaceChart::~SurfaceChart()
{
    //delete m_surfaceProxy;
    delete myPlot;
}

void SurfaceChart::setTitle(QString title)
{
    this->setWindowTitle(title);
    titleLabel->setText(title);
}

void SurfaceChart::setDeviceCode(QVector<QString> code)
{
    device_code.clear();
    QVector<QString>::ConstIterator cIt;
    cIt = code.constBegin();
    for(;cIt != code.constEnd();cIt++)
    {
        QString item = *cIt;
        device_code.push_back(item);
    }
}

void SurfaceChart::setChannelCode(QVector<QString> code)
{
    channel_code.clear();
    QVector<QString>::ConstIterator cIt;
    cIt = code.constBegin();
    for(;cIt != code.constEnd();cIt++)
    {
        QString item = *cIt;
        channel_code.push_back(item);
    }
}

void SurfaceChart::setupWindowsLabel(QString xlabel,QString ylabel)
{
    myPlot->xAxis->setLabel(xlabel);
    myPlot->yAxis->setLabel(ylabel);
    myPlot->replot();
}

void SurfaceChart::setChartType(TREENODETYPE chart_type)
{
    this->chart_type = chart_type;

    switch (chart_type) {
    case WATERFALL_ACC:
        this->setupWindowsLabel(tr("Hz"),tr("m/ss"));
        break;
    case WATERFALL_SPD:
        this->setupWindowsLabel(tr("Hz"),tr("mm/s"));
        break;
    case WATERFALL_VOL:
        this->setupWindowsLabel(tr("Hz"),tr("V"));
        break;
    case WATERFALL_CUR:
        this->setupWindowsLabel(tr("Hz"),tr("A"));
        break;
    default:
        break;
    }
    legends.clear();

    int itemLen = device_code.length();
    for(int i=0;i<itemLen;i++)
    {
        QString key = device_code[i] + "-" + channel_code[i];
        QString value = legend_b + "-" + GlobalVariable::findNameByCode(device_code[i]) + "-" + channel_code[i];
        legends.insert(key,value);
    }
}

TREENODETYPE SurfaceChart::getChartType()
{
    return chart_type;
}

void SurfaceChart::setLegendBase(QString legend)
{
    this->legend_b = legend;
}

void SurfaceChart::resizeEvent(QResizeEvent* event)
{
    myPlot->setFixedWidth(chartwidget->size().width());
    myPlot->setFixedHeight(chartwidget->size().height());
    QWidget::resizeEvent(event);
}

void SurfaceChart::setOfflineData(QString data,QString sample,QString interval)
{
    bool tok;
    float interval_f;
    interval_f = interval.toFloat(&tok);

    QStringList items = data.split(';');
    int items_len = items.size();

    float xmin,xmax,ymin,ymax,zmin,zmax;

    xmin = xmax = ymin = ymax = zmin = zmax = 0;

    int bsize = myPlot->plottableCount();
    for(int i=bsize-1;i>=0;i--)
    {
        myPlot->removePlottable(i);
    }
    pre_bar = nullptr;

    curState = false;

    for (int i = 0 ; i < items_len ; i++) {
        // Keep values within range bounds, since just adding step can cause minor drift due
        // to the rounding errors.
        QString item = items[i];
        QStringList item_str = item.split('#');
        if(item_str.size() == 2)
        {
            QCPBars *regen_l = new QCPBars(myPlot->xAxis, myPlot->yAxis);
            QColor re_c;
            re_c.setRgb(qrand()%256, qrand()%256, qrand()%256);
            regen_l->setPen(QPen(re_c.lighter(130)));
            regen_l->setBrush(re_c);
            regen_l->setName(item_str[1]);
            int index = 0;
            QString freqs = item_str[0];
            QStringList freqs_item = freqs.split(',');
            QVector<double> xValue,yValue;
            for (int j = 0; j < freqs_item.size(); j++) {
                QString freq_str = freqs_item[j];
                float x = j/interval_f;
                float y = freq_str.toFloat(&tok);
                if(x>GlobalVariable::freqlimit)
                {
                    break;
                }
                xValue.push_back(x);
                yValue.push_back(y);
                if(xmax<x)
                    xmax = x;
                if(xmin>x)
                    xmin = x;
                if(ymax<y)
                    ymax = y;
                if(ymin>y)
                    ymin = y;
                index++;
            }

            regen_l->setData(xValue,yValue);
            if(pre_bar != nullptr)
            {
                regen_l->moveAbove(pre_bar);
            }

            pre_bar = regen_l;
        }
    }

    myPlot->replot();
}

QString SurfaceChart::title()
{
    return this->windowTitle();
}

void SurfaceChart::contextMenuRequest(const QPoint &pos)
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

void SurfaceChart::resetZoomGraph()
{
    myPlot->rescaleAxes();
    myPlot->replot();
}

void SurfaceChart::timeUpdate()
{
    if(!GlobalVariable::isOnline)
    {
        return;
    }
    if(GlobalVariable::isOnline != curState)
    {
        curState = GlobalVariable::isOnline;
        int bsize = myPlot->plottableCount();
        for(int i=bsize-1;i>=0;i--)
        {
            myPlot->removePlottable(i);
        }
        pre_bar = nullptr;
    }
    static int index_x = 0;
    int tmp = 0;

    float xmin,xmax,ymin,ymax;

    xmin = xmax = ymin = ymax = 0;

    tmp = channel_code.size();
    for(int i=0;i<tmp;i++)
    {
        QString channel_item = channel_code[i];
        QString device_item = device_code[i];

        switch (chart_type) {
        case WATERFALL_ACC:
        case WATERFALL_SPD:
            {
                QMutexLocker m_diagnose_lock(&GlobalVariable::vibdiagnoseglobalMutex);
                if(GlobalVariable::vibrate_analyse.contains(device_item))
                {
                    if(GlobalVariable::vibrate_analyse[device_item].contains(channel_item))
                    {
                        if (GlobalVariable::vibrate_analyse[device_item][channel_item].size() > 0)
                        {
                            //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                            VibrateAnyseDataLib* info = GlobalVariable::vibrate_analyse[device_item][channel_item].back();

                            QString info_t = info->sample_time;
                            QDateTime dt_i = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                            bool flag = false;
                            if(initTime.secsTo(dt_i) >0)
                            {
                                initTime = dt_i;
                                flag = true;
                            }
                            if(!flag)
                            {
                                continue;
                            }
                            int index = 0;
                            float tmplen = 1.0;
                            std::vector<double> data_list;

                            if(chart_type == WATERFALL_ACC)
                            {
                                data_list = info->acc_freq;
                                tmplen = data_list.size();
                                tmplen /= info->sample_interval;
                            }
                            else
                            {
                                data_list = info->spd_freq;
                                tmplen = data_list.size();
                                tmplen /= info->sample_interval;
                            }

                            std::vector<double>::iterator it;
                            QCPBars *regen_l = new QCPBars(myPlot->xAxis,myPlot->yAxis);
                            QColor re_c;
                            re_c.setRgb(qrand()%256, qrand()%256, qrand()%256);
                            regen_l->setPen(QPen(re_c.lighter(130)));
                            regen_l->setBrush(re_c);
                            regen_l->setName(info_t);

                            index_x = 0;
                            QVector<double> xValue,yValue;
                            for (it = data_list.begin(); it != data_list.end();it++)
                            {
                                double i_value = *it;
                                float x_v = index*1.0;

                                float x = x_v/info->sample_interval;
                                float y = i_value;
                                xValue.push_back(x);
                                yValue.push_back(y);
                                if(xmax<x)
                                    xmax = x;
                                if(xmin>x)
                                    xmin = x;
                                if(ymax<y)
                                    ymax = y;
                                if(ymin>y)
                                    ymin = y;
                                index++;

                                if(x > GlobalVariable::freqlimit)
                                {
                                    break;
                                }
                            }

                            regen_l->setData(xValue,yValue);
                            if(pre_bar != nullptr)
                            {
                                regen_l->moveAbove(pre_bar);
                            }

                            pre_bar = regen_l;
                            if(myPlot->plottableCount()>GlobalVariable::waterfalldepthlimit)
                            {
                                myPlot->removePlottable(0);
                            }
                            myPlot->replot();
                        }
                    }
                }
            }
            break;
        case WATERFALL_VOL:
        case WATERFALL_CUR:
            {
                QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                if(GlobalVariable::electric_analyse.contains(device_item))
                {
                    if(GlobalVariable::electric_analyse[device_item].contains(channel_item))
                    {
                        if (GlobalVariable::electric_analyse[device_item][channel_item].size() > 0)
                        {
                            //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                            ElectricAnyseDataLib* info = GlobalVariable::electric_analyse[device_item][channel_item].back();

                            QString info_t = info->sample_time;
                            QDateTime dt_i = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                            bool flag = false;
                            if(initTime.secsTo(dt_i) >0)
                            {
                                initTime = dt_i;
                                //vibtimedatas.push_back(info);
                                //if(vibtimedatas.size()>GlobalVariable::waterfalldepthlimit)
                                //{
                                //    vibtimedatas.removeAt(0);
                                //}
                                flag = true;
                            }
                            if(!flag)
                            {
                                continue;
                            }
                            int index = 0;
                            float tmplen = 1.0;
                            std::vector<double> data_list;

                            if(chart_type == WATERFALL_VOL)
                            {
                                data_list = info->vol_freq;
                                tmplen = data_list.size();
                                tmplen /= info->sample_interval;
                            }
                            else
                            {
                                data_list = info->cur_freq;
                                tmplen = data_list.size();
                                tmplen /= info->sample_interval;
                            }

                            std::vector<double>::iterator it;
                            QCPBars *regen_l = new QCPBars(myPlot->xAxis,myPlot->yAxis);
                            QColor re_c;
                            re_c.setRgb(qrand()%256, qrand()%256, qrand()%256);
                            regen_l->setPen(QPen(re_c.lighter(130)));
                            regen_l->setBrush(re_c);
                            regen_l->setName(info_t);

                            index_x = 0;
                            QVector<double> xValue,yValue;
                            for (it = data_list.begin(); it != data_list.end();it++)
                            {
                                double i_value = *it;
                                float x_v = index*1.0;

                                float x = x_v/info->sample_interval;
                                float y = i_value;
                                xValue.push_back(x);
                                yValue.push_back(y);
                                if(xmax<x)
                                    xmax = x;
                                if(xmin>x)
                                    xmin = x;
                                if(ymax<y)
                                    ymax = y;
                                if(ymin>y)
                                    ymin = y;
                                index++;

                                if(x > GlobalVariable::freqlimit)
                                {
                                    break;
                                }
                            }

                            regen_l->setData(xValue,yValue);
                            if(pre_bar != nullptr)
                            {
                                regen_l->moveAbove(pre_bar);
                            }

                            pre_bar = regen_l;
                            if(myPlot->plottableCount()>GlobalVariable::waterfalldepthlimit)
                            {
                                myPlot->removePlottable(0);
                            }
                            myPlot->replot();
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
}
