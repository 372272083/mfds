#include "syncdatainfo.h"

#include "globalvariable.h"
#include "constants.h"
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTimer>

SyncDataInfo::SyncDataInfo(QWidget *parent) : QDialog(parent)
{
    this->setFixedSize(600,150);
    QGridLayout* main_layout = new QGridLayout(this);
    main_layout->setAlignment(Qt::AlignCenter);
    int index = 0;

    info = new QLabel(this);
    info->setText(tr("start sync data"));
    main_layout->addWidget(info,index,0,1,1,Qt::AlignLeft);

    index++;
    pecentage = new QProgressBar(this);
    pecentage->setFixedWidth(560);
    pecentage->setValue(0);
    main_layout->addWidget(pecentage,index,0,1,1,Qt::AlignLeft);

    index++;
    sync_btn = new QPushButton(this);
    connect(sync_btn,&QPushButton::clicked,this,&SyncDataInfo::ok);
    sync_btn->setText(tr("Sync Data"));

    cancel_sync = new QPushButton(this);
    connect(cancel_sync,&QPushButton::clicked,this,&SyncDataInfo::cancel);
    cancel_sync->setText(tr("Cancel Sync Data"));

    QHBoxLayout* bottom = new QHBoxLayout(this);
    bottom->addStretch();
    bottom->addWidget(sync_btn);
    bottom->addWidget(cancel_sync);
    main_layout->addLayout(bottom,index,0,1,1,Qt::AlignRight);

    this->setLayout(main_layout);
}

void SyncDataInfo::cancel()
{
    GlobalVariable::is_sync = false;
    timer->stop();
    delete timer;
    timer = nullptr;

    this->close();
}

void SyncDataInfo::timeUpdate()
{
    static int count = 0;
    static int sum = 100;
    count++;

    int s = GlobalVariable::sync_process;
    int state = s & 0x80;
    int p100 = s & 0x40;
    int p80 = s & 0x20;
    int p64 = s & 0x10;
    int p48 = s & 0x8;
    int p32 = s & 0x4;
    int p16 = s & 0x2;

    if(state > 0)
    {
        return;
    }

    if(p100 > 0)
    {
        info->setText(tr("sync data done!"));
        pecentage->setValue(100);
        timer->stop();
        delete timer;
        timer = nullptr;
        this->close();
    }
    else if(p80 > 0)
    {
        info->setText(tr("sync device channel data..."));
        pecentage->setValue(80.0);
    }
    else if(p64 > 0)
    {
        info->setText(tr("sync device type data..."));
        pecentage->setValue(64.0);
    }
    else if(p48 > 0)
    {
        info->setText(tr("sync bearing type data..."));
        pecentage->setValue(48.0);
    }
    else if(p32 > 0)
    {
        info->setText(tr("sync motor type data..."));
        pecentage->setValue(32.0);
    }
    else if(p16 > 0)
    {
        info->setText(tr("sync device data..."));
        pecentage->setValue(16.0);
    }
    else
    {
        info->setText(tr("sync motor data..."));
    }
}

void SyncDataInfo::ok()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer->start(BASE_INTERVAL * 2);
    sync_btn->setEnabled(false);

    GlobalVariable::is_sync = true;
    GlobalVariable::sync_process = 0;
}
