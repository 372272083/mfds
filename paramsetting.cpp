#include "paramsetting.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include <QList>

#include <QDebug>
#include <QCheckBox>

#include "sqlitedb.h"
#include "qiplineedit.h"

#include "mainwindow.h"

ParamSetting::ParamSetting(SqliteDB *db,QWidget *parent) : QDialog(parent),mdb(db)
{
    this->setWindowTitle(tr("Parameter Setting Edit"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(400,400);
    //this->setMaximumSize(400,300);

    this->setWindowIcon(QIcon(":/images/icon"));

    QVBoxLayout *playout = new QVBoxLayout;

    int index = 0;

    QGridLayout *items = new QGridLayout;

    QLabel *pnameLabel = new QLabel(this);
    pnameLabel->setText(tr("Deployment name:"));
    nameEdit = new QLineEdit(this);
    nameEdit->setText(MainWindow::group_name);

    index++;
    items->addWidget(pnameLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(nameEdit,index,1,1,1);

    QLabel *pcomLabel = new QLabel(this);
    pcomLabel->setText(tr("Is Sample Data:"));
    com_enable = new QCheckBox(this);
    com_enable->setChecked(MainWindow::com_enable);

    index++;
    items->addWidget(pcomLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(com_enable,index,1,1,1);

    QLabel *pshowLabel = new QLabel(this);
    pshowLabel->setText(tr("Is Show Sample Data:"));
    show_enable = new QCheckBox(this);
    show_enable->setChecked(MainWindow::show_enable);

    index++;
    items->addWidget(pshowLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(show_enable,index,1,1,1);

    QLabel *pdesktopLabel = new QLabel(this);
    pdesktopLabel->setText(tr("Is Capture desktop:"));
    capture_desktop = new QCheckBox(this);
    capture_desktop->setChecked(MainWindow::desktop_capture_enable);

    index++;
    items->addWidget(pdesktopLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(capture_desktop,index,1,1,1);

    QLabel *pvirturalLabel = new QLabel(this);
    pvirturalLabel->setText(tr("Is Create Virtual Data:"));
    virtual_data = new QCheckBox(this);
    virtual_data->setChecked(MainWindow::virtual_data_enable);

    index++;
    items->addWidget(pvirturalLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(virtual_data,index,1,1,1);

    QLabel *pmodbusLabel = new QLabel(this);
    pmodbusLabel->setText(tr("Is Start Modbus server:"));
    modbus_server = new QCheckBox(this);
    modbus_server->setChecked(MainWindow::modbus_server_enable);

    index++;
    items->addWidget(pmodbusLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(modbus_server,index,1,1,1);

    index++;

    QLabel *pmodelLabel = new QLabel(this);
    pmodelLabel->setText(tr("Measure Sample Interval:"));
    QLabel *pmintervalMetricLabel = new QLabel(this);
    pmintervalMetricLabel->setText(tr("(s)"));
    measureIntervalEdit = new QLineEdit(this);
    measureIntervalEdit->setText(QString::number(MainWindow::measure_sample_interval));

    index++;
    items->addWidget(pmodelLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(measureIntervalEdit,index,1,1,1);
    items->addWidget(pmintervalMetricLabel,index,2,1,1);

    QLabel *ptypeLabel = new QLabel(this);
    ptypeLabel->setText(tr("Freq Sample Interval:"));
    QLabel *pfintervalMetricLabel = new QLabel(this);
    pfintervalMetricLabel->setText(tr("(s)"));
    freqIntervalEdit = new QLineEdit(this);
    freqIntervalEdit->setText(QString::number(MainWindow::freq_sample_interval));

    index++;
    items->addWidget(ptypeLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(freqIntervalEdit,index,1,1,1);
    items->addWidget(pfintervalMetricLabel,index,2,1,1);

    QLabel *prinLabel = new QLabel(this);
    prinLabel->setText(tr("Wave Sample Interval:"));
    QLabel *pwintervalMetricLabel = new QLabel(this);
    pwintervalMetricLabel->setText(tr("(s)"));
    waveIntervalEdit = new QLineEdit(this);
    waveIntervalEdit->setText(QString::number(MainWindow::wave_sample_interval));

    index++;
    items->addWidget(prinLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(waveIntervalEdit,index,1,1,1);
    items->addWidget(pwintervalMetricLabel,index,2,1,1);

    QLabel *proutLabel = new QLabel(this);
    proutLabel->setText(tr("Sample Waiting Time"));
    QLabel *psWaitingMetricLabel = new QLabel(this);
    psWaitingMetricLabel->setText(tr("(s)"));
    sampleWaitingEdit = new QLineEdit(this);
    sampleWaitingEdit->setText(QString::number(MainWindow::sample_waiting));

    index++;
    items->addWidget(proutLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(sampleWaitingEdit,index,1,1,1);
    items->addWidget(psWaitingMetricLabel,index,2,1,1);

    QLabel *pserverELabel = new QLabel(this);
    pserverELabel->setText(tr("Is Server Enable:"));
    server_enable = new QCheckBox(this);
    server_enable->setChecked(MainWindow::server_enable);

    index++;
    items->addWidget(pserverELabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(server_enable,index,1,1,1);

    QLabel *pserverLabel = new QLabel(this);
    pserverLabel->setText(tr("Server Ip Address:"));
    serverIpEdit = new QIPLineEdit(this);
    serverIpEdit->setText(MainWindow::server_ip);

    index++;
    items->addWidget(pserverLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(serverIpEdit,index,1,1,1);

    QLabel *pangleLabel = new QLabel(this);
    pangleLabel->setText(tr("Server Waiting Time:"));
    QLabel *pangleMetricLabel = new QLabel(this);
    pangleMetricLabel->setText(tr("(s)"));
    serverWaitingEdit = new QLineEdit(this);
    serverWaitingEdit->setText(QString::number(MainWindow::server_waiting));

    index++;
    items->addWidget(pangleLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(serverWaitingEdit,index,1,1,1);
    items->addWidget(pangleMetricLabel,index,2,1,1);

    QLabel *pdaysLabel = new QLabel(this);
    pdaysLabel->setText(tr("Data save days:"));
    savedaysEdit = new QLineEdit(this);
    savedaysEdit->setText(QString::number(MainWindow::data_save_days));

    index++;
    items->addWidget(pdaysLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(savedaysEdit,index,1,1,1);

    playout->addLayout(items);
    playout->setContentsMargins(10,10,10,10);

    QHBoxLayout *btn = new QHBoxLayout;
    btn->addStretch();

    QPushButton *pokbtn = new QPushButton(this);
    connect(pokbtn, SIGNAL(clicked()),this, SLOT(ok()));
    pokbtn->setText(tr("ok"));
    btn->addWidget(pokbtn);

    QPushButton *pcancelbtn = new QPushButton(this);
    connect(pcancelbtn, SIGNAL(clicked()),this, SLOT(cancel()));
    pcancelbtn->setText(tr("cancel"));
    btn->addWidget(pcancelbtn);

    playout->addLayout(btn);
    this->setLayout(playout);
}

void ParamSetting::ok()
{
    QString dname = nameEdit->text();
    QString mInterval = measureIntervalEdit->text();
    QString fInterval = freqIntervalEdit->text();
    QString wInterval = waveIntervalEdit->text();
    QString sampleWaiting = sampleWaitingEdit->text();
    QString serverIp = serverIpEdit->text();
    QString serverWaiting = serverWaitingEdit->text();
    QString days = savedaysEdit->text();

    if (dname.length() == 0 || mInterval.length() == 0 || fInterval.length() == 0 || wInterval.length() == 0 || sampleWaiting.length() == 0 || serverWaiting.length() == 0 || serverIp.length() == 0 || days.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return;
    }

    bool tok;

    int mInterval_i;
    int fInterval_i;
    int wInterval_i;
    int sampleWaiting_i;
    int serverWaiting_i;
    int days_i;

    mInterval_i = mInterval.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        measureIntervalEdit->setFocus();
        return;
    }

    fInterval_i = fInterval.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        freqIntervalEdit->setFocus();
        return;
    }

    wInterval_i = wInterval.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        waveIntervalEdit->setFocus();
        return;
    }

    sampleWaiting_i = sampleWaiting.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        sampleWaitingEdit->setFocus();
        return;
    }

    serverWaiting_i = serverWaiting.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        serverWaitingEdit->setFocus();
        return;
    }

    days_i = days.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        savedaysEdit->setFocus();
        return;
    }
    else if(days_i > 365 || days_i < 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Input number over range!"));
        savedaysEdit->setFocus();
        return;
    }

    if (mInterval_i != MainWindow::measure_sample_interval)
    {
        QString sql = "update setting set attr_value='" + mInterval + "' where id = 7";
        mdb->updatasql(sql);
        MainWindow::measure_sample_interval = mInterval_i;
    }

    if (fInterval_i != MainWindow::freq_sample_interval)
    {
        QString sql = "update setting set attr_value='" + fInterval + "' where id = 6";
        mdb->updatasql(sql);
        MainWindow::freq_sample_interval = fInterval_i;
    }

    if (wInterval_i != MainWindow::wave_sample_interval)
    {
        QString sql = "update setting set attr_value='" + wInterval + "' where id = 5";
        mdb->updatasql(sql);
        MainWindow::wave_sample_interval = wInterval_i;
    }

    if (sampleWaiting_i != MainWindow::sample_waiting)
    {
        QString sql = "update setting set attr_value='" + sampleWaiting + "' where id = 8";
        mdb->updatasql(sql);
        MainWindow::sample_waiting = sampleWaiting_i;
    }

    if (serverWaiting_i != MainWindow::server_waiting)
    {
        QString sql = "update setting set attr_value='" + serverWaiting + "' where id = 10";
        mdb->updatasql(sql);
        MainWindow::server_waiting = serverWaiting_i;
    }

    if (days_i != MainWindow::data_save_days)
    {
        QString sql = "update setting set attr_value='" + days + "' where id = 12";
        mdb->updatasql(sql);
        MainWindow::data_save_days = days_i;
    }

    if (serverIp.compare(MainWindow::server_ip) != 0)
    {
        QString sql = "update setting set attr_value='" + serverIp + "' where id = 9";
        mdb->updatasql(sql);
        MainWindow::server_ip = serverIp;
    }

    if (dname.compare(MainWindow::group_name) != 0)
    {
        QString sql = "update setting set attr_value='" + dname + "' where id = 11";
        mdb->updatasql(sql);
        MainWindow::group_name = dname;
    }

    if (com_enable->isChecked() != MainWindow::com_enable)
    {
        if(com_enable->isChecked())
        {
            QString sql = "update setting set attr_value='1' where id = 1";
            mdb->updatasql(sql);
        }
        else
        {
            QString sql = "update setting set attr_value='0' where id = 1";
            mdb->updatasql(sql);
        }

        MainWindow::com_enable = com_enable->isChecked();
    }

    if (show_enable->isChecked() != MainWindow::show_enable)
    {
        if(show_enable->isChecked())
        {
            QString sql = "update setting set attr_value='1' where id = 2";
            mdb->updatasql(sql);
        }
        else
        {
            QString sql = "update setting set attr_value='0' where id = 2";
            mdb->updatasql(sql);
        }

        MainWindow::show_enable = show_enable->isChecked();
    }

    if (virtual_data->isChecked() != MainWindow::virtual_data_enable)
    {
        if(virtual_data->isChecked())
        {
            QString sql = "update setting set attr_value='1' where id = 3";
            mdb->updatasql(sql);
        }
        else
        {
            QString sql = "update setting set attr_value='0' where id = 3";
            mdb->updatasql(sql);
        }

        MainWindow::virtual_data_enable = virtual_data->isChecked();
    }

    if (capture_desktop->isChecked() != MainWindow::desktop_capture_enable)
    {
        if(capture_desktop->isChecked())
        {
            QString sql = "update setting set attr_value='1' where id = 4";
            mdb->updatasql(sql);
        }
        else
        {
            QString sql = "update setting set attr_value='0' where id = 4";
            mdb->updatasql(sql);
        }

        MainWindow::desktop_capture_enable = capture_desktop->isChecked();
    }

    if (modbus_server->isChecked() != MainWindow::modbus_server_enable)
    {
        if(modbus_server->isChecked())
        {
            QString sql = "update setting set attr_value='1' where id = 13";
            mdb->updatasql(sql);
        }
        else
        {
            QString sql = "update setting set attr_value='0' where id = 13";
            mdb->updatasql(sql);
        }

        MainWindow::modbus_server_enable = modbus_server->isChecked();
    }

    if (server_enable->isChecked() != MainWindow::server_enable)
    {
        if(server_enable->isChecked())
        {
            QString sql = "update setting set attr_value='1' where id = 14";
            mdb->updatasql(sql);
        }
        else
        {
            QString sql = "update setting set attr_value='0' where id = 14";
            mdb->updatasql(sql);
        }

        MainWindow::server_enable = server_enable->isChecked();
    }

    this->close();
}

void ParamSetting::cancel()
{
    this->close();
}
