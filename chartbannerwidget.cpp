#include "chartbannerwidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>

ChartBannerWidget::ChartBannerWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);

    this->setAutoFillBackground(true);
    this->setStyleSheet(tr("border:0px solid gray"));

    /*
    s_time = new QLabel();
    s_time->setText(tr("sample time"));

    position = new QLabel();
    position->setText(tr("Position"));
    */

    rms = new QLabel();
    rms->setText(tr("RMS"));

    pkpk = new QLabel();
    pkpk->setText(tr("PKPK"));

    //layout->addWidget(s_time);
    //layout->addWidget(position);
    layout->addWidget(rms);
    layout->addWidget(pkpk);

    layout->setSpacing(0);
}

void ChartBannerWidget::setRMS(QString t)
{
    rms->setText(t);
}

void ChartBannerWidget::setPKPK(QString t)
{
    pkpk->setText(t);
}

void ChartBannerWidget::setRMSVisible(bool v)
{
    rms->setHidden(v);
}
void ChartBannerWidget::setPKPKVisible(bool v)
{
    pkpk->setHidden(v);
}
