#include "chartinfocontroller.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QresizeEvent>
#include <QTimer>
#include <QStringList>
#include <QException>

#include "constants.h"
#include "chartwidget.h"
#include "mainwindow.h"

#include "chargeinfo.h"
#include "vibrateinfo.h"
#include "temperatureinfo.h"

#include "freqinfo.h"
#include <QPen>

//#include "sqlitedb.h"

ChartInfoController::ChartInfoController(SqliteDB* db,QWidget *parent) : QWidget(parent),m_db(db)
{
    graph_u_title = tr("U-effective");
    graph_i_title = tr("I-effective");

    graph_u_freq_title = tr("U-Frequency");
    graph_i_freq_title = tr("I-Frequency");

    graph_add_title = tr("Acc-effective");
    graph_speed_title = tr("Speed-effective");

    graph_add_freq_title = tr("Acc-Frequency");

    graph_temperature_title = tr("Temperature");

    QHBoxLayout *layout = new QHBoxLayout();
    chartLayout = new QVBoxLayout();
    chart = new ChartWidget();
    chart->setMode(0);
    chart->setMaxPoint(200);
    chart->setupWindowsLabel(tr("time(s)"),tr("U/I(V/A)"));
    chart->setupXRange(0,200);
    chart->setupYRange(0,5);
    chart->setTitle(tr("Real-Time Data"));
    chart->setAutoRange(true,true);
    connect(chart, SIGNAL(openWaveForm()),this, SLOT(openWaveForm()));

    chartFreq = new ChartWidget();
    chartFreq->setMode(1);
    chartFreq->setMaxPoint(1024);
    chartFreq->setupWindowsLabel(tr("F(Hz)"),tr("F"));
    chartFreq->setupXRange(0,1024);
    chartFreq->setupYRange(0,5);
    chartFreq->setTitle(tr("Spectrum Data"));
    chartFreq->setAutoRange(true,true);

    chartLayout->addWidget(chart);
    chartLayout->addWidget(chartFreq);
    layout->addLayout(chartLayout);

    QWidget *right = new QWidget();
    QVBoxLayout *rightcontroller = new QVBoxLayout();
    right->setFixedWidth(WINDOW_RIGHT_WIDTH);
    rightgrid = new QGridLayout();
    rightgrid->setAlignment(Qt::AlignTop);
    right->setLayout(rightcontroller);

    QLabel *eLabel = new QLabel();
    eLabel->setText(tr("Attribute value"));

    rightgrid->addWidget(eLabel,0,0,1,2);
    //rightgrid->re

    rightcontroller->addLayout(rightgrid);
    rightcontroller->addStretch();

    layout->addWidget(right);

    this->setLayout(layout);

    this->motor = "";
    this->device = "";
    this->pipe = "";
    this->deviceType = "";
    this->deviceModel = "";

    initTime = QDateTime::currentDateTime();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer->start(BASE_INTERVAL);
}

void ChartInfoController::timeUpdate()
{
    if(!MainWindow::show_enable)
    {
        return;
    }
    try
    {
        if (e_table_name.length()>0)
        {
            if (e_table_name.compare("electriccharge") == 0)
            {
                if(MainWindow::charges.contains(this->pipe))
                {
                    if(MainWindow::charges[this->pipe].size()> 0)
                    {
                        ChargeInfo* info = MainWindow::charges[this->pipe].head();
                        QDateTime dt = QDateTime::fromString(info->rksj,"yyyy-MM-dd hh:mm:ss");
                        int x = initTime.secsTo(dt);
                        if (x > 0)
                        {
                            if(chart->containsGraph(graph_u_title))
                            {
                                chart->addData(graph_u_title,x,info->u);
                            }

                            if(chart->containsGraph(graph_i_title))
                            {
                                chart->addData(graph_i_title,x,info->i);
                            }

                            if(valueAttributeLabels.contains("u"))
                            {
                                valueAttributeLabels["u"]->setText(QString::number(info->u));
                            }
                            if(valueAttributeLabels.contains("i"))
                            {
                                valueAttributeLabels["i"]->setText(QString::number(info->i));
                            }
                            if(valueAttributeLabels.contains("f"))
                            {
                                valueAttributeLabels["f"]->setText(QString::number(info->f));
                            }
                            if(valueAttributeLabels.contains("factor"))
                            {
                                valueAttributeLabels["factor"]->setText(QString::number(info->factor));
                            }
                            if(valueAttributeLabels.contains("p"))
                            {
                                valueAttributeLabels["p"]->setText(QString::number(info->p));
                            }
                            if(valueAttributeLabels.contains("q"))
                            {
                                valueAttributeLabels["q"]->setText(QString::number(info->q));
                            }
                            if(valueAttributeLabels.contains("s"))
                            {
                                valueAttributeLabels["s"]->setText(QString::number(info->s));
                            }
                        }
                    }
                }
            }
            else if(e_table_name.compare("vibrate") == 0)
            {
                if(MainWindow::vibrates.contains(this->pipe))
                {
                    if (MainWindow::vibrates[this->pipe].size() > 0)
                    {
                        VibrateInfo* info = MainWindow::vibrates[this->pipe].head();
                        QDateTime dt = QDateTime::fromString(info->rksj,"yyyy-MM-dd hh:mm:ss");
                        int x = initTime.secsTo(dt);
                        if (x > 0)
                        {
                            if(chart->containsGraph(graph_add_title))
                            {
                                chart->addData(graph_add_title,x,info->vibrate_e);
                            }

                            if(chart->containsGraph(graph_speed_title))
                            {
                                chart->addData(graph_speed_title,x,info->speed_e);
                            }

                            if(valueAttributeLabels.contains("add"))
                            {
                                valueAttributeLabels["add"]->setText(QString::number(info->vibrate_e));
                            }
                            if(valueAttributeLabels.contains("speed"))
                            {
                                valueAttributeLabels["speed"]->setText(QString::number(info->speed_e));
                            }
                        }
                    }
                }
            }
            else if(e_table_name.compare("temperature") == 0)
            {
                if(MainWindow::temperatures.contains(this->pipe))
                {
                    if (MainWindow::temperatures[this->pipe].size()>0)
                    {
                        TemperatureInfo* info = MainWindow::temperatures[this->pipe].head();
                        QDateTime dt = QDateTime::fromString(info->rksj,"yyyy-MM-dd hh:mm:ss");
                        int x = initTime.secsTo(dt);
                        if (x > 0)
                        {
                            if(chart->containsGraph(graph_temperature_title))
                            {
                                chart->addData(graph_temperature_title,x,info->temp);
                            }

                            if(valueAttributeLabels.contains("temp"))
                            {
                                valueAttributeLabels["temp"]->setText(QString::number(info->temp));
                            }
                        }
                    }
                }
            }
        }

        if (freq_table_name.length()>0)
        {
            if (freq_table_name.compare("electricchargewavefreq") == 0)
            {
                if(MainWindow::freqs.contains(this->pipe))
                {
                    if(MainWindow::freqs[this->pipe].size() > 0)
                    {
                        //FreqInfo* info = MainWindow::freqs[this->pipe].head();
                        QQueue<FreqInfo*>::const_iterator it;
                        for(it=MainWindow::freqs[this->pipe].constBegin();it != MainWindow::freqs[this->pipe].constEnd();it++)
                        {
                            FreqInfo* tmp = *it;
                            if(chartFreq->containsGraph(graph_u_freq_title))
                            {
                                if (tmp->stype == 0)
                                {
                                    QString buffer = tmp->sample_freq;
                                    QStringList freqs = buffer.split(",");
                                    int index = 0;
                                    QStringList::const_iterator constIterator;
                                    //chartFreq->clearGraph(0);
                                    QVector<double> x_value, y_value;
                                    for (constIterator = freqs.constBegin(); constIterator != freqs.constEnd();++constIterator)
                                    {
                                        QString item = *constIterator;
                                        if (item.length() > 0)
                                        {
                                            bool tok;
                                            float hz;

                                            hz = item.toFloat(&tok);
                                            if (tok)
                                            {
                                                x_value.push_back(index);
                                                y_value.push_back(hz);
                                            }
                                            else
                                            {
                                                x_value.push_back(index);
                                                y_value.push_back(0);
                                            }
                                            index++;
                                        }
                                    }
                                    chartFreq->setData(graph_u_freq_title,x_value,y_value);
                                    //break;
                                }
                            }

                            if(chartFreq->containsGraph(graph_i_freq_title))
                            {
                                if (tmp->stype == 1)
                                {
                                    QString buffer = tmp->sample_freq;
                                    QStringList freqs = buffer.split(",");
                                    int index = 0;
                                    QStringList::const_iterator constIterator;
                                    //chartFreq->clearGraph(0);
                                    QVector<double> x_value, y_value;
                                    for (constIterator = freqs.constBegin(); constIterator != freqs.constEnd();++constIterator)
                                    {
                                        QString item = *constIterator;
                                        if (item.length() > 0)
                                        {
                                            bool tok;
                                            float hz;

                                            hz = item.toFloat(&tok);
                                            if (tok)
                                            {
                                                x_value.push_back(index);
                                                y_value.push_back(hz);
                                            }
                                            else
                                            {
                                                x_value.push_back(index);
                                                y_value.push_back(0);
                                            }
                                            index++;
                                        }
                                    }
                                    chartFreq->setData(graph_i_freq_title,x_value,y_value);
                                    //break;
                                }
                            }
                        }
                    }
                }
            }
            else if(freq_table_name.compare("vibratewavefreq") == 0)
            {
                if(MainWindow::freqs_v.contains(this->pipe))
                {
                    if(MainWindow::freqs_v[this->pipe].size()>0)
                    {
                        FreqInfo* info = MainWindow::freqs_v[this->pipe].head();
                        if(chartFreq->containsGraph(graph_add_freq_title))
                        {
                            if(info->stype == 0)
                            {
                                QString buffer = info->sample_freq;
                                QStringList freqs = buffer.split(",");
                                int index = 0;
                                QStringList::const_iterator constIterator;
                                //chartFreq->clearGraph(0);
                                QVector<double> x_value, y_value;
                                for (constIterator = freqs.constBegin(); constIterator != freqs.constEnd();++constIterator)
                                {
                                    QString item = *constIterator;
                                    if (item.length() > 0)
                                    {
                                        bool tok;
                                        float hz;

                                        hz = item.toFloat(&tok);
                                        if (tok)
                                        {
                                            x_value.push_back(index);
                                            y_value.push_back(hz);
                                        }
                                        else
                                        {
                                            x_value.push_back(index);
                                            y_value.push_back(0);
                                        }
                                        index++;
                                    }
                                }
                                chartFreq->setData(graph_add_freq_title,x_value,y_value);
                            }
                        }
                    }
                }
            }
        }
    }
    catch(QException e)
    {
        qDebug()<< e.what();
    }
}

void ChartInfoController::setCurDevicePipe(QString motor,QString device,QString pipe,QString deviceType)
{
    if (this->deviceType.compare(deviceType) || this->device.compare(device) || this->pipe.compare(pipe))
    {
        initTime = QDateTime::currentDateTime();
        /*
        chart->clearGraph(0);
        if (nullptr != chartFreq)
        {
            chartFreq->clearGraph(0);
        }
        */
    }

    this->motor = motor;
    this->device = device;
    this->pipe = pipe;
    this->deviceType = deviceType;

    valueAttributeLabels.clear();
    QVector<QLabel*>::const_iterator it;
    for(it=allAttributeLables.constBegin();it != allAttributeLables.constEnd();it++)
    {
        QLabel* item = *it;
        rightgrid->removeWidget(item);
        delete item;
        item = nullptr;
    }
    allAttributeLables.clear();

    if (this->deviceType.compare("E") == 0) //electric charge
    {
        QString spaceWidth = "        ";
        QLabel* u_label = new QLabel();
        u_label->setText(tr("U Effective(V): "));
        u_label->setAlignment(Qt::AlignRight);
        QLabel* u_value_label = new QLabel();
        u_value_label->setText(spaceWidth);

        allAttributeLables.append(u_label);
        allAttributeLables.append(u_value_label);
        valueAttributeLabels.insert("u",u_value_label);

        rightgrid->addWidget(u_label,1,0,1,1);
        rightgrid->addWidget(u_value_label,1,1,1,1);

        QLabel* i_label = new QLabel();
        i_label->setText(tr("I Effective(A): "));
        i_label->setAlignment(Qt::AlignRight);
        QLabel* i_value_label = new QLabel();
        i_value_label->setText(spaceWidth);

        allAttributeLables.append(i_label);
        allAttributeLables.append(i_value_label);
        valueAttributeLabels.insert("i",i_value_label);

        rightgrid->addWidget(i_label,2,0,1,1);
        rightgrid->addWidget(i_value_label,2,1,1,1);

        QLabel* f_label = new QLabel();
        f_label->setText(tr("Frequency(Hz): "));
        f_label->setAlignment(Qt::AlignRight);
        QLabel* f_value_label = new QLabel();
        f_value_label->setText(spaceWidth);

        allAttributeLables.append(f_label);
        allAttributeLables.append(f_value_label);
        valueAttributeLabels.insert("f",f_value_label);

        rightgrid->addWidget(f_label,3,0,1,1);
        rightgrid->addWidget(f_value_label,3,1,1,1);

        QLabel* factor_label = new QLabel();
        factor_label->setText(tr("Factor: "));
        factor_label->setAlignment(Qt::AlignRight);
        QLabel* factor_value_label = new QLabel();
        factor_value_label->setText(spaceWidth);

        allAttributeLables.append(factor_label);
        allAttributeLables.append(factor_value_label);
        valueAttributeLabels.insert("factor",factor_value_label);

        rightgrid->addWidget(factor_label,4,0,1,1);
        rightgrid->addWidget(factor_value_label,4,1,1,1);

        QLabel* p_label = new QLabel();
        p_label->setText(tr("P: "));
        p_label->setAlignment(Qt::AlignRight);
        QLabel* p_value_label = new QLabel();
        p_value_label->setText(spaceWidth);

        allAttributeLables.append(p_label);
        allAttributeLables.append(p_value_label);
        valueAttributeLabels.insert("p",p_value_label);

        rightgrid->addWidget(p_label,5,0,1,1);
        rightgrid->addWidget(p_value_label,5,1,1,1);

        QLabel* q_label = new QLabel();
        q_label->setText(tr("Q: "));
        q_label->setAlignment(Qt::AlignRight);
        QLabel* q_value_label = new QLabel();
        q_value_label->setText(spaceWidth);

        allAttributeLables.append(q_label);
        allAttributeLables.append(q_value_label);
        valueAttributeLabels.insert("q",q_value_label);

        rightgrid->addWidget(q_label,6,0,1,1);
        rightgrid->addWidget(q_value_label,6,1,1,1);

        QLabel* s_label = new QLabel();
        s_label->setText(tr("S: "));
        s_label->setAlignment(Qt::AlignRight);
        QLabel* s_value_label = new QLabel();
        s_value_label->setText(spaceWidth);

        allAttributeLables.append(s_label);
        allAttributeLables.append(s_value_label);
        valueAttributeLabels.insert("s",s_value_label);

        rightgrid->addWidget(s_label,7,0,1,1);
        rightgrid->addWidget(s_value_label,7,1,1,1);

        if (nullptr == chartFreq)
        {
            chartFreq = new ChartWidget();
            chartFreq->setMaxPoint(1024);
            chartFreq->setupWindowsLabel(tr("F(Hz)"),tr("F"));
            chartFreq->setupXRange(0,1024);
            chartFreq->setupYRange(0,5);
            chartFreq->setTitle(tr("Spectrum Data"));
            chartFreq->setAutoRange(true,true);

            chartLayout->addWidget(chartFreq);
        }
        chart->deleteAllGraph();
        chartFreq->deleteAllGraph();

        e_table_name = "electriccharge";
        freq_table_name = "electricchargewavefreq";

        chart->addGraph(graph_u_title,QPen(Qt::red));
        chart->addGraph(graph_i_title,QPen(Qt::blue));

        chartFreq->addGraph(graph_u_freq_title,QPen(Qt::red));
        chartFreq->addGraph(graph_i_freq_title,QPen(Qt::blue));
    }
    else if (this->deviceType.compare("V") == 0) //vibrate charge
    {
        QString spaceWidth = "        ";
        QLabel* add_label = new QLabel();
        add_label->setText(tr("Acc(m/ss): "));
        add_label->setAlignment(Qt::AlignRight);
        QLabel* add_value_label = new QLabel();
        add_value_label->setText(spaceWidth);

        allAttributeLables.append(add_label);
        allAttributeLables.append(add_value_label);
        valueAttributeLabels.insert("add",add_value_label);

        rightgrid->addWidget(add_label,1,0,1,1);
        rightgrid->addWidget(add_value_label,1,1,1,1);

        QLabel* speed_label = new QLabel();
        speed_label->setText(tr("speed(m/s): "));
        speed_label->setAlignment(Qt::AlignRight);
        QLabel* speed_value_label = new QLabel();
        speed_value_label->setText(spaceWidth);

        allAttributeLables.append(speed_label);
        allAttributeLables.append(speed_value_label);
        valueAttributeLabels.insert("speed",speed_value_label);

        rightgrid->addWidget(speed_label,2,0,1,1);
        rightgrid->addWidget(speed_value_label,2,1,1,1);

        chart->setupWindowsLabel(tr("time(s)"),tr("m/ss"));
        if (nullptr == chartFreq)
        {
            chartFreq = new ChartWidget();
            chartFreq->setMaxPoint(1024);
            chartFreq->setupWindowsLabel(tr("F(Hz)"),tr("F"));
            chartFreq->setupXRange(0,1024);
            chartFreq->setupYRange(0,5);
            chartFreq->setTitle(tr("Spectrum Data"));
            chartFreq->setAutoRange(true,true);

            chartLayout->addWidget(chartFreq);
        }

        chart->deleteAllGraph();
        chartFreq->deleteAllGraph();

        e_table_name = "vibrate";
        freq_table_name = "vibratewavefreq";

        chart->addGraph(graph_add_title,QPen(Qt::red));
        chart->addGraph(graph_speed_title,QPen(Qt::blue));

        chartFreq->addGraph(graph_add_freq_title,QPen(Qt::red));
    }
    else if (this->deviceType.compare("T") == 0) //temperature charge
    {
        QString spaceWidth = "        ";
        QLabel* temp_label = new QLabel();
        temp_label->setText(tr("Temperature: "));
        temp_label->setAlignment(Qt::AlignRight);
        QLabel* temp_value_label = new QLabel();
        temp_value_label->setText(spaceWidth);

        allAttributeLables.append(temp_label);
        allAttributeLables.append(temp_value_label);
        valueAttributeLabels.insert("temp",temp_value_label);

        rightgrid->addWidget(temp_label,1,0,1,1);
        rightgrid->addWidget(temp_value_label,1,1,1,1);

        chart->deleteAllGraph();
        //chart->setupWindows(1);
        chartLayout->removeWidget(chartFreq);
        delete chartFreq;
        chartFreq = nullptr;
        e_table_name = "temperature";
        freq_table_name = "";

        chart->setupWindowsLabel(tr("time(s)"),tr("°C"));
        chart->addGraph(graph_temperature_title,QPen(Qt::red));
    }
}

void ChartInfoController::openWaveForm()
{
    emit openWaveWindow(this->motor,this->device,this->pipe,this->deviceType);
}
