#include "parktransformationwidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFont>
#include <QTimer>
#include <QMutexLocker>

#include "qcustomplot.h"
#include "globalvariable.h"

ParkTransformationWidget::ParkTransformationWidget(MDISubWidget *parent) : MDISubWidget(parent)
{
    titleLabel = new QLabel(this);
    titleLabel->setFixedHeight(50);
    titleLabel->setAlignment(Qt::AlignCenter);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255, 255, 255));
    titleLabel->setAutoFillBackground(true);  //一定要这句，否则不行
    titleLabel->setPalette(palette);

    this->setAutoFillBackground(true);
    this->setPalette(palette);

    QFont ft = QFont("sans", 17, QFont::Bold);
    titleLabel->setFont(ft);

    msgLabel = new QLabel(this);
    msgLabel->setFixedHeight(20);
    msgLabel->setAlignment(Qt::AlignRight);
    msgLabel->setAutoFillBackground(true);  //一定要这句，否则不行
    msgLabel->setPalette(palette);
    msgLabel->setText(tr("Sample time:  "));

    QGridLayout *containLayout = new QGridLayout(this);
    //containLayout->setAlignment(Qt::AlignTop);
    containLayout->setSpacing(5);

    containLayout->addWidget(titleLabel,0,0,1,2);
    containLayout->addWidget(msgLabel,1,0,1,2);

    QVBoxLayout *uparklayout = new QVBoxLayout(this);
    uparklayout->setSpacing(0);

    park_u_widget = new QWidget(this);
    myPlot_park_u = new QCustomPlot(park_u_widget);
    myPlot_park_u->legend->setVisible(false);
    this->setupXRange(myPlot_park_u,0,200);
    this->setupYRange(myPlot_park_u,0,5);
    this->setChartTitle(myPlot_park_u,tr("Voltage Park Transformation"));
    park_u_msg = new QLabel(this);
    park_u_msg->setFixedHeight(30);
    park_u_msg->setText(tr("Ready"));
    park_u_msg->setAutoFillBackground(true);  //一定要这句，否则不行
    park_u_msg->setPalette(palette);
    uparklayout->addWidget(park_u_widget);
    uparklayout->addWidget(park_u_msg);

    this->addGraph(myPlot_park_u,tr("Park-U"),QPen(Qt::red));
    myPlot_park_u->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myPlot_park_u, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    myPlot_park_u->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

    containLayout->addLayout(uparklayout,2,0,1,1);

    QVBoxLayout *iparklayout = new QVBoxLayout(this);
    iparklayout->setSpacing(0);

    park_i_widget = new QWidget(this);
    myPlot_park_i = new QCustomPlot(park_i_widget);
    this->setupXRange(myPlot_park_i,0,200);
    this->setupYRange(myPlot_park_i,0,5);
    this->setChartTitle(myPlot_park_i,tr("Current Park Transformation"));
    park_i_msg = new QLabel(this);
    park_i_msg->setFixedHeight(30);
    park_i_msg->setText(tr("Ready"));
    park_i_msg->setAutoFillBackground(true);  //一定要这句，否则不行
    park_i_msg->setPalette(palette);
    iparklayout->addWidget(park_i_widget);
    iparklayout->addWidget(park_i_msg);

    this->addGraph(myPlot_park_i,tr("Park-I"),QPen(Qt::red));
    myPlot_park_i->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myPlot_park_i, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    myPlot_park_i->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

    containLayout->addLayout(iparklayout,2,1,1,1);

    ///////////////////////////////////////////

    QVBoxLayout *ufparklayout = new QVBoxLayout(this);
    ufparklayout->setSpacing(0);

    park_u_f_widget = new QWidget(this);
    myPlot_park_u_freq = new QCustomPlot(park_u_f_widget);
    this->setupXRange(myPlot_park_u_freq,0,200);
    this->setupYRange(myPlot_park_u_freq,0,5);
    this->setChartTitle(myPlot_park_u_freq,tr("Voltage Park Transformation Freqency"));
    park_u_f_msg = new QLabel(this);
    park_u_f_msg->setFixedHeight(30);
    park_u_f_msg->setText(tr("Ready"));
    park_u_f_msg->setAutoFillBackground(true);  //一定要这句，否则不行
    park_u_f_msg->setPalette(palette);
    ufparklayout->addWidget(park_u_f_widget);
    ufparklayout->addWidget(park_u_f_msg);

    this->addGraph(myPlot_park_u_freq,tr("Park-U-Freqency"),QPen(Qt::red));
    myPlot_park_u_freq->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myPlot_park_u_freq, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    myPlot_park_u_freq->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

    containLayout->addLayout(ufparklayout,3,0,1,1);

    QVBoxLayout *ifparklayout = new QVBoxLayout(this);
    ifparklayout->setSpacing(0);

    park_i_f_widget = new QWidget(this);
    myPlot_park_i_freq = new QCustomPlot(park_i_f_widget);
    this->setupXRange(myPlot_park_i_freq,0,200);
    this->setupYRange(myPlot_park_i_freq,0,5);
    this->setChartTitle(myPlot_park_i_freq,tr("Current Park Transformation Freqency"));
    park_i_f_msg = new QLabel(this);
    park_i_f_msg->setFixedHeight(30);
    park_i_f_msg->setText(tr("Ready"));
    park_i_f_msg->setAutoFillBackground(true);  //一定要这句，否则不行
    park_i_f_msg->setPalette(palette);
    ifparklayout->addWidget(park_i_f_widget);
    ifparklayout->addWidget(park_i_f_msg);

    this->addGraph(myPlot_park_i_freq,tr("Park-I-Freqency"),QPen(Qt::red));
    myPlot_park_i_freq->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myPlot_park_i_freq, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    myPlot_park_i_freq->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

    containLayout->addLayout(ifparklayout,3,1,1,1);

    this->setLayout(containLayout);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer->start(BASE_INTERVAL);
}

ParkTransformationWidget::~ParkTransformationWidget()
{

}

void ParkTransformationWidget::setTitle(QString title)
{
    this->setWindowTitle(title);
    titleLabel->setText(title);
}

void ParkTransformationWidget::setOfflineData(QString data,QString sample,QString interval)
{
    bool tok;

    float info_interval = interval.toFloat(&tok);
    QStringList datas = data.split('#');

    QVector<double> x_value, y_value;
    QList<QString>::ConstIterator it;

    QString data_item = datas.at(0);
    QStringList data_items = data_item.split(',');

    for(it=data_items.constBegin();it!=data_items.constEnd();it++)
    {
        QString tmp = *it;
        double va = tmp.toFloat(&tok);
        x_value.push_back(va);
    }

    data_item = datas.at(1);
    data_items = data_item.split(',');
    for(it=data_items.constBegin();it!=data_items.constEnd();it++)
    {
        QString tmp = *it;
        double va = tmp.toFloat(&tok);
        y_value.push_back(va);
    }

    this->setData(myPlot_park_u,tr("Park-U"),x_value,y_value);

    x_value.clear();
    y_value.clear();

    data_item = datas.at(3);
    data_items = data_item.split(',');
    for(it=data_items.constBegin();it!=data_items.constEnd();it++)
    {
        QString tmp = *it;
        double va = tmp.toFloat(&tok);
        x_value.push_back(va);
    }

    data_item = datas.at(4);
    data_items = data_item.split(',');
    for(it=data_items.constBegin();it!=data_items.constEnd();it++)
    {
        QString tmp = *it;
        double va = tmp.toFloat(&tok);
        y_value.push_back(va);
    }

    this->setData(myPlot_park_i,tr("Park-I"),x_value,y_value);

    x_value.clear();
    y_value.clear();
    int index = 1;
    data_item = datas.at(2);
    data_items = data_item.split(',');
    for(it=data_items.constBegin();it!=data_items.constEnd();it++)
    {
        double x = index / info_interval;
        x_value.push_back(x);
        QString tmp = *it;
        double va = tmp.toFloat(&tok);
        y_value.push_back(va);
        if(x >= GlobalVariable::freqlimit)
            break;
        index++;
    }

    this->setData(myPlot_park_u_freq,tr("Park-U-Freqency"),x_value,y_value);

    x_value.clear();
    y_value.clear();
    index = 1;
    data_item = datas.at(5);
    data_items = data_item.split(',');
    for(it=data_items.constBegin();it!=data_items.constEnd();it++)
    {
        double x = index / info_interval;
        x_value.push_back(x);
        QString tmp = *it;
        double va = tmp.toFloat(&tok);
        y_value.push_back(va);
        if(x >= GlobalVariable::freqlimit)
            break;
        index++;
    }

    this->setData(myPlot_park_i_freq,tr("Park-I-Freqency"),x_value,y_value);
}

QString ParkTransformationWidget::title()
{
    return this->windowTitle();
}

void ParkTransformationWidget::resizeEvent(QResizeEvent* event)
{
    myPlot_park_u->setFixedWidth(park_u_widget->size().width());
    myPlot_park_u->setFixedHeight(park_u_widget->size().height());

    myPlot_park_i->setFixedWidth(park_i_widget->size().width());
    myPlot_park_i->setFixedHeight(park_i_widget->size().height());

    myPlot_park_u_freq->setFixedWidth(park_u_f_widget->size().width());
    myPlot_park_u_freq->setFixedHeight(park_u_f_widget->size().height());

    myPlot_park_i_freq->setFixedWidth(park_i_f_widget->size().width());
    myPlot_park_i_freq->setFixedHeight(park_i_f_widget->size().height());

    QWidget::resizeEvent(event);
}

void ParkTransformationWidget::addGraph(QCustomPlot *myPlot,QString title,QPen pen)
{
    //QCPGraph* graph = myPlot->addGraph();
    QCPCurve* graph = new QCPCurve(myPlot->xAxis,myPlot->yAxis);
    graph->setPen(pen);
    graph->setName(title);

    curves.insert(title,graph);
    myPlot->replot();
}

void ParkTransformationWidget::resetZoomGraph()
{
    myPlot_tmp->rescaleAxes();
    myPlot_tmp->replot();
}

void ParkTransformationWidget::contextMenuRequest(const QPoint &pos)
{
    QCustomPlot* myPlot = (QCustomPlot*)sender();
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (myPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {

    } else  // general context menu on graphs requested
    {
        myPlot_tmp = myPlot;
        menu->addAction(tr("reset zoom"), this, SLOT(resetZoomGraph()));

        //if (myPlot->selectedGraphs().size() > 0)
        //    menu->addAction(tr("Remove selected graph"), this, SLOT(removeSelectedGraph()));
        //if (myPlot->graphCount() > 0)
        //    menu->addAction(tr("Remove all graphs"), this, SLOT(removeAllGraphs()));
    }

    menu->popup(myPlot->mapToGlobal(pos));
}

void ParkTransformationWidget::setAutoRange(QCustomPlot *myPlot,bool xAixs,bool yAixs)
{

}

void ParkTransformationWidget::setupWindowsLabel(QCustomPlot *myPlot,QString xlabel,QString ylabel)
{
    myPlot->xAxis->setLabel(xlabel);
    myPlot->yAxis->setLabel(ylabel);
    myPlot->replot();
}

void ParkTransformationWidget::setChartTitle(QCustomPlot *myPlot,QString titlestr)
{
    myPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(myPlot, titlestr, QFont("sans", 13, QFont::Bold));
    myPlot->plotLayout()->addElement(0, 0, title);
}

void ParkTransformationWidget::setupXRange(QCustomPlot *myPlot,double min,double max)
{
    myPlot->xAxis->setRange(min, max);
}

void ParkTransformationWidget::setupYRange(QCustomPlot *myPlot,double min,double max)
{
    myPlot->yAxis->setRange(min, max);
}

void ParkTransformationWidget::setData(QCustomPlot *myPlot,QString graph,QVector<double> x,QVector<double> y)
{
    QList<QString> names = curves.keys();
    QList<QString>::ConstIterator it;
    for(it=names.constBegin();it!=names.constEnd();it++)
    {
        QString name = *it;
        if(name.compare(graph) == 0)
        {
            curves[name]->setData(x,y);
            break;
        }
    }

    myPlot->replot();
}

void ParkTransformationWidget::my_mouseMove(QMouseEvent* event)
{

}

void ParkTransformationWidget::timeUpdate()
{
    if(!GlobalVariable::isOnline)
    {
        return;
    }

    QMutexLocker m_lock(&GlobalVariable::elcglobalMutex);
    if(GlobalVariable::elcglobaldatas.contains(this->m_mcode))
    {
        if(GlobalVariable::elcglobaldatas[this->m_mcode].size()>0)
        {
            //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::vibrates[device_item][channel_item].size());

            ElectricGlobal* info = GlobalVariable::elcglobaldatas[this->m_mcode].head();
            QString info_t = info->sample_time;
            float info_interval = info->sample_interval;
            QString msgTime = tr("Sample time:  ") + info_t + "  ";
            msgLabel->setText(msgTime);

            QVector<double> x_value_vol, y_value_vol;
            std::vector<double>::iterator it;
            for(it=info->vol_park_x.begin();it!=info->vol_park_x.end();it++)
            {
                double va = *it;
                x_value_vol.push_back(va);
            }
            for(it=info->vol_park_y.begin();it!=info->vol_park_y.end();it++)
            {
                double va = *it;
                y_value_vol.push_back(va);
            }

            QVector<double> x_value_cur, y_value_cur;
            for(it=info->cur_park_x.begin();it!=info->cur_park_x.end();it++)
            {
                double va = *it;
                x_value_cur.push_back(va);
            }
            for(it=info->cur_park_y.begin();it!=info->cur_park_y.end();it++)
            {
                double va = *it;
                y_value_cur.push_back(va);
            }

            QVector<double> x_value_vol_freq, y_value_vol_freq;
            int index = 1;
            for(it=info->vol_park_freq.begin();it!=info->vol_park_freq.end();it++)
            {
                double x = index / info_interval;
                x_value_vol_freq.push_back(x);
                double va = *it;
                y_value_vol_freq.push_back(va);
                if(x >= GlobalVariable::freqlimit)
                    break;
                index++;
            }

            QVector<double> x_value_cur_freq, y_value_cur_freq;
            index = 1;
            for(it=info->cur_park_freq.begin();it!=info->cur_park_freq.end();it++)
            {
                double x = index / info_interval;
                x_value_cur_freq.push_back(x);
                double va = *it;
                y_value_cur_freq.push_back(va);
                if(x >= GlobalVariable::freqlimit)
                    break;
                index++;
            }

            this->setData(myPlot_park_u,tr("Park-U"),x_value_vol,y_value_vol);
            this->setData(myPlot_park_i,tr("Park-I"),x_value_cur,y_value_cur);
            this->setData(myPlot_park_u_freq,tr("Park-U-Freqency"),x_value_vol_freq,y_value_vol_freq);
            this->setData(myPlot_park_i_freq,tr("Park-I-Freqency"),x_value_cur_freq,y_value_cur_freq);
        }
    }
}
