#include "devicepipeselectwidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QColor>

#include "devicepipeinfo.h"
#include "headeritemwidget.h"

DevicePipeSelectWidget::DevicePipeSelectWidget(QWidget *parent) : QWidget(parent)
{
    banner = new QLabel();
    banner->setText(tr("please select a motor from left widget"));

    pLayout = new QHBoxLayout();
    pLayout->addWidget(banner);
    this->setLayout(pLayout);
}

void DevicePipeSelectWidget::setupPipes(QList<DevicePipeInfo *> list)
{
    int len = list.count();

    int lenb = buttonItems.size();
    for(int i=0;i<lenb;i++)
    {
        delete buttonItems[i];
    }
    buttonItems.clear();

    delete pLayout;
    pLayout = nullptr;

    pLayout = new QHBoxLayout();
    this->setLayout(pLayout);
    if (len == 0)
    {
        if (nullptr == banner)
        {
            banner = new QLabel();
            banner->setText(tr("please select a motor from left widget"));
        }
        pLayout->addWidget(banner);
    }
    else
    {
        pLayout->removeWidget(banner);
        delete banner;
        banner = nullptr;
        int len = list.count();
        for (int n=0;n<len;n++)
        {
            DevicePipeInfo* dpi = list.at(n);

            HeaderItemWidget* bitem = new HeaderItemWidget();

            connect(bitem,SIGNAL(clicked()),this,SLOT(pipeToggle()));
            bitem->motor = dpi->getMCode();
            bitem->device = dpi->getDCode();
            bitem->pipe = dpi->getPipeCode();
            bitem->deviceType = dpi->getDType();
            //bitem->deviceModel = item->deviceModel;
            QString pipeShowName = dpi->getDName()+"-"+dpi->getPipeName();
            bitem->setText(pipeShowName);
            pLayout->addWidget(bitem);
            buttonItems.append(bitem);
            buttonOriginColor = bitem->palette().color(QPalette::ButtonText);
        }

        pLayout->addStretch();
    }
}

void DevicePipeSelectWidget::pipeToggle()
{
    int lenb = buttonItems.size();
    for(int i=0;i<lenb;i++)
    {
        QPalette pal = buttonItems[i]->palette();
        pal.setColor(QPalette::ButtonText,buttonOriginColor);
        buttonItems[i]->setPalette(pal);
        buttonItems[i]->setAutoFillBackground(true);
        //buttonItems[i]->setFlat(true);
    }

    //QTcpSocket *pSocket = qobject_cast<QTcpSocket*>(sender());
    HeaderItemWidget *hiw = qobject_cast<HeaderItemWidget*>(sender());
    QPalette pal = hiw->palette();
    pal.setColor(QPalette::ButtonText,Qt::blue);
    hiw->setPalette(pal);
    hiw->setAutoFillBackground(true);
    //hiw->setFlat(true);

    emit pipeChanged(hiw->motor,hiw->device,hiw->pipe,hiw->deviceType);
}
