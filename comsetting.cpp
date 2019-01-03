#include "comsetting.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QTabWidget>
#include <QList>
#include <QResizeEvent>
#include <QDebug>
#include <QCheckBox>

#include "qiplineedit.h"

#include "mainwindow.h"
#include "cmieedevicelinkinfo.h"
#include "cmievdevicelinkinfo.h"
#include "cmietdevicelinkinfo.h"

ComSetting::ComSetting(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Device Com Setting"));
    this->setMinimumSize(350,300);
    mainWidget = new QTabWidget(this);
}

void ComSetting::resizeEvent(QResizeEvent* event)
{
    mainWidget->setFixedWidth(this->size().width());
    mainWidget->setFixedHeight(this->size().height());
}

void ComSetting::setupDevice(QMap<QString,DeviceLinkInfo*> devices)
{
    controls.clear();

    deviceInfo = devices;
    int tabs = mainWidget->count();
    for(int i=tabs-1;i>=0;i--)
    {
        mainWidget->removeTab(i);
    }
    QList<DeviceLinkInfo*> dList = devices.values();
    QList<DeviceLinkInfo*>::const_iterator it;
    for(it=dList.constBegin();it!=dList.constEnd();it++)
    {
        DeviceLinkInfo* item = *it;
        QWidget* board = new QWidget();
        QVBoxLayout *vBox = new QVBoxLayout;

        board->setLayout(vBox);

        int index = 0;
        QString dictKey = "";
        if(item->deviceModel.compare("CMIE-E") == 0)
        {
            vBox->addStretch();
            QGridLayout *grid = new QGridLayout();
            vBox->addLayout(grid);
            QLabel* ipLabel = new QLabel();
            ipLabel->setText(tr("Ip Address: "));
            grid->addWidget(ipLabel,index,0,1,1,Qt::AlignRight);
            QIPLineEdit* ipEdit = new QIPLineEdit();
            ipEdit->setText(item->getComAddress());
            dictKey = item->deviceName+"-ip";
            controls[dictKey] = ipEdit;
            grid->addWidget(ipEdit,index,1,1,1);

            index++;
            QLabel* ipUaLabel = new QLabel();
            ipUaLabel->setText(tr("Ua PT: "));
            grid->addWidget(ipUaLabel,index,0,1,1,Qt::AlignRight);
            QLineEdit* UaEdit = new QLineEdit();
            UaEdit->setText(item->getParam(1));
            dictKey = item->deviceName+"-pua";
            controls[dictKey] = UaEdit;
            grid->addWidget(UaEdit,index,1,1,1);

            index++;
            QLabel* ipUbLabel = new QLabel();
            ipUbLabel->setText(tr("Ub PT: "));
            grid->addWidget(ipUbLabel,index,0,1,1,Qt::AlignRight);
            QLineEdit* UbEdit = new QLineEdit();
            UbEdit->setText(item->getParam(2));
            dictKey = item->deviceName+"-pub";
            controls[dictKey] = UbEdit;
            grid->addWidget(UbEdit,index,1,1,1);

            index++;
            QLabel* ipUcLabel = new QLabel();
            ipUcLabel->setText(tr("Uc PT: "));
            grid->addWidget(ipUcLabel,index,0,1,1,Qt::AlignRight);
            QLineEdit* UcEdit = new QLineEdit();
            UcEdit->setText(item->getParam(3));
            dictKey = item->deviceName+"-puc";
            controls[dictKey] = UcEdit;
            grid->addWidget(UcEdit,index,1,1,1);

            index++;
            QLabel* ipIaLabel = new QLabel();
            ipIaLabel->setText(tr("Ia CT: "));
            grid->addWidget(ipIaLabel,index,0,1,1,Qt::AlignRight);
            QLineEdit* IaEdit = new QLineEdit();
            IaEdit->setText(item->getParam(4));
            dictKey = item->deviceName+"-cia";
            controls[dictKey] = IaEdit;
            grid->addWidget(IaEdit,index,1,1,1);

            index++;
            QLabel* ipIbLabel = new QLabel();
            ipIbLabel->setText(tr("Ib CT: "));
            grid->addWidget(ipIbLabel,index,0,1,1,Qt::AlignRight);
            QLineEdit* IbEdit = new QLineEdit();
            IbEdit->setText(item->getParam(5));
            dictKey = item->deviceName+"-cib";
            controls[dictKey] = IbEdit;
            grid->addWidget(IbEdit,index,1,1,1);

            index++;
            QLabel* ipIcLabel = new QLabel();
            ipIcLabel->setText(tr("Ic CT: "));
            grid->addWidget(ipIcLabel,index,0,1,1,Qt::AlignRight);
            QLineEdit* IcEdit = new QLineEdit();
            IcEdit->setText(item->getParam(6));
            dictKey = item->deviceName+"-cic";
            controls[dictKey] = IcEdit;
            grid->addWidget(IcEdit,index,1,1,1);
        }
        else if(item->deviceModel.compare("CMIE-V") == 0)
        {
            vBox->addStretch();
            QGridLayout *grid = new QGridLayout();
            vBox->addLayout(grid);
            QLabel* ipLabel = new QLabel();
            ipLabel->setText(tr("Ip Address: "));
            grid->addWidget(ipLabel,index,0,1,1,Qt::AlignRight);
            QIPLineEdit* ipEdit = new QIPLineEdit();
            ipEdit->setText(item->getComAddress());
            dictKey = item->deviceName+"-ip";
            controls[dictKey] = ipEdit;
            grid->addWidget(ipEdit,index,1,1,1);
        }
        else if(item->deviceModel.compare("CMIE-T") == 0)
        {
            vBox->addStretch();
            QGridLayout *grid = new QGridLayout();
            vBox->addLayout(grid);
            QLabel* ipLabel = new QLabel();
            ipLabel->setText(tr("Ip Address: "));
            grid->addWidget(ipLabel,index,0,1,1,Qt::AlignRight);
            QIPLineEdit* ipEdit = new QIPLineEdit();
            ipEdit->setText(item->getComAddress());
            dictKey = item->deviceName+"-ip";
            controls[dictKey] = ipEdit;
            grid->addWidget(ipEdit,index,1,1,1);
        }
        else
        {
            vBox->addStretch();
            QGridLayout *grid = new QGridLayout();
            vBox->addLayout(grid);
            QLabel* ipLabel = new QLabel();
            ipLabel->setText(tr("Ip Address: "));
            grid->addWidget(ipLabel,index,0,1,1,Qt::AlignRight);
            QIPLineEdit* ipEdit = new QIPLineEdit();
            ipEdit->setText(item->getComAddress());
            grid->addWidget(ipEdit,index,1,1,1);
        }

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

        vBox->addStretch();
        vBox->addLayout(btn);

        mainWidget->addTab(board,item->deviceName);
    }
}

void ComSetting::ok()
{
    int tabIndex = mainWidget->currentIndex();
    QString tabName = mainWidget->tabText(tabIndex);

    QList<DeviceLinkInfo*> dList = deviceInfo.values();
    QList<DeviceLinkInfo*>::const_iterator it;
    for(it=dList.constBegin();it!=dList.constEnd();it++)
    {
        DeviceLinkInfo* item = *it;
        if(item->deviceName.compare(tabName) == 0)
        {
            //QString itemType = item->deviceType;

            QString keyName = item->deviceName+"-ip";
            QString ip = "";
            if (controls.contains(keyName))
            {
                QIPLineEdit *ipEdit = static_cast<QIPLineEdit*>(controls[keyName]);
                ip = ipEdit->text();
            }
            if(item->deviceModel.compare("CMIE-E") == 0)
            {
                if(ip.compare(item->deviceIpAddress) != 0)
                {
                    //write ip address
                }
            }
            else if(item->deviceModel.compare("CMIE-V") == 0)
            {
                if(ip.compare(item->deviceIpAddress) != 0)
                {
                    //write ip address
                }
            }
            else if(item->deviceModel.compare("CMIE-T") == 0)
            {
                if(ip.compare(item->deviceIpAddress) != 0)
                {
                    //write ip address
                }
            }
            else
            {
            }

            break;
        }
    }
    this->close();
}

void ComSetting::cancel()
{
    this->close();
}
