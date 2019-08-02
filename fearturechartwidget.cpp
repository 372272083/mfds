#include "fearturechartwidget.h"

#include "qcustomplot.h"
//#include "sqlitedb.h"

#include <QTimer>
#include <QAction>
#include "globalvariable.h"
#include "vibrateinfo.h"
#include "chargeinfo.h"
#include "tw888info.h"
#include "freqinfo.h"
#include "waveinfo.h"
#include "temperatureinfo.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "chartbannerwidget.h"

#include "mfdslib/IO_Param.h"
#include "mfdslib/VibWaveAnsys.h"
#include "mfdslib/ElcWaveAnsys.h"

FeartureChartWidget::FeartureChartWidget(/*SqliteDB *db,*/MDISubWidget *parent) : MDISubWidget(parent)//, m_db(db)
{
    this->setMinimumSize(600,400);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(5,1,5,1);

    QHBoxLayout* msgLayout = new QHBoxLayout;

    this->setLayout(layout);

    autoRangeAction = new QAction(tr("Auto Range"), this);
    autoRangeAction->setCheckable(true);
    connect(autoRangeAction, &QAction::triggered, this, &FeartureChartWidget::autoRangeSlot);
    sample_time = new QLabel(this);
    sample_time->setFixedHeight(30);
    msgLayout->addWidget(sample_time);
    msgLayout->setSpacing(10);

    sample_time->setAutoFillBackground(true);
    sample_time->setStyleSheet(tr("border:0px solid gray"));

    chartwidget = new QWidget(this);
    layout->addWidget(chartwidget);

    msg = new QLabel(this);
    msg->setFixedHeight(30);
    msg->setText("Mouse Position: (0,0)");
    msgLayout->addWidget(msg);
    msgLayout->addStretch();

    layout->addLayout(msgLayout);

    myPlot = new QCustomPlot(chartwidget);
    connect(myPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(my_mouseMove(QMouseEvent*)));
    myPlot->legend->setVisible(false);
    myPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    //myPlot->setMinimumSize(paintArea->size().width()-100,paintArea->size().height()-150);
    myPlot->setFixedWidth(this->size().width());
    myPlot->setFixedHeight(this->size().height()-200);

    myPlot->xAxis2->setVisible(false);
    myPlot->yAxis2->setVisible(false);

    // setup policy and connect slot for context menu popup:
    myPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    myPlot->replot();

    this->setMaxPoint(200);
    //this->setupWindowsLabel(tr("time(s)"),tr("U/I(V/A)"));
    this->setupXRange(0,200);
    this->setupYRange(0,5);
    //this->setChartTitle(tr("Real-Time Data"));
    this->setAutoRange(true,true);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer->start(BASE_INTERVAL / 4);

    initTime = QDateTime::currentDateTime();

    xMin = 0;
    xMax = 200;
    yMin = 0;
    yMax = 5;
    point_size = 0;

    /*
    QPalette palette(msg->palette());
    msg->setAutoFillBackground(true);
    palette.setColor(QPalette::Background, Qt::black);
    msg->setPalette(palette);
    */

    this->setAutoFillBackground(true);
    this->setStyleSheet(tr("border:1px solid gray"));

    msg->setAutoFillBackground(true);
    msg->setStyleSheet(tr("border:0px solid gray"));

    //sample_time->setAutoFillBackground(true);
    //sample_time->setStyleSheet(tr("border:0px solid gray"));

    banner = new ChartBannerWidget(this);
    banner->setGeometry(100,40,200,80);
}

void FeartureChartWidget::setChartTitle(QString titlestr)
{
    //myPlot->plotLayout()->clear();
    myPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(myPlot, titlestr, QFont("sans", 17, QFont::Bold));
    myPlot->plotLayout()->addElement(0, 0, title);
}

void FeartureChartWidget::setupXRange(double min,double max)
{
    myPlot->xAxis->setRange(min, max);
}

void FeartureChartWidget::setupYRange(double min,double max)
{
    myPlot->yAxis->setRange(min, max);
}

void FeartureChartWidget::setMaxPoint(int maxPoint)
{
    max_point = maxPoint;
}

void FeartureChartWidget::setAutoRange(bool xAixs,bool yAixs)
{
    xAutoRange = xAixs;
    yAutoRange = yAixs;
}

void FeartureChartWidget::setupWindowsLabel(QString xlabel,QString ylabel)
{
    myPlot->xAxis->setLabel(xlabel);
    myPlot->yAxis->setLabel(ylabel);
    myPlot->replot();
}

void FeartureChartWidget::setDeviceCode(QVector<QString> code)
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

void FeartureChartWidget::setChannelCode(QVector<QString> code)
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

void FeartureChartWidget::setTitle(QString title)
{
    this->setWindowTitle(title);
    //this->setChartTitle(title);
}

QString FeartureChartWidget::title()
{
    return this->windowTitle();
}

void FeartureChartWidget::my_mouseMove(QMouseEvent* event)
{
    //获取鼠标坐标点
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();

   // 把鼠标坐标点 转换为 QCustomPlot 内部坐标值 （pixelToCoord 函数）
   // coordToPixel 函数与之相反 是把内部坐标值 转换为外部坐标点
    float x_val = myPlot->xAxis->pixelToCoord(x_pos);
    float y_val = myPlot->yAxis->pixelToCoord(y_pos);

    //banner->setPosition(tr("(%3  %4)").arg(x_val).arg(y_val));
    msg->setText(tr("Mouse Position: (%3  %4)").arg(x_val).arg(y_val));
   // 然后打印在界面上
   //ui->label->setText(tr("(%1  %2  ||  %3  %4)").arg(x_pos).arg(y_pos).arg(x_val).arg(y_val));
}

void FeartureChartWidget::contextMenuRequest(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (myPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {

    } else  // general context menu on graphs requested
    {
        menu->addAction(tr("reset zoom"), this, SLOT(resetZoomGraph()));
        menu->addAction(autoRangeAction);
        if(xAutoRange)
        {
            autoRangeAction->setChecked(true);
        }
        else
        {
            autoRangeAction->setChecked(false);
        }
        /*
        switch (chart_type) {
        case MEASURE:
        case MEASURE_SPEED:
        case MEASURE_POS:
        case UMEASURE:
        case IMEASURE:
        case ACC_FREQ_HX:
        case ACC_FREQ_1X:
        case ACC_FREQ_2X:
        case ACC_FREQ_3X:
        case ACC_FREQ_4X:
        case ACC_FREQ_5X:
        case ACC_FREQ_6X:
        case ACC_FREQ_7X:
        case ACC_FREQ_8X:
        case ACC_FREQ_9X:
        case ACC_FREQ_10X:
        case SPD_FREQ_HX:
        case SPD_FREQ_1X:
        case SPD_FREQ_2X:
        case SPD_FREQ_3X:
        case SPD_FREQ_4X:
        case SPD_FREQ_5X:
        case SPD_FREQ_6X:
        case SPD_FREQ_7X:
        case SPD_FREQ_8X:
        case SPD_FREQ_9X:
        case SPD_FREQ_10X:
        case VOL_FREQ_HX:
        case VOL_FREQ_1X:
        case VOL_FREQ_2X:
        case VOL_FREQ_3X:
        case VOL_FREQ_4X:
        case VOL_FREQ_5X:
        case VOL_FREQ_6X:
        case VOL_FREQ_7X:
        case VOL_FREQ_8X:
        case VOL_FREQ_9X:
        case VOL_FREQ_10X:
        case CUR_FREQ_HX:
        case CUR_FREQ_1X:
        case CUR_FREQ_2X:
        case CUR_FREQ_3X:
        case CUR_FREQ_4X:
        case CUR_FREQ_5X:
        case CUR_FREQ_6X:
        case CUR_FREQ_7X:
        case CUR_FREQ_8X:
        case CUR_FREQ_9X:
        case CUR_FREQ_10X:
        case ACTPOWER:
        case REACTPOWER:
        case APPPOWER:
        case COSF:
        case U_P_SEQUENCE:
        case U_N_SEQUENCE:
        case U_Z_SEQUENCE:
        case I_P_SEQUENCE:
        case I_N_SEQUENCE:
        case I_Z_SEQUENCE:
        case TEMP_MEASURE:
            menu->addAction(autoRangeAction);
            if(xAutoRange)
            {
                autoRangeAction->setChecked(true);
            }
            else
            {
                autoRangeAction->setChecked(false);
            }
            break;
        default:
            break;
        }
        */

        //if (myPlot->selectedGraphs().size() > 0)
        //    menu->addAction(tr("Remove selected graph"), this, SLOT(removeSelectedGraph()));
        //if (myPlot->graphCount() > 0)
        //    menu->addAction(tr("Remove all graphs"), this, SLOT(removeAllGraphs()));
    }

    menu->popup(myPlot->mapToGlobal(pos));
}

void FeartureChartWidget::autoRangeSlot()
{
    xAutoRange = !xAutoRange;
    yAutoRange = xAutoRange;
}

void FeartureChartWidget::resetZoomGraph()
{
    myPlot->rescaleAxes();
    myPlot->replot();
}

void FeartureChartWidget::resizeEvent(QResizeEvent* event)
{
    myPlot->setFixedWidth(chartwidget->size().width());
    myPlot->setFixedHeight(chartwidget->size().height());
    QWidget::resizeEvent(event);
}

void FeartureChartWidget::timeUpdate()
{
    if(!GlobalVariable::isOnline)
    {
        return;
    }
    static bool isRuning = false;
    static int index_x = 0;
    int tmp = 0;

    if(isRuning)
    {
        return;
    }
    isRuning = true;
    tmp = channel_code.size();

    QVector<double> x_path_value, y_path_value;
    for(int i=0;i<tmp;i++)
    {
        QString channel_item = channel_code[i];
        QString device_item = device_code[i];

        switch (chart_type) {
        case MEASURE:
        case MEASURE_SPEED:
        case MEASURE_POS:
            {
                QMutexLocker m_lock(&GlobalVariable::vibratesglobalMutex);
                if(GlobalVariable::vibrates.contains(device_item))
                {
                    if(GlobalVariable::vibrates[device_item].contains(channel_item))
                    {
                        if (GlobalVariable::vibrates[device_item][channel_item].size() > 0)
                        {
                            //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::vibrates[device_item][channel_item].size());

                            VibrateInfo *info = GlobalVariable::vibrates[device_item][channel_item].head();
                            QDateTime dt = QDateTime::fromString(info->rksj,GlobalVariable::dtFormat);
                            int x = initTime.secsTo(dt);
                            if (x > 0 && (index_x != x))
                            {
                                if(index_x < x)
                                {
                                    index_x = x;
                                    QString key = device_item + "-" + channel_item;
                                    if(this->containsGraph(legends[key]))
                                    {
                                        if(chart_type == MEASURE)
                                        {
                                            this->addData(legends[key],x,info->vibrate_e);
                                        }
                                        else if(chart_type == MEASURE_SPEED)
                                        {
                                            this->addData(legends[key],x,info->speed_e);
                                        }
                                        else if(chart_type == MEASURE_POS)
                                        {
                                             this->addData(legends[key],x,info->position_e);
                                        }

                                        sample_time->setText(info->rksj);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case WAVE:
        case FREQENCY:
        case SPEED_WAVE_A:
        case SPEED_FREQ_A:
        case EN_FREQ_A:
        case DIS_WAVE_A:
        case ACC_POWER:
        case ACC_Cepstrum:
        case SPD_POWER:
        case SPD_Cepstrum:
            {
                QMutexLocker m_diagnose_lock(&GlobalVariable::vibdiagnoseglobalMutex);
                if(GlobalVariable::vibrate_analyse.contains(device_item))
                {
                    if(GlobalVariable::vibrate_analyse[device_item].contains(channel_item))
                    {
                        if (GlobalVariable::vibrate_analyse[device_item][channel_item].size() > 0)
                        {
                            //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                            VibrateAnyseDataLib* info = GlobalVariable::vibrate_analyse[device_item][channel_item].head();

                            int index = 0;
                            float tmplen = info->sample_interval;
                            std::vector<double> data_list;
                            QString info_t = info->sample_time;
                            //qDebug() << "Data time: " << info_t;

                            QDateTime s_dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                            if(initTime.secsTo(s_dt) > 0)
                            {
                                initTime = s_dt;
                            }
                            else
                            {
                                //qDebug() << "Data Show Contiune";
                                continue;
                            }

                            sample_time->setText(info_t);
                            if(chart_type == SPEED_WAVE_A)
                            {
                                data_list = info->spd_wave;
                                banner->setRMS(tr("RMS: ") + QString::number(info->spdTimeFeat.RMSValue,10,4) + tr(" mm/s"));
                                banner->setPKPK(tr("PKPK: ") + QString::number(info->spdTimeFeat.PkPkValue,10,4) + tr(" mm/s"));
                                tmplen = data_list.size();
                                tmplen /= info->sample_interval;
                            }
                            else if(chart_type == FREQENCY)
                            {
                                data_list = info->acc_freq;
                            }
                            else if(chart_type == SPEED_FREQ_A)
                            {
                                data_list = info->spd_freq;
                            }
                            else if(chart_type == EN_FREQ_A)
                            {
                                data_list = info->env_freq;
                            }
                            else if(chart_type == ACC_POWER)
                            {
                                data_list = info->acc_power;
                            }
                            else if(chart_type == SPD_POWER)
                            {
                                data_list = info->spd_power;
                            }
                            else if(chart_type == ACC_Cepstrum)
                            {
                                data_list = info->acc_cepstrum;
                            }
                            else if(chart_type == SPD_Cepstrum)
                            {
                                data_list = info->spd_cepstrum;
                            }
                            else if(chart_type == WAVE)
                            {
                                data_list = info->acc_wave;
                                banner->setRMS(tr("RMS: ") + QString::number(info->accTimeFeat.RMSValue,10,4) + tr(" m/ss"));
                                banner->setPKPK(tr("PKPK: ") + QString::number(info->accTimeFeat.PkPkValue,10,4) + tr(" m/ss"));
                                tmplen = data_list.size();
                                tmplen /= info->sample_interval;
                            }
                            else
                            {
                                data_list = info->dis_wave;
                                //banner->setRMS(tr("RMS: ") + QString::number(info->accTimeFeat.RMSValue,10,4) + tr(" m/ss"));
                                //banner->setPKPK(tr("PKPK: ") + QString::number(info->accTimeFeat.PkPkValue,10,4) + tr(" m/ss"));
                                tmplen = data_list.size();
                                tmplen /= info->sample_interval;
                            }

                            std::vector<double>::iterator it;
                            //chartFreq->clearGraph(0);
                            QVector<double> x_value, y_value;

                            int mods = 0;
                            xMin = 0;
                            xMax = 0;
                            yMin = 0;
                            yMax = 0;
                            for (it = data_list.begin(); it != data_list.end();it++)
                            {
                                /*
                                if(chart_type == SPEED_WAVE_A || chart_type == WAVE || chart_type == DIS_WAVE_A)
                                {
                                    mods = index % 4;
                                }
                                */

                                if(0 == mods)
                                {
                                    float i_value = *it;
                                    float x_v = index/tmplen;
                                    x_value.push_back(x_v);
                                    y_value.push_back(i_value);

                                    if(qIsNaN(i_value))
                                    {
                                        qDebug() << "Data Show!";
                                    }
                                    if(x_v <= xMin)
                                        xMin = x_v;
                                    if(x_v >= xMax)
                                        xMax = x_v;
                                    if(i_value <= yMin)
                                        yMin = i_value;
                                    if(i_value >= yMax)
                                        yMax = i_value;

                                    if(chart_type == FREQENCY || chart_type == SPEED_FREQ_A || chart_type == SPD_POWER || chart_type == ACC_POWER || chart_type == ACC_Cepstrum || chart_type == SPD_Cepstrum)
                                    {
                                        if(x_v >= GlobalVariable::freqlimit)
                                        {
                                            break;
                                        }
                                    }
                                }
                                index++;
                            }

                            QString key = device_item + "-" + channel_item;
                            //qDebug() << "Data X size(): " << QString::number(x_value.size());
                            //qDebug() << "Data Y size(): " << QString::number(y_value.size());
                            if(this->containsGraph(legends[key]))
                            {
                                //qDebug() << "Data Show!";
                                this->setData(legends[key],x_value,y_value);
                            }
                        }
                    }
                }
            }
            break;
        case PATH_TRACKING:
            {
                QMutexLocker m_diagnose_lock(&GlobalVariable::vibdiagnoseglobalMutex);
                if(GlobalVariable::vibrate_analyse.contains(device_item))
                {
                    if(GlobalVariable::vibrate_analyse[device_item].contains(channel_item))
                    {
                        if (GlobalVariable::vibrate_analyse[device_item][channel_item].size() > 0)
                        {
                            //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                            VibrateAnyseDataLib* info = GlobalVariable::vibrate_analyse[device_item][channel_item].head();

                            int index = 0;
                            float tmplen = info->sample_interval;
                            std::vector<double> data_list = info->dis_wave;

                            int mods = 0;
                            std::vector<double>::iterator it;
                            for (it = data_list.begin(); it != data_list.end();it++)
                            {
                                //mods = index % 4;
                                if(0 == mods)
                                {
                                    double i_value = *it;
                                    if(i==0)
                                    {
                                        x_path_value.append(i_value);
                                    }
                                    else if(i==1)
                                    {
                                        y_path_value.append(i_value);
                                    }
                                }
                                index++;
                            }
                        }
                    }
                }
            }
            break;
        case ACC_FREQ_HX:
        case ACC_FREQ_1X:
        case ACC_FREQ_2X:
        case ACC_FREQ_3X:
        case ACC_FREQ_4X:
        case ACC_FREQ_5X:
        case ACC_FREQ_6X:
        case ACC_FREQ_7X:
        case ACC_FREQ_8X:
        case ACC_FREQ_9X:
        case ACC_FREQ_10X:
            {
                QMutexLocker m_diagnose_lock(&GlobalVariable::vibdiagnoseglobalMutex);
                if(GlobalVariable::vibrate_analyse.contains(device_item))
                {
                    if(GlobalVariable::vibrate_analyse[device_item].contains(channel_item))
                    {
                        if (GlobalVariable::vibrate_analyse[device_item][channel_item].size() > 0)
                        {
                            //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                            VibrateAnyseDataLib* info = GlobalVariable::vibrate_analyse[device_item][channel_item].head();

                            QString info_t = info->sample_time;
                            QDateTime dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                            int x = initTime.secsTo(dt);
                            if (x > 0 && (index_x != x))
                            {
                                if(index_x < x)
                                {
                                    index_x = x;
                                    QString key = device_item + "-" + channel_item;
                                    if(this->containsGraph(legends[key]))
                                    {
                                        int tmp_cur = (int)chart_type;
                                        int tmp_base = (int)ACC_FREQ_HX;
                                        int idx = tmp_cur - tmp_base;
                                        this->addData(legends[key],x,info->accFreqFeat.vibMultFrq[idx]);

                                        sample_time->setText(info_t);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case SPD_FREQ_HX:
        case SPD_FREQ_1X:
        case SPD_FREQ_2X:
        case SPD_FREQ_3X:
        case SPD_FREQ_4X:
        case SPD_FREQ_5X:
        case SPD_FREQ_6X:
        case SPD_FREQ_7X:
        case SPD_FREQ_8X:
        case SPD_FREQ_9X:
        case SPD_FREQ_10X:
            {
                QMutexLocker m_diagnose_lock(&GlobalVariable::vibdiagnoseglobalMutex);
                if(GlobalVariable::vibrate_analyse.contains(device_item))
                {
                    if(GlobalVariable::vibrate_analyse[device_item].contains(channel_item))
                    {
                        if (GlobalVariable::vibrate_analyse[device_item][channel_item].size() > 0)
                        {
                            //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                            VibrateAnyseDataLib* info = GlobalVariable::vibrate_analyse[device_item][channel_item].head();

                            QString info_t = info->sample_time;
                            QDateTime dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                            int x = initTime.secsTo(dt);
                            if (x > 0 && (index_x != x))
                            {
                                if(index_x < x)
                                {
                                    index_x = x;
                                    QString key = device_item + "-" + channel_item;
                                    if(this->containsGraph(legends[key]))
                                    {
                                        int tmp_cur = (int)chart_type;
                                        int tmp_base = (int)SPD_FREQ_HX;
                                        int idx = tmp_cur - tmp_base;
                                        this->addData(legends[key],x,info->spdFreqFeat.vibMultFrq[idx]);

                                        sample_time->setText(info_t);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case TEMP_MEASURE:
            {
                QMutexLocker m_lock(&GlobalVariable::temperaturesglobalMutex);
                if(GlobalVariable::temperatures.contains(device_item))
                {
                    if(GlobalVariable::temperatures[device_item].contains(channel_item))
                    {
                        if (GlobalVariable::temperatures[device_item][channel_item].size() > 0)
                        {
                            TemperatureInfo *info = GlobalVariable::temperatures[device_item][channel_item].head();
                            QDateTime dt = QDateTime::fromString(info->rksj,GlobalVariable::dtFormat);
                            int x = initTime.secsTo(dt);
                            if (x > 0 && (index_x != x))
                            {
                                if(index_x < x)
                                {
                                    index_x = x;
                                    QString key = device_item + "-" + channel_item;
                                    if(this->containsGraph(legends[key]))
                                    {
                                        this->addData(legends[key],x,info->temp);
                                        sample_time->setText(info->rksj);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    int etype = 0;
    if(chart_type != PATH_TRACKING)
    {
        int itemLen = device_code.length();
        if(itemLen>0)
        {
            DeviceInfo* di = GlobalVariable::findDeviceByCode(device_code[0]);
            if(di->deviceType.compare("E",Qt::CaseInsensitive) == 0)
            {
                if(di->deviceModel.compare("TW888",Qt::CaseInsensitive) == 0)
                {
                    etype = 1;
                }
            }
        }
    }

    if(0 == etype)
    {
        for(int i=0;i<tmp;i++)
        {
            QString channel_item = channel_code[i];
            QString device_item = device_code[i];

            switch (chart_type) {
            case UMEASURE:
            case IMEASURE:
                {
                    QMutexLocker m_lock(&GlobalVariable::chargesglobalMutex);
                    if(GlobalVariable::charges.contains(device_item))
                    {
                        if(GlobalVariable::charges[device_item].contains(channel_item))
                        {
                            if (GlobalVariable::charges[device_item][channel_item].size() > 0)
                            {
                                qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::charges[device_item][channel_item].size());

                                ChargeInfo *info = GlobalVariable::charges[device_item][channel_item].head();
                                QDateTime dt = QDateTime::fromString(info->rksj,GlobalVariable::dtFormat);
                                int x = initTime.secsTo(dt);
                                if (x > 0 && (index_x != x))
                                {
                                    if(index_x < x)
                                    {
                                        index_x = x;
                                        QString key = device_item + "-" + channel_item;
                                        if(this->containsGraph(legends[key]))
                                        {
                                            if(chart_type == UMEASURE)
                                            {
                                                this->addData(legends[key],x,info->u);
                                            }
                                            else if(chart_type == IMEASURE)
                                            {
                                                this->addData(legends[key],x,info->i);
                                            }

                                            sample_time->setText(info->rksj);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case UWAVE:
            case IWAVE:
            case UFREQENCY:
            case IFREQENCY:
                {
                    QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                    if(GlobalVariable::electric_analyse.contains(device_item))
                    {
                        if(GlobalVariable::electric_analyse[device_item].contains(channel_item))
                        {
                            if (GlobalVariable::electric_analyse[device_item][channel_item].size() > 0 && !GlobalVariable::electric_analyse[device_item][channel_item].isEmpty())
                            {
                                //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                                ElectricAnyseDataLib* info = GlobalVariable::electric_analyse[device_item][channel_item].head();

                                QString info_t = info->sample_time;
                                QDateTime s_dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                if(initTime.secsTo(s_dt) > 0)
                                {
                                    initTime = s_dt;
                                }
                                else
                                {
                                    //isRuning = false;
                                    continue;
                                }

                                int index = 0;
                                float tmplen = info->sample_interval;
                                std::vector<double> data_list;

                                sample_time->setText(info_t);
                                if(chart_type == UWAVE)
                                {
                                    data_list = info->vol_wave;
                                    banner->setRMS(tr("RMS: ") + QString::number(info->volTimeFeat.RMSValue,10,4) + tr(" V"));
                                    banner->setPKPK(tr("PKPK: ") + QString::number(info->volTimeFeat.PkPkValue,10,4) + tr(" V"));
                                    tmplen = data_list.size();
                                    tmplen /= info->sample_interval;
                                }
                                else if(chart_type == IWAVE)
                                {
                                    data_list = info->cur_wave;
                                    banner->setRMS(tr("RMS: ") + QString::number(info->curTimeFeat.RMSValue,10,4) + tr(" A"));
                                    banner->setPKPK(tr("PKPK: ") + QString::number(info->curTimeFeat.PkPkValue,10,4) + tr(" A"));
                                    tmplen = data_list.size();
                                    tmplen /= info->sample_interval;
                                }
                                else if(chart_type == UFREQENCY)
                                {
                                    data_list = info->vol_freq;
                                }
                                else if(chart_type == IFREQENCY)
                                {
                                    data_list = info->cur_freq;
                                }

                                std::vector<double>::iterator it;
                                //chartFreq->clearGraph(0);
                                QVector<double> x_value, y_value;

                                int mods = 0;

                                xMin = 0;
                                xMax = 0;
                                yMin = 0;
                                yMax = 0;

                                for (it = data_list.begin(); it != data_list.end();it++)
                                {
                                    /*
                                    if(chart_type == UWAVE || chart_type == IWAVE)
                                    {
                                        mods = index % 4;
                                    }
                                    */
                                    if(0 == mods)
                                    {
                                        double i_value = *it;
                                        float x_v = index/tmplen;
                                        x_value.push_back(x_v);
                                        y_value.push_back(i_value);

                                        if(x_v <= xMin)
                                            xMin = x_v;
                                        if(x_v >= xMax)
                                            xMax = x_v;
                                        if(i_value <= yMin)
                                            yMin = i_value;
                                        if(i_value >= yMax)
                                            yMax = i_value;

                                        if(chart_type == UFREQENCY || chart_type == IFREQENCY)
                                        {
                                            if(x_v >= GlobalVariable::freqlimit)
                                            {
                                                break;
                                            }
                                        }
                                    }
                                    index++;
                                }

                                QString key = device_item + "-" + channel_item;
                                if(this->containsGraph(legends[key]))
                                {
                                    this->setData(legends[key],x_value,y_value);
                                }
                            }
                        }
                    }
                }
                break;
            case VOL_FREQ_HX:
            case VOL_FREQ_1X:
            case VOL_FREQ_2X:
            case VOL_FREQ_3X:
            case VOL_FREQ_4X:
            case VOL_FREQ_5X:
            case VOL_FREQ_6X:
            case VOL_FREQ_7X:
            case VOL_FREQ_8X:
            case VOL_FREQ_9X:
            case VOL_FREQ_10X:
                {
                    QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                    if(GlobalVariable::electric_analyse.contains(device_item))
                    {
                        if(GlobalVariable::electric_analyse[device_item].contains(channel_item))
                        {
                            if (GlobalVariable::electric_analyse[device_item][channel_item].size() > 0)
                            {
                                //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                                ElectricAnyseDataLib* info = GlobalVariable::electric_analyse[device_item][channel_item].head();

                                QString info_t = info->sample_time;
                                QDateTime dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                int x = initTime.secsTo(dt);
                                if (x > 0 && (index_x != x))
                                {
                                    if(index_x < x)
                                    {
                                        index_x = x;
                                        QString key = device_item + "-" + channel_item;
                                        if(this->containsGraph(legends[key]))
                                        {
                                            int tmp_cur = (int)chart_type;
                                            int tmp_base = (int)VOL_FREQ_HX;
                                            int idx = tmp_cur - tmp_base;
                                            this->addData(legends[key],x,info->volFreqFeat.elcMultFrq[idx]);

                                            sample_time->setText(info_t);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case CUR_FREQ_HX:
            case CUR_FREQ_1X:
            case CUR_FREQ_2X:
            case CUR_FREQ_3X:
            case CUR_FREQ_4X:
            case CUR_FREQ_5X:
            case CUR_FREQ_6X:
            case CUR_FREQ_7X:
            case CUR_FREQ_8X:
            case CUR_FREQ_9X:
            case CUR_FREQ_10X:
                {
                    QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                    if(GlobalVariable::electric_analyse.contains(device_item))
                    {
                        if(GlobalVariable::electric_analyse[device_item].contains(channel_item))
                        {
                            if (GlobalVariable::electric_analyse[device_item][channel_item].size() > 0)
                            {
                                //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                                ElectricAnyseDataLib *info = GlobalVariable::electric_analyse[device_item][channel_item].head();

                                QString info_t = info->sample_time;
                                QDateTime dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                int x = initTime.secsTo(dt);
                                if (x > 0 && (index_x != x))
                                {
                                    if(index_x < x)
                                    {
                                        index_x = x;
                                        QString key = device_item + "-" + channel_item;
                                        if(this->containsGraph(legends[key]))
                                        {
                                            int tmp_cur = (int)chart_type;
                                            int tmp_base = (int)CUR_FREQ_HX;
                                            int idx = tmp_cur - tmp_base;
                                            this->addData(legends[key],x,info->curFreqFeat.elcMultFrq[idx]);

                                            sample_time->setText(info_t);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case ACTPOWER:
            case REACTPOWER:
            case APPPOWER:
            case COSF:
            case U_P_SEQUENCE:
            case U_N_SEQUENCE:
            case U_Z_SEQUENCE:
            case I_P_SEQUENCE:
            case I_N_SEQUENCE:
            case I_Z_SEQUENCE:
                {
                    QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                    if(GlobalVariable::electric_analyse.contains(device_item))
                    {
                        if(GlobalVariable::electric_analyse[device_item].contains(channel_item))
                        {
                            if (GlobalVariable::electric_analyse[device_item][channel_item].size() > 0)
                            {
                                //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                                ElectricAnyseDataLib *info = GlobalVariable::electric_analyse[device_item][channel_item].head();

                                QString info_t = info->sample_time;

                                QDateTime dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                int x = initTime.secsTo(dt);
                                if (x > 0 && (index_x != x))
                                {
                                    if(index_x < x)
                                    {
                                        index_x = x;
                                        QString key = device_item + "-" + channel_item;
                                        if(this->containsGraph(legends[key]))
                                        {
                                            float yvalue = 0.0;
                                            switch (chart_type) {
                                            case ACTPOWER:
                                                yvalue = info->powerAnsysFeat.actPower;
                                                break;
                                            case REACTPOWER:
                                                yvalue = info->powerAnsysFeat.reactPower;
                                                break;
                                            case APPPOWER:
                                                yvalue = info->powerAnsysFeat.appPower;
                                                break;
                                            case COSF:
                                                yvalue = info->powerAnsysFeat.cosf;
                                                break;
                                            case U_P_SEQUENCE:
                                                yvalue = info->vVolComp[0];
                                                break;
                                            case U_N_SEQUENCE:
                                                yvalue = info->vVolComp[1];
                                                break;
                                            case U_Z_SEQUENCE:
                                                yvalue = info->vVolComp[2];
                                                break;
                                            case I_P_SEQUENCE:
                                                yvalue = info->vCurComp[0];
                                                break;
                                            case I_N_SEQUENCE:
                                                yvalue = info->vCurComp[1];
                                                break;
                                            case I_Z_SEQUENCE:
                                                yvalue = info->vCurComp[2];
                                                break;
                                            default:
                                                break;
                                            }
                                            this->addData(legends[key],x,yvalue);

                                            sample_time->setText(info_t);
                                        }
                                    }
                                }
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
    else if(1 == etype)
    {
        for(int i=0;i<tmp;i++)
        {
            QString channel_item = channel_code[i];
            QString device_item = device_code[i];

            switch (chart_type) {
            case UMEASURE:
            case IMEASURE:
                {
                    QMutexLocker m_lock(&GlobalVariable::tw888chargesglobalMutex);
                    if(GlobalVariable::tw888charges.contains(device_item))
                    {
                        if(GlobalVariable::tw888charges[device_item].size() > 0)
                        {
                            TW888Info *info = GlobalVariable::tw888charges[device_item].head();
                            QDateTime dt = QDateTime::fromString(info->rksj,GlobalVariable::dtFormat);
                            int x = initTime.secsTo(dt);
                            if (x > 0 && (index_x != x))
                            {
                                if(index_x < x)
                                {
                                    index_x = x;
                                    QString key = device_item + "-A";
                                    if(this->containsGraph(legends[key]))
                                    {
                                        if(chart_type == UMEASURE)
                                        {
                                            this->addData(legends[key],x,info->vcp.u_rms[0]);
                                        }
                                        else if(chart_type == IMEASURE)
                                        {
                                            this->addData(legends[key],x,info->vcp.i_rms[0]);
                                        }
                                    }

                                    key = device_item + "-B";
                                    if(this->containsGraph(legends[key]))
                                    {
                                        if(chart_type == UMEASURE)
                                        {
                                            this->addData(legends[key],x,info->vcp.u_rms[1]);
                                        }
                                        else if(chart_type == IMEASURE)
                                        {
                                            this->addData(legends[key],x,info->vcp.i_rms[1]);
                                        }
                                    }

                                    key = device_item + "-C";
                                    if(this->containsGraph(legends[key]))
                                    {
                                        if(chart_type == UMEASURE)
                                        {
                                            this->addData(legends[key],x,info->vcp.u_rms[2]);
                                        }
                                        else if(chart_type == IMEASURE)
                                        {
                                            this->addData(legends[key],x,info->vcp.i_rms[2]);
                                        }
                                    }

                                    sample_time->setText(info->rksj);
                                }
                            }
                        }
                    }
                }
                break;
            case VOL_FREQ_HX:
            case VOL_FREQ_1X:
            case VOL_FREQ_2X:
            case VOL_FREQ_3X:
            case VOL_FREQ_4X:
            case VOL_FREQ_5X:
            case VOL_FREQ_6X:
            case VOL_FREQ_7X:
            case VOL_FREQ_8X:
            case VOL_FREQ_9X:
            case VOL_FREQ_10X:
                {
                    QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                    if(GlobalVariable::electric_analyse.contains(device_item))
                    {
                        if(GlobalVariable::electric_analyse[device_item].contains(channel_item))
                        {
                            if (GlobalVariable::electric_analyse[device_item][channel_item].size() > 0)
                            {
                                //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                                ElectricAnyseDataLib* info = GlobalVariable::electric_analyse[device_item][channel_item].head();

                                QString info_t = info->sample_time;
                                QDateTime dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                int x = initTime.secsTo(dt);
                                if (x > 0 && (index_x != x))
                                {
                                    if(index_x < x)
                                    {
                                        index_x = x;
                                        QString key = device_item + "-" + channel_item;
                                        if(this->containsGraph(legends[key]))
                                        {
                                            int tmp_cur = (int)chart_type;
                                            int tmp_base = (int)VOL_FREQ_HX;
                                            int idx = tmp_cur - tmp_base;
                                            this->addData(legends[key],x,info->volFreqFeat.elcMultFrq[idx]);

                                            sample_time->setText(info_t);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case CUR_FREQ_HX:
            case CUR_FREQ_1X:
            case CUR_FREQ_2X:
            case CUR_FREQ_3X:
            case CUR_FREQ_4X:
            case CUR_FREQ_5X:
            case CUR_FREQ_6X:
            case CUR_FREQ_7X:
            case CUR_FREQ_8X:
            case CUR_FREQ_9X:
            case CUR_FREQ_10X:
                {
                    QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                    if(GlobalVariable::electric_analyse.contains(device_item))
                    {
                        if(GlobalVariable::electric_analyse[device_item].contains(channel_item))
                        {
                            if (GlobalVariable::electric_analyse[device_item][channel_item].size() > 0)
                            {
                                //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                                ElectricAnyseDataLib *info = GlobalVariable::electric_analyse[device_item][channel_item].head();

                                QString info_t = info->sample_time;
                                QDateTime dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                int x = initTime.secsTo(dt);
                                if (x > 0 && (index_x != x))
                                {
                                    if(index_x < x)
                                    {
                                        index_x = x;
                                        QString key = device_item + "-" + channel_item;
                                        if(this->containsGraph(legends[key]))
                                        {
                                            int tmp_cur = (int)chart_type;
                                            int tmp_base = (int)CUR_FREQ_HX;
                                            int idx = tmp_cur - tmp_base;
                                            this->addData(legends[key],x,info->curFreqFeat.elcMultFrq[idx]);

                                            sample_time->setText(info_t);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case ACTPOWER:
            case REACTPOWER:
            case APPPOWER:
            case COSF:
            case U_P_SEQUENCE:
            case U_N_SEQUENCE:
            case U_Z_SEQUENCE:
            case I_P_SEQUENCE:
            case I_N_SEQUENCE:
            case I_Z_SEQUENCE:
                {
                    QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                    if(GlobalVariable::electric_analyse.contains(device_item))
                    {
                        if(GlobalVariable::electric_analyse[device_item].contains(channel_item))
                        {
                            if (GlobalVariable::electric_analyse[device_item][channel_item].size() > 0)
                            {
                                //qDebug() <<"channel " << channel_item << " data: " << QString::number(GlobalVariable::waves[device_item][channel_item].size());

                                ElectricAnyseDataLib *info = GlobalVariable::electric_analyse[device_item][channel_item].head();

                                QString info_t = info->sample_time;

                                QDateTime dt = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                int x = initTime.secsTo(dt);
                                if (x > 0 && (index_x != x))
                                {
                                    if(index_x < x)
                                    {
                                        index_x = x;
                                        QString key = device_item + "-" + channel_item;
                                        if(this->containsGraph(legends[key]))
                                        {
                                            float yvalue = 0.0;
                                            switch (chart_type) {
                                            case ACTPOWER:
                                                yvalue = info->powerAnsysFeat.actPower;
                                                break;
                                            case REACTPOWER:
                                                yvalue = info->powerAnsysFeat.reactPower;
                                                break;
                                            case APPPOWER:
                                                yvalue = info->powerAnsysFeat.appPower;
                                                break;
                                            case COSF:
                                                yvalue = info->powerAnsysFeat.cosf;
                                                break;
                                            case U_P_SEQUENCE:
                                                yvalue = info->vVolComp[0];
                                                break;
                                            case U_N_SEQUENCE:
                                                yvalue = info->vVolComp[1];
                                                break;
                                            case U_Z_SEQUENCE:
                                                yvalue = info->vVolComp[2];
                                                break;
                                            case I_P_SEQUENCE:
                                                yvalue = info->vCurComp[0];
                                                break;
                                            case I_N_SEQUENCE:
                                                yvalue = info->vCurComp[1];
                                                break;
                                            case I_Z_SEQUENCE:
                                                yvalue = info->vCurComp[2];
                                                break;
                                            default:
                                                break;
                                            }
                                            this->addData(legends[key],x,yvalue);

                                            sample_time->setText(info_t);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            default:
                break;
            }

            break;
        }
    }


    if(chart_type == PATH_TRACKING)
    {
        if(x_path_value.size()>0 && y_path_value.size()>0)
        {
            int len = x_path_value.size();
            if(len > y_path_value.size())
            {
                len = y_path_value.size();
            }

            QString value = legend_b + "[path tracking]";
            this->setData(value,x_path_value,y_path_value);
        }
    }
    isRuning = false;
}

void FeartureChartWidget::setChartType(TREENODETYPE chart_type)
{
    this->chart_type = chart_type;

    banner->setRMSVisible(true);
    banner->setPKPKVisible(true);
    banner->setGeometry(100,40,200,0);
    switch (chart_type) {
    case MEASURE:
        this->setupWindowsLabel(tr("time(s)"),tr("m/ss"));
        break;
    case MEASURE_SPEED:
        this->setupWindowsLabel(tr("time(s)"),tr("mm/s"));
        break;
    case MEASURE_POS:
        this->setupWindowsLabel(tr("time(s)"),tr("um"));
        break;
    case FREQENCY:
        this->setupWindowsLabel(tr("Hz"),tr("m/ss"));
        this->setupYRange(0,20);
        this->setupXRange(0,GlobalVariable::freqlimit);
        break;
    case WAVE:
        this->setupWindowsLabel(tr("time(s)"),tr("m/ss"));
        this->setupYRange(-10,10);
        this->setupXRange(0,1);
        banner->setRMSVisible(false);
        banner->setPKPKVisible(false);
        banner->setGeometry(100,40,200,80);
        break;
    case SPEED_WAVE_A:
        this->setupWindowsLabel(tr("time(s)"),tr("mm/s"));
        this->setupYRange(-10,10);
        this->setupXRange(0,1);
        banner->setRMSVisible(false);
        banner->setPKPKVisible(false);
        banner->setGeometry(100,40,200,80);
        break;
    case DIS_WAVE_A:
        this->setupWindowsLabel(tr("time(s)"),tr("um"));
        this->setupYRange(-10,10);
        this->setupXRange(0,1);
        banner->setRMSVisible(false);
        banner->setPKPKVisible(false);
        banner->setGeometry(100,40,200,80);
        break;
    case SPEED_FREQ_A:
        this->setupWindowsLabel(tr("Hz"),tr("mm/s"));
        this->setupYRange(0,20);
        this->setupXRange(0,GlobalVariable::freqlimit);
        break;
    case EN_FREQ_A:
        this->setupWindowsLabel(tr("Hz"),tr("m/ss"));
        this->setupYRange(0,20);
        this->setupXRange(0,GlobalVariable::freqlimit);
        break;
    case UMEASURE:
        this->setupWindowsLabel(tr("time(s)"),tr("V"));
        break;
    case IMEASURE:
        this->setupWindowsLabel(tr("time(s)"),tr("A"));
        break;
    case UWAVE:
        this->setupWindowsLabel(tr("time(s)"),tr("V"));
        this->setupYRange(-600,600);
        this->setupXRange(0,1);
        banner->setRMSVisible(false);
        banner->setPKPKVisible(false);
        banner->setGeometry(100,40,200,80);
        break;
    case IWAVE:
        this->setupWindowsLabel(tr("time(s)"),tr("A"));
        this->setupYRange(-600,600);
        this->setupXRange(0,1);
        banner->setRMSVisible(false);
        banner->setPKPKVisible(false);
        banner->setGeometry(100,40,200,80);
        break;
    case UFREQENCY:
        this->setupWindowsLabel(tr("Hz"),tr("V"));
        this->setupYRange(0,20);
        this->setupXRange(0,GlobalVariable::freqlimit);
        break;
    case IFREQENCY:
        this->setupWindowsLabel(tr("Hz"),tr("A"));
        this->setupYRange(0,20);
        this->setupXRange(0,GlobalVariable::freqlimit);
        break;
    case ACC_FREQ_HX:
    case ACC_FREQ_1X:
    case ACC_FREQ_2X:
    case ACC_FREQ_3X:
    case ACC_FREQ_4X:
    case ACC_FREQ_5X:
    case ACC_FREQ_6X:
    case ACC_FREQ_7X:
    case ACC_FREQ_8X:
    case ACC_FREQ_9X:
    case ACC_FREQ_10X:
        this->setupWindowsLabel(tr("time(s)"),tr("m/ss"));
        break;
    case SPD_FREQ_HX:
    case SPD_FREQ_1X:
    case SPD_FREQ_2X:
    case SPD_FREQ_3X:
    case SPD_FREQ_4X:
    case SPD_FREQ_5X:
    case SPD_FREQ_6X:
    case SPD_FREQ_7X:
    case SPD_FREQ_8X:
    case SPD_FREQ_9X:
    case SPD_FREQ_10X:
        this->setupWindowsLabel(tr("time(s)"),tr("mm/s"));
        break;
    case VOL_FREQ_HX:
    case VOL_FREQ_1X:
    case VOL_FREQ_2X:
    case VOL_FREQ_3X:
    case VOL_FREQ_4X:
    case VOL_FREQ_5X:
    case VOL_FREQ_6X:
    case VOL_FREQ_7X:
    case VOL_FREQ_8X:
    case VOL_FREQ_9X:
    case VOL_FREQ_10X:
        this->setupWindowsLabel(tr("time(s)"),tr("V"));
        break;
    case CUR_FREQ_HX:
    case CUR_FREQ_1X:
    case CUR_FREQ_2X:
    case CUR_FREQ_3X:
    case CUR_FREQ_4X:
    case CUR_FREQ_5X:
    case CUR_FREQ_6X:
    case CUR_FREQ_7X:
    case CUR_FREQ_8X:
    case CUR_FREQ_9X:
    case CUR_FREQ_10X:
        this->setupWindowsLabel(tr("time(s)"),tr("A"));
        break;
    case PATH_TRACKING:
        this->setupWindowsLabel(tr("(mm)"),tr("mm"));
        break;
    default:
        break;
    }
    legends.clear();

    if(chart_type != PATH_TRACKING)
    {
        int itemLen = device_code.length();
        if(itemLen>0)
        {
            DeviceInfo* di = GlobalVariable::findDeviceByCode(device_code[0]);
            if(di->deviceType.compare("E",Qt::CaseInsensitive) == 0)
            {
                myPlot->legend->setVisible(true);
                for(int i=0;i<3;i++)
                {
                    QString phase = "A";
                    QPen pen = QPen(Qt::red);
                    switch (i) {
                    case 0:
                        phase = "A";
                        pen = QPen(Qt::red);
                        break;
                    case 1:
                        phase = "B";
                        pen = QPen(Qt::green);
                        break;
                    case 2:
                        phase = "C";
                        pen = QPen(Qt::blue);
                        break;
                    default:
                        break;
                    }
                    QString key = device_code[0] + "-" + phase;
                    QString value = legend_b + "-" + GlobalVariable::findNameByCode(device_code[0]) + "-" + phase;
                    legends.insert(key,value);
                    this->addGraph(value,pen);

                    if(i==0)
                    {
                        QString title = this->windowTitle() + " [A, B, C]";
                        this->setChartTitle(title);
                    }
                }

                return;
            }
        }
        for(int i=0;i<itemLen;i++)
        {
            QString key = device_code[i] + "-" + channel_code[i];
            QString value = legend_b + "-" + GlobalVariable::findNameByCode(device_code[i]) + "-" + channel_code[i];
            legends.insert(key,value);
            this->addGraph(value,QPen(Qt::red));

            QString title = this->windowTitle() + " [" + value + "]";
            this->setChartTitle(title);
            break;
        }
    }
    else
    {
        if(device_code.size() > 1)
        {
            QString key = device_code[0] + "-" + channel_code[1];
            QString value = legend_b + "[path tracking]";
            legends.insert(key,value);
            QString title = this->windowTitle() + " [" + value + "]";
            this->setChartTitle(title);
            this->addGraph(value,QPen(Qt::red));
        }
    }
}

TREENODETYPE FeartureChartWidget::getChartType()
{
    return chart_type;
}

void FeartureChartWidget::setOfflineData(QString data,QString sample,QString interval,float rotate)
{
    static int index_x = 0;
    int tmp = 0;

    bool tok;
    int sample_num = 16384;
    sample_num = sample.toInt(&tok);

    float interval_f;
    interval_f = interval.toFloat(&tok);

    switch (chart_type) {
    case MEASURE:
    case MEASURE_SPEED:
    case UMEASURE:
    case IMEASURE:
    case ACC_FREQ_HX:
    case ACC_FREQ_1X:
    case ACC_FREQ_2X:
    case ACC_FREQ_3X:
    case ACC_FREQ_4X:
    case ACC_FREQ_5X:
    case ACC_FREQ_6X:
    case ACC_FREQ_7X:
    case ACC_FREQ_8X:
    case ACC_FREQ_9X:
    case ACC_FREQ_10X:
    case SPD_FREQ_HX:
    case SPD_FREQ_1X:
    case SPD_FREQ_2X:
    case SPD_FREQ_3X:
    case SPD_FREQ_4X:
    case SPD_FREQ_5X:
    case SPD_FREQ_6X:
    case SPD_FREQ_7X:
    case SPD_FREQ_8X:
    case SPD_FREQ_9X:
    case SPD_FREQ_10X:
    case VOL_FREQ_HX:
    case VOL_FREQ_1X:
    case VOL_FREQ_2X:
    case VOL_FREQ_3X:
    case VOL_FREQ_4X:
    case VOL_FREQ_5X:
    case VOL_FREQ_6X:
    case VOL_FREQ_7X:
    case VOL_FREQ_8X:
    case VOL_FREQ_9X:
    case VOL_FREQ_10X:
    case CUR_FREQ_HX:
    case CUR_FREQ_1X:
    case CUR_FREQ_2X:
    case CUR_FREQ_3X:
    case CUR_FREQ_4X:
    case CUR_FREQ_5X:
    case CUR_FREQ_6X:
    case CUR_FREQ_7X:
    case CUR_FREQ_8X:
    case CUR_FREQ_9X:
    case CUR_FREQ_10X:
    case ACTPOWER:
    case REACTPOWER:
    case APPPOWER:
    case COSF:
    case U_P_SEQUENCE:
    case U_N_SEQUENCE:
    case U_Z_SEQUENCE:
    case I_P_SEQUENCE:
    case I_N_SEQUENCE:
    case I_Z_SEQUENCE:
        {
            QString buffer = data;
            QStringList waves = buffer.split(",");
            int index = 0;
            QStringList::const_iterator constIterator;
            //chartFreq->clearGraph(0);
            QVector<double> x_value, y_value;

            for (constIterator = waves.constBegin(); constIterator != waves.constEnd();++constIterator)
            {
                QString item = *constIterator;
                bool tok;
                float hz;

                hz = item.toFloat(&tok);
                x_value.push_back(index);
                y_value.push_back(hz);

                index++;
            }

            int itemLen = device_code.length();
            for(int i=0;i<itemLen;i++)
            {
                QString key = device_code[i] + "-" + channel_code[i];

                if(this->containsGraph(legends[key]))
                {
                    this->setData(legends[key],x_value,y_value);
                }
            }
            sample_time->setText("");
        }
        break;
    case MEASURE_POS:

        break;
    case WAVE:
    case FREQENCY:
    case SPEED_WAVE_A:
    case SPEED_FREQ_A:
    case EN_FREQ_A:
    case DIS_WAVE_A:
    case ACC_POWER:
    case ACC_Cepstrum:
    case SPD_POWER:
    case SPD_Cepstrum:
        {
            QString buffer = data;
            QStringList waves = buffer.split(",");
            int index = 0;
            QStringList::const_iterator constIterator;
            //chartFreq->clearGraph(0);
            QVector<double> x_value, y_value;
            DOUBLE_VCT vAccWave[6];
            bool tok;
            int idx = 0;
            for (constIterator = waves.constBegin(); constIterator != waves.constEnd();++constIterator)
            {
                QString item = *constIterator;
                if (item.length() > 0)
                {
                    float hz;

                    hz = item.toFloat(&tok);
                    if (tok)
                    {
                        vAccWave[idx].push_back(hz);
                    }
                    else
                    {
                        vAccWave[idx].push_back(0);
                    }
                }
            }

            xMin = 0;
            xMax = 0;
            yMin = 0;
            yMax = 0;

            int sample = sample_num/interval_f;
            //SGlbParams *glbParams = new SGlbParams(sample,intervals[0]);
            CGlbParams *pGlbVib=new CGlbParams(rotate/60.0,sample,sample_num,4);

            SVibChnParams vibChnParams[6];
            SVibAnsyParms vibAnsyParms[6];

            double bearFeatParm[20];
            SRollBearParm rollBearParm;

            int mitreeid = GlobalVariable::findMotorTreeIndexByCode(m_mcode);
            MotorInfo *mi = GlobalVariable::findMotorByTreeId(mitreeid);

            if(mi != nullptr)
            {
                rollBearParm.ballDiameter = mi->roller_d;
                rollBearParm.contactAngle = mi->contact_angle;
                rollBearParm.innerDiameter = mi->rin;
                rollBearParm.outerDiameter = mi->rout;
                rollBearParm.pitchDiameter = mi->pitch;
                rollBearParm.nBall = mi->roller_n;

                if(rollBearParm.nBall == -1)
                {
                    vibAnsyParms[idx].bEnvFeat = 0;
                }
                else
                {
                    vibAnsyParms[idx].bEnvFeat = 1;
                    rollBearParm.getBearFaultFeat(bearFeatParm);
                    vibAnsyParms[idx].setBearFaultFeat(bearFeatParm);
                }
            }

            vibChnParams[idx].windowType = (EnWindowType)GlobalVariable::fft_params.fft_w;
            vibChnParams[idx].filterFreq[0] = GlobalVariable::fft_params.filter_lower;
            vibChnParams[idx].filterFreq[1] = GlobalVariable::fft_params.filter_up;
            vibChnParams[idx].filterType = (EnFilterType)GlobalVariable::fft_params.fft_filter;

            SVibAnsyDataLib *pVibAnsyDataLib=new SVibAnsyDataLib;

            CVibWaveAnsys *pVibWaveAnsys=new CVibWaveAnsys(pGlbVib,vibAnsyParms,vibChnParams,pVibAnsyDataLib);

            //波形

            if(pVibAnsyDataLib->pAccWave[idx] == NULL)
            {
                if(vAccWave[idx].size() > 0)
                    pVibWaveAnsys->getAnsyAccWaveByInitAccWave(vAccWave[idx],idx);
            }

            DOUBLE_VCT::iterator it;
            if(chart_type == WAVE)
            {
                if(pVibAnsyDataLib->pAccTimeDmnFeat[idx] == NULL)
                {
                    pVibWaveAnsys->getAccTimeDmnFeat(idx);
                    banner->setRMS(tr("RMS: ") + QString::number(pVibAnsyDataLib->pAccTimeDmnFeat[idx]->RMSValue,10,4) + tr(" m/ss"));
                    banner->setPKPK(tr("PKPK: ") + QString::number(pVibAnsyDataLib->pAccTimeDmnFeat[idx]->PkPkValue,10,4) + tr(" m/ss"));
                }

                float sample_count = pVibAnsyDataLib->pAccWave[idx]->size();
                sample_count /= interval_f;
                for (it = pVibAnsyDataLib->pAccWave[idx]->begin(); it != pVibAnsyDataLib->pAccWave[idx]->end(); it++)
                {
                    float tmp = *it;
                    float x_v = index/(sample_count);
                    x_value.push_back(x_v);
                    y_value.push_back(tmp);

                    if(x_v <= xMin)
                        xMin = x_v;
                    if(x_v >= xMax)
                        xMax = x_v;
                    if(tmp <= yMin)
                        yMin = tmp;
                    if(tmp >= yMax)
                        yMax = tmp;

                    index++;
                }
            }
            else if (chart_type == FREQENCY || chart_type == ACC_POWER)
            {
                if(pVibAnsyDataLib->pAccSpectrum[idx] == NULL)
                {
                    pVibWaveAnsys->getAccSpectrum(idx);
                }

                if(chart_type == FREQENCY)
                {
                    for (it = pVibAnsyDataLib->pAccSpectrum[idx]->begin(); it != pVibAnsyDataLib->pAccSpectrum[idx]->end(); it++)
                    {
                        float tmp = *it;
                        float x_v = index/interval_f;
                        x_value.push_back(x_v);
                        y_value.push_back(tmp);

                        if(x_v <= xMin)
                            xMin = x_v;
                        if(x_v >= xMax)
                            xMax = x_v;
                        if(tmp <= yMin)
                            yMin = tmp;
                        if(tmp >= yMax)
                            yMax = tmp;

                        index++;
                        if(x_v >= GlobalVariable::freqlimit)
                        {
                            break;
                        }
                    }
                }
                else if(chart_type == ACC_POWER)
                {
                    if(pVibAnsyDataLib->pAccPowSpectrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getAccPowerSpectrum(idx);
                    }
                    for (it = pVibAnsyDataLib->pAccPowSpectrum[idx]->begin(); it != pVibAnsyDataLib->pAccPowSpectrum[idx]->end(); it++)
                    {
                        float tmp = *it;
                        float x_v = index/interval_f;
                        x_value.push_back(x_v);
                        y_value.push_back(tmp);

                        if(x_v <= xMin)
                            xMin = x_v;
                        if(x_v >= xMax)
                            xMax = x_v;
                        if(tmp <= yMin)
                            yMin = tmp;
                        if(tmp >= yMax)
                            yMax = tmp;

                        index++;
                        if(x_v >= GlobalVariable::freqlimit)
                        {
                            break;
                        }
                    }
                }
            }
            else if(chart_type == SPEED_WAVE_A || chart_type == SPEED_FREQ_A || chart_type == DIS_WAVE_A || chart_type == SPD_POWER || chart_type == SPD_Cepstrum)
            {
                if(pVibAnsyDataLib->pSpdWave[idx] == NULL)
                {
                    pVibWaveAnsys->getAnsySpdWaveByAnsyAccWave(idx);
                }

                if(chart_type == SPEED_WAVE_A)
                {
                    if(pVibAnsyDataLib->pSpdTimeDmnFeat[idx] == NULL)
                    {
                        pVibWaveAnsys->getSpdTimeDmnFeat(idx);
                        banner->setRMS(tr("RMS: ") +QString::number(pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->RMSValue,10,4) + tr(" mm/s"));
                        banner->setPKPK(tr("PKPK: ") +QString::number(pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->PkPkValue,10,4) + tr(" mm/s"));
                    }

                    DOUBLE_VCT::iterator it;
                    float sample_count = pVibAnsyDataLib->pSpdWave[idx]->size();
                    sample_count /= interval_f;
                    for (it = pVibAnsyDataLib->pSpdWave[idx]->begin(); it != pVibAnsyDataLib->pSpdWave[idx]->end(); it++)
                    {
                        float tmp = *it;
                        float x_v = index/(sample_count);
                        x_value.push_back(x_v);
                        y_value.push_back(tmp);

                        if(x_v <= xMin)
                            xMin = x_v;
                        if(x_v >= xMax)
                            xMax = x_v;
                        if(tmp <= yMin)
                            yMin = tmp;
                        if(tmp >= yMax)
                            yMax = tmp;

                        index++;
                    }
                }
                else if(chart_type == SPD_Cepstrum)
                {
                    if(pVibAnsyDataLib->pSpdCepstrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getSpdCepstrum(idx);
                    }

                    DOUBLE_VCT::iterator it;
                    for (it = pVibAnsyDataLib->pSpdCepstrum[idx]->begin(); it != pVibAnsyDataLib->pSpdCepstrum[idx]->end(); it++)
                    {
                        float tmp = *it;
                        float x_v = index/interval_f;
                        x_value.push_back(x_v);
                        y_value.push_back(tmp);

                        if(x_v <= xMin)
                            xMin = x_v;
                        if(x_v >= xMax)
                            xMax = x_v;
                        if(tmp <= yMin)
                            yMin = tmp;
                        if(tmp >= yMax)
                            yMax = tmp;

                        index++;
                        if(x_v >= GlobalVariable::freqlimit)
                        {
                            break;
                        }
                    }
                }
                else if(chart_type == DIS_WAVE_A)
                {
                    if(pVibAnsyDataLib->pDisWave[idx] == NULL)
                    {
                        pVibWaveAnsys->getAnsyDisWaveByAnsySpdWave(idx);
                        //banner->setRMS(tr("RMS: ") +QString::number(pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->RMSValue,10,4) + tr(" mm/s"));
                        //banner->setPKPK(tr("PKPK: ") +QString::number(pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->PkPkValue,10,4) + tr(" mm/s"));
                    }

                    DOUBLE_VCT::iterator it;
                    float tmplen = pVibAnsyDataLib->pDisWave[idx]->size();
                    tmplen /= interval_f;
                    for (it = pVibAnsyDataLib->pDisWave[idx]->begin(); it != pVibAnsyDataLib->pDisWave[idx]->end(); it++)
                    {
                        float tmp = *it;
                        float x_v = index/(tmplen);

                        x_value.push_back(x_v);
                        y_value.push_back(tmp);

                        if(x_v <= xMin)
                            xMin = x_v;
                        if(x_v >= xMax)
                            xMax = x_v;
                        if(tmp <= yMin)
                            yMin = tmp;
                        if(tmp >= yMax)
                            yMax = tmp;

                        index++;
                    }
                }
                else
                {
                    if(pVibAnsyDataLib->pSpdSpectrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getSpdSpectrum(idx);
                    }

                    if(chart_type == SPEED_FREQ_A)
                    {
                        DOUBLE_VCT::iterator it;
                        for (it = pVibAnsyDataLib->pSpdSpectrum[idx]->begin(); it != pVibAnsyDataLib->pSpdSpectrum[idx]->end(); it++)
                        {
                            float tmp = *it;
                            float x_v = index/interval_f;
                            x_value.push_back(x_v);
                            y_value.push_back(tmp);

                            if(x_v <= xMin)
                                xMin = x_v;
                            if(x_v >= xMax)
                                xMax = x_v;
                            if(tmp <= yMin)
                                yMin = tmp;
                            if(tmp >= yMax)
                                yMax = tmp;

                            index++;
                            if(x_v >= GlobalVariable::freqlimit)
                            {
                                break;
                            }
                        }
                    }
                    else if(chart_type == SPD_POWER)
                    {
                        if(pVibAnsyDataLib->pSpdPowSpectrum[idx] == NULL)
                        {
                            pVibWaveAnsys->getSpdPowerSpectrum(idx);
                        }

                        DOUBLE_VCT::iterator it;
                        for (it = pVibAnsyDataLib->pSpdPowSpectrum[idx]->begin(); it != pVibAnsyDataLib->pSpdPowSpectrum[idx]->end(); it++)
                        {
                            float tmp = *it;
                            float x_v = index/interval_f;
                            x_value.push_back(x_v);
                            y_value.push_back(tmp);

                            if(x_v <= xMin)
                                xMin = x_v;
                            if(x_v >= xMax)
                                xMax = x_v;
                            if(tmp <= yMin)
                                yMin = tmp;
                            if(tmp >= yMax)
                                yMax = tmp;

                            index++;
                            if(x_v >= GlobalVariable::freqlimit)
                            {
                                break;
                            }
                        }
                    }
                }
            }
            else if(chart_type == EN_FREQ_A)
            {
                if(pVibAnsyDataLib->pEnvSpectrum[idx] == NULL)
                {
                    pVibWaveAnsys->getAccEnvelope(idx);
                }

                DOUBLE_VCT::iterator it;
                for (it = pVibAnsyDataLib->pEnvSpectrum[idx]->begin(); it != pVibAnsyDataLib->pEnvSpectrum[idx]->end(); it++)
                {
                    float tmp = *it;
                    float x_v = index/interval_f;

                    x_value.push_back(x_v);
                    y_value.push_back(tmp);

                    if(x_v <= xMin)
                        xMin = x_v;
                    if(x_v >= xMax)
                        xMax = x_v;
                    if(tmp <= yMin)
                        yMin = tmp;
                    if(tmp >= yMax)
                        yMax = tmp;

                    index++;
                }
            }
            else if(chart_type == ACC_Cepstrum)
            {
                if(pVibAnsyDataLib->pAccPowSpectrum[idx] == NULL)
                {
                    pVibWaveAnsys->getAccPowerSpectrum(idx);
                }

                DOUBLE_VCT::iterator it;
                for (it = pVibAnsyDataLib->pAccPowSpectrum[idx]->begin(); it != pVibAnsyDataLib->pAccPowSpectrum[idx]->end(); it++)
                {
                    float tmp = *it;
                    float x_v = index/interval_f;
                    x_value.push_back(x_v);
                    y_value.push_back(tmp);

                    if(x_v <= xMin)
                        xMin = x_v;
                    if(x_v >= xMax)
                        xMax = x_v;
                    if(tmp <= yMin)
                        yMin = tmp;
                    if(tmp >= yMax)
                        yMax = tmp;

                    index++;
                    if(x_v >= GlobalVariable::freqlimit)
                    {
                        break;
                    }
                }
            }

            delete pVibWaveAnsys;
            delete pVibAnsyDataLib;

            pVibWaveAnsys = NULL;
            pVibAnsyDataLib = NULL;

            delete pGlbVib;
            pGlbVib = NULL;

            int itemLen = device_code.length();
            for(int i=0;i<itemLen;i++)
            {
                QString key = device_code[i] + "-" + channel_code[i];

                if(this->containsGraph(legends[key]))
                {
                    this->setData(legends[key],x_value,y_value);
                }
            }
            sample_time->setText("");
        }
        break;
    case PATH_TRACKING:
        {
            QStringList sl = data.split('#');
            QString buffer = sl[0];
            QString buffer1 = sl[1];
            QStringList waves = buffer.split(",");
            int index = 0;
            QStringList::const_iterator constIterator;
            //chartFreq->clearGraph(0);
            QVector<double> x_value, y_value;
            DOUBLE_VCT vAccWave[6];
            bool tok;
            int idx = 0;
            for (constIterator = waves.constBegin(); constIterator != waves.constEnd();++constIterator)
            {
                QString item = *constIterator;
                if (item.length() > 0)
                {
                    float hz;

                    hz = item.toFloat(&tok);
                    if (tok)
                    {
                        vAccWave[idx].push_back(hz);
                    }
                    else
                    {
                        vAccWave[idx].push_back(0);
                    }
                }
            }

            idx = 1;
            waves = buffer1.split(",");
            for (constIterator = waves.constBegin(); constIterator != waves.constEnd();++constIterator)
            {
                QString item = *constIterator;
                if (item.length() > 0)
                {
                    float hz;

                    hz = item.toFloat(&tok);
                    if (tok)
                    {
                        vAccWave[idx].push_back(hz);
                    }
                    else
                    {
                        vAccWave[idx].push_back(0);
                    }
                }
            }

            int sample = sample_num/interval_f;
            //SGlbParams *glbParams = new SGlbParams(sample,intervals[0]);
            CGlbParams *pGlbVib=new CGlbParams(rotate/60.0,sample,sample_num,4);

            SVibChnParams vibChnParams[6];
            SVibAnsyParms vibAnsyParms[6];

            double bearFeatParm[20];
            SRollBearParm rollBearParm;

            int mitreeid = GlobalVariable::findMotorTreeIndexByCode(m_mcode);
            MotorInfo *mi = GlobalVariable::findMotorByTreeId(mitreeid);

            if(mi != nullptr)
            {
                rollBearParm.ballDiameter = mi->roller_d;
                rollBearParm.contactAngle = mi->contact_angle;
                rollBearParm.innerDiameter = mi->rin;
                rollBearParm.outerDiameter = mi->rout;
                rollBearParm.pitchDiameter = mi->pitch;
                rollBearParm.nBall = mi->roller_n;

                if(rollBearParm.nBall == -1)
                {
                    vibAnsyParms[idx].bEnvFeat = 0;
                }
                else
                {
                    vibAnsyParms[idx].bEnvFeat = 1;
                    rollBearParm.getBearFaultFeat(bearFeatParm);
                    vibAnsyParms[idx].setBearFaultFeat(bearFeatParm);
                }
            }

            for(int idx=0;idx<2;idx++)
            {
                vibChnParams[idx].windowType = (EnWindowType)GlobalVariable::fft_params.fft_w;
                vibChnParams[idx].filterFreq[0] = GlobalVariable::fft_params.filter_lower;
                vibChnParams[idx].filterFreq[1] = GlobalVariable::fft_params.filter_up;
                vibChnParams[idx].filterType = (EnFilterType)GlobalVariable::fft_params.fft_filter;
            }
            SVibAnsyDataLib *pVibAnsyDataLib=new SVibAnsyDataLib;

            CVibWaveAnsys *pVibWaveAnsys=new CVibWaveAnsys(pGlbVib,vibAnsyParms,vibChnParams,pVibAnsyDataLib);

            //波形
            DOUBLE_VCT::iterator it;
            for(int idx=0;idx<2;idx++)
            {
                if(pVibAnsyDataLib->pAccWave[idx] == NULL)
                {
                    if(vAccWave[idx].size() > 0)
                        pVibWaveAnsys->getAnsyAccWaveByInitAccWave(vAccWave[idx],idx);
                }

                if(pVibAnsyDataLib->pSpdWave[idx] == NULL)
                {
                    pVibWaveAnsys->getAnsySpdWaveByAnsyAccWave(idx);
                }

                if(pVibAnsyDataLib->pDisWave[idx] == NULL)
                {
                    pVibWaveAnsys->getAnsyDisWaveByAnsySpdWave(idx);
                }

                index = 0;
                for (it = pVibAnsyDataLib->pDisWave[idx]->begin(); it != pVibAnsyDataLib->pDisWave[idx]->end(); it++)
                {
                    float tmp = *it;
                    if(idx == 0)
                    {
                        x_value.push_back(tmp);
                    }
                    else
                    {
                        y_value.push_back(tmp);
                    }
                    index++;
                }
            }

            delete pVibWaveAnsys;
            delete pVibAnsyDataLib;

            pVibWaveAnsys = NULL;
            pVibAnsyDataLib = NULL;

            delete pGlbVib;
            pGlbVib = NULL;

            if(x_value.size()>0 && y_value.size()>0)
            {
                int len = x_value.size();
                if(len > y_value.size())
                {
                    len = y_value.size();
                }

                QString value = legend_b + "[path tracking]";
                this->setData(value,x_value,y_value);
            }
        }
        break;
    case UWAVE:
    case UFREQENCY:
        {
            QString buffer = data;
            QStringList waves = buffer.split(",");
            int index = 0;
            QStringList::const_iterator constIterator;
            //chartFreq->clearGraph(0);
            QVector<double> x_value, y_value;
            DOUBLE_VCT vElcWave[3];
            bool tok;
            int idx = 0;
            for (constIterator = waves.constBegin(); constIterator != waves.constEnd();++constIterator)
            {
                QString item = *constIterator;
                if (item.length() > 0)
                {
                    float hz;

                    hz = item.toFloat(&tok);
                    if (tok)
                    {
                        vElcWave[idx].push_back(hz);
                    }
                    else
                    {
                        vElcWave[idx].push_back(0);
                    }
                }
            }

            int sample = sample_num/interval_f;
            //SGlbParams *glbParams = new SGlbParams(sample,intervals[0]);
            CGlbParams *pGlbElc=new CGlbParams(rotate/60.0,sample,sample_num,4);

            SElcChnParams elcChnParams[6];
            SElcAnsyParms elcAnsyParms[6];

            elcChnParams[idx].windowType = (EnWindowType)GlobalVariable::fft_params.fft_w;
            elcChnParams[idx].filterFreq[0] = GlobalVariable::fft_params.filter_lower;
            elcChnParams[idx].filterFreq[1] = GlobalVariable::fft_params.filter_up;
            elcChnParams[idx].filterType = (EnFilterType)GlobalVariable::fft_params.fft_filter;

            SElcAnsyDataLib *pElcAnsyDataLib=new SElcAnsyDataLib;
            CElcWaveAnsys *pElcWaveAnsys=new CElcWaveAnsys(pGlbElc,elcAnsyParms,elcChnParams,pElcAnsyDataLib);

            if(pElcAnsyDataLib->pVolWave[idx] == NULL)
            {
                if(vElcWave[idx].size() > 0)
                    pElcWaveAnsys->getAnsyVolWaveByInitVolWave(vElcWave[idx],idx);
            }

            if(chart_type == UWAVE)
            {
                if(pElcAnsyDataLib->pVolTimeDmnFeat[idx] == NULL)
                {
                    pElcWaveAnsys->getVolTimeDmnFeat(idx);
                    banner->setRMS(tr("RMS: ") + QString::number(pElcAnsyDataLib->pVolTimeDmnFeat[idx]->RMSValue,10,4) + tr(" V"));
                    banner->setPKPK(tr("PKPK: ") + QString::number(pElcAnsyDataLib->pVolTimeDmnFeat[idx]->PkPkValue,10,4) + tr(" V"));
                }
                if(pElcAnsyDataLib->pVolWave[idx] != NULL)
                {
                    DOUBLE_VCT::iterator it;
                    for (it = pElcAnsyDataLib->pVolWave[idx]->begin(); it != pElcAnsyDataLib->pVolWave[idx]->end(); it++)
                    {
                        float tmp = *it;
                        x_value.push_back(index/((float)sample));
                        y_value.push_back(tmp);
                        index++;
                    }

                    int itemLen = device_code.length();
                    for(int i=0;i<itemLen;i++)
                    {
                        QString key = device_code[i] + "-" + channel_code[i];

                        if(this->containsGraph(legends[key]))
                        {
                            this->setData(legends[key],x_value,y_value);
                        }
                    }
                    sample_time->setText("");
                }
            }
            else
            {
                if(pElcAnsyDataLib->pVolSpectrum[idx] == NULL)
                {
                    pElcWaveAnsys->getVolSpectrum(idx);
                }


                if(pElcAnsyDataLib->pVolSpectrum[idx] != NULL)
                {
                    DOUBLE_VCT::iterator it;
                    for (it = pElcAnsyDataLib->pVolSpectrum[idx]->begin(); it != pElcAnsyDataLib->pVolSpectrum[idx]->end(); it++)
                    {
                        float tmp = *it;
                        float x_v = index/interval_f;
                        x_value.push_back(x_v);
                        y_value.push_back(tmp);
                        index++;

                        if(x_v >= GlobalVariable::freqlimit)
                        {
                            break;
                        }
                    }

                    int itemLen = device_code.length();
                    for(int i=0;i<itemLen;i++)
                    {
                        QString key = device_code[i] + "-" + channel_code[i];

                        if(this->containsGraph(legends[key]))
                        {
                            this->setData(legends[key],x_value,y_value);
                        }
                    }
                    sample_time->setText("");
                }
            }


            delete pElcWaveAnsys;
            delete pElcAnsyDataLib;

            pElcWaveAnsys = NULL;
            pElcAnsyDataLib = NULL;

            delete pGlbElc;
            pGlbElc = NULL;
        }
        break;
    case IWAVE:
    case IFREQENCY:
        {
            QString buffer = data;
            QStringList waves = buffer.split(",");
            int index = 0;
            QStringList::const_iterator constIterator;
            //chartFreq->clearGraph(0);
            QVector<double> x_value, y_value;
            DOUBLE_VCT vElcWave[3];
            bool tok;
            int idx = 0;
            for (constIterator = waves.constBegin(); constIterator != waves.constEnd();++constIterator)
            {
                QString item = *constIterator;
                if (item.length() > 0)
                {
                    float hz;

                    hz = item.toFloat(&tok);
                    if (tok)
                    {
                        vElcWave[idx].push_back(hz);
                    }
                    else
                    {
                        vElcWave[idx].push_back(0);
                    }
                }
            }

            int sample = sample_num/interval_f;
            //SGlbParams *glbParams = new SGlbParams(sample,intervals[0]);
            CGlbParams *pGlbElc=new CGlbParams(rotate/60.0,sample,sample_num,4);

            SElcChnParams elcChnParams[6];
            SElcAnsyParms elcAnsyParms[6];

            elcChnParams[idx].windowType = (EnWindowType)GlobalVariable::fft_params.fft_w;
            elcChnParams[idx].filterFreq[0] = GlobalVariable::fft_params.filter_lower;
            elcChnParams[idx].filterFreq[1] = GlobalVariable::fft_params.filter_up;
            elcChnParams[idx].filterType = (EnFilterType)GlobalVariable::fft_params.fft_filter;

            SElcAnsyDataLib *pElcAnsyDataLib=new SElcAnsyDataLib;
            CElcWaveAnsys *pElcWaveAnsys=new CElcWaveAnsys(pGlbElc,elcAnsyParms,elcChnParams,pElcAnsyDataLib);

            if(pElcAnsyDataLib->pCurWave[idx] == NULL)
            {
                if(vElcWave[idx].size() > 0)
                    pElcWaveAnsys->getAnsyCurWaveByInitCurWave(vElcWave[idx],idx);
            }

            if(chart_type == IWAVE)
            {
                if(pElcAnsyDataLib->pCurTimeDmnFeat[idx] == NULL)
                {
                    pElcWaveAnsys->getCurTimeDmnFeat(idx);
                    banner->setRMS(tr("RMS: ") + QString::number(pElcAnsyDataLib->pCurTimeDmnFeat[idx]->RMSValue,10,4) + tr(" A"));
                    banner->setPKPK(tr("PKPK: ") + QString::number(pElcAnsyDataLib->pCurTimeDmnFeat[idx]->PkPkValue,10,4) + tr(" A"));
                }
                if(pElcAnsyDataLib->pCurWave[idx] != NULL)
                {
                    DOUBLE_VCT::iterator it;
                    for (it = pElcAnsyDataLib->pCurWave[idx]->begin(); it != pElcAnsyDataLib->pCurWave[idx]->end(); it++)
                    {
                        float tmp = *it;
                        x_value.push_back(index/((float)sample));
                        y_value.push_back(tmp);
                        index++;
                    }

                    int itemLen = device_code.length();
                    for(int i=0;i<itemLen;i++)
                    {
                        QString key = device_code[i] + "-" + channel_code[i];

                        if(this->containsGraph(legends[key]))
                        {
                            this->setData(legends[key],x_value,y_value);
                        }
                    }
                    sample_time->setText("");
                }
            }
            else
            {
                if(pElcAnsyDataLib->pCurSpectrum[idx] == NULL)
                {
                    pElcWaveAnsys->getCurSpectrum(idx);
                }

                if(pElcAnsyDataLib->pCurSpectrum[idx] != NULL)
                {
                    DOUBLE_VCT::iterator it;
                    for (it = pElcAnsyDataLib->pCurSpectrum[idx]->begin(); it != pElcAnsyDataLib->pCurSpectrum[idx]->end(); it++)
                    {
                        float tmp = *it;
                        float x_v = index/interval_f;
                        x_value.push_back(x_v);
                        y_value.push_back(tmp);
                        index++;
                        if(x_v >= GlobalVariable::freqlimit)
                        {
                            break;
                        }
                    }

                    int itemLen = device_code.length();
                    for(int i=0;i<itemLen;i++)
                    {
                        QString key = device_code[i] + "-" + channel_code[i];

                        if(this->containsGraph(legends[key]))
                        {
                            this->setData(legends[key],x_value,y_value);
                        }
                    }
                    sample_time->setText("");
                }
            }

            delete pElcWaveAnsys;
            delete pElcAnsyDataLib;

            pElcWaveAnsys = NULL;
            pElcAnsyDataLib = NULL;

            delete pGlbElc;
            pGlbElc = NULL;
        }
        break;
    default:
        break;
    }
}

bool FeartureChartWidget::containsGraph(QString graph)
{
    /*
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
    */

    QList<QString> names = curves.keys();
    QList<QString>::ConstIterator it;
    for(it=names.constBegin();it!=names.constEnd();it++)
    {
        QString name = *it;
        if(name.compare(graph) == 0)
        {
            return true;
        }
    }

    return false;
}

void FeartureChartWidget::setData(QString graph,QVector<double> x,QVector<double> y)
{
    /*
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
    */

    QList<QString> names = curves.keys();
    QList<QString>::ConstIterator it;
    for(it=names.constBegin();it!=names.constEnd();it++)
    {
        QString name = *it;
        if(name.compare(graph) == 0)
        {
            //qDebug() << "Data Set Show";
            curves[name]->setData(x,y);
            break;
        }
    }

    xAxise[graph] = x;
    yAxise[graph] = y;

    if(xAutoRange)
    {
        float xoffset = 0;
        float yoffset = 0;

        switch (chart_type) {
        case WAVE:
        case SPEED_WAVE_A:
        case DIS_WAVE_A:
        case UWAVE:
        case IWAVE:
            yoffset = (yMax-yMin)*0.1;
            if(yMax - yMin < 5)
            {
                yMax = 2;
                yMin = -2;
            }
            break;
        default:
            yoffset = (yMax-yMin)*0.1;
            yMin = 0;
            yMax += yoffset;
            break;
        }

        setupXRange(xMin-xoffset,xMax+xoffset);
        setupYRange(yMin,yMax);
    }

    myPlot->replot();
}

void FeartureChartWidget::addData(QString graph,int x,float y)
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

    QList<QString> names = curves.keys();
    QList<QString>::ConstIterator it;
    for(it=names.constBegin();it!=names.constEnd();it++)
    {
        QString name = *it;
        if(name.compare(graph) == 0)
        {
            curves[name]->setData(xAxise[graph],yAxise[graph]);
            break;
        }
    }

    if(xAutoRange)
    {
        float xmin,ymin,xmax,ymax;

        xmin = 0;
        ymin = 0;
        xmax = 0;
        ymax = 0;

        xmin = xAxise[graph].at(0);
        xmax = x;

        ymin = yAxise[graph].at(0);
        ymax = y;

        float xoffset = (xmax-xmin)*0.1;
        float yoffset = (ymax-ymin)*0.1;

        if(xmax - xmin < 5)
        {
            xmax += 2;
            xmin -= 2;
        }

        if(ymax - ymin < 5)
        {
            ymax += 2;
            ymin -= 2;
        }

        setupXRange(xmin-xoffset,xmax+xoffset);
        setupYRange(ymin-yoffset,ymax+yoffset);
    }

    /*
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
    */

    myPlot->replot();
}

void FeartureChartWidget::addGraph(QString title,QPen pen)
{
    //QCPGraph* graph = myPlot->addGraph();
    QCPCurve* graph = new QCPCurve(myPlot->xAxis,myPlot->yAxis);
    graph->setPen(pen);
    graph->setName(title);

    curves.insert(title,graph);
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

void FeartureChartWidget::setLegendBase(QString legend)
{
    this->legend_b = legend;
}

FeartureChartWidget::~FeartureChartWidget()
{
    delete myPlot;
    curves.clear();
}
