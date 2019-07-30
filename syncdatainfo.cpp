#include "syncdatainfo.h"

#include "globalvariable.h"
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>

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
    sync_btn->setText(tr("Sync Data"));

    cancel_sync = new QPushButton(this);
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

}

void SyncDataInfo::ok()
{

}
