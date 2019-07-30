#include "pathtrackingchannelselect.h"

#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

#include <QMessageBox>

#include "globalvariable.h"
#include "channelinfo.h"

PathTrackingChannelSelect::PathTrackingChannelSelect(QDialog *parent):QDialog(parent)
{
    this->setMaximumSize(250,200);
    this->setMinimumSize(250,200);

    QGridLayout* layout = new QGridLayout();

    QLabel* xAxisLabel = new QLabel(this);
    xAxisLabel->setText(tr("X Axis Channel:"));
    xAxisCB = new QComboBox(this);

    layout->addWidget(xAxisLabel,0,0,1,1,Qt::AlignRight);
    layout->addWidget(xAxisCB,0,1,1,1,Qt::AlignLeft);

    QLabel* yAxisLabel = new QLabel(this);
    yAxisLabel->setText(tr("Y Axis Channel:"));
    yAxisCB = new QComboBox(this);

    layout->addWidget(yAxisLabel,1,0,1,1,Qt::AlignRight);
    layout->addWidget(yAxisCB,1,1,1,1,Qt::AlignLeft);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(layout);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();

    QPushButton *pokbtn = new QPushButton(this);
    connect(pokbtn, SIGNAL(clicked()),this, SLOT(ok()));
    pokbtn->setText(tr("ok"));
    buttonLayout->addWidget(pokbtn);

    QPushButton *pcancelbtn = new QPushButton(this);
    connect(pcancelbtn, SIGNAL(clicked()),this, SLOT(cancel()));
    pcancelbtn->setText(tr("cancel"));
    buttonLayout->addWidget(pcancelbtn);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void PathTrackingChannelSelect::setmode(int mode)
{
    this->mode = mode;
    QVector<ChannelInfo*>::ConstIterator it;
    for(it=GlobalVariable::channelInfos.constBegin();it!=GlobalVariable::channelInfos.constEnd();it++)
    {
        ChannelInfo* ci = *it;
        if(ci->pid = mode)
        {
            QString dcode = ci->device_code;
            DeviceInfo* dtype = GlobalVariable::findDeviceByCode(dcode);
            if(dtype != nullptr)
            {
                if(dtype->deviceType.compare("V",Qt::CaseInsensitive) == 0)
                {
                    this->xAxisCB->addItem(ci->title,ci->tree_id);
                    this->yAxisCB->addItem(ci->title,ci->tree_id);
                }
            }
        }
    }
}

void PathTrackingChannelSelect::ok()
{
    if(xAxisCB->currentIndex() >= 0)
    {
        QString x_id_s = xAxisCB->itemData(xAxisCB->currentIndex()).toString();
        QString y_id_s = yAxisCB->itemData(yAxisCB->currentIndex()).toString();

        int x_id,y_id;
        bool tok;
        x_id = x_id_s.toInt(&tok);
        y_id = y_id_s.toInt(&tok);

        QString x_device = GlobalVariable::findDeviceCodeByChannelTreeId(x_id);
        QString y_device = GlobalVariable::findDeviceCodeByChannelTreeId(y_id);

        if(x_device.compare(y_device,Qt::CaseInsensitive) != 0)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Selected Channels must be in same Device!"));
            return;
        }

        emit channelSelected(x_id,y_id);
        this->close();
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please select a item!"));
    }
}

void PathTrackingChannelSelect::cancel()
{
    this->close();
}
