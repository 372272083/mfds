#include "faultwidget.h"

#include <QtWidgets/QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFont>
#include <QTimer>
#include <QProgressBar>
#include <QPushButton>
#include <QGroupBox>
#include <QDebug>

#include "globalvariable.h"
#include "faultinfo.h"
#include "faulttypeengine.h"
#include "faultdetailresultdialog.h"

FaultWidget::FaultWidget(MDISubWidget *parent) : MDISubWidget(parent)
{
    titleLabel = new QLabel(this);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255, 255, 255));
    titleLabel->setAutoFillBackground(true);  //一定要这句，否则不行
    titleLabel->setPalette(palette);

    QFont ft = QFont("sans", 17, QFont::Bold);
    titleLabel->setFont(ft);

    QPalette pal(this->palette());

    //设置背景黑色
    pal.setColor(QPalette::Background, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    //QWidget *containerleft = new QWidget;
    //QWidget *containerright = new QWidget;

    QGroupBox *left_box = new QGroupBox;
    QGroupBox *right_box = new QGroupBox;

    QFont box_ft = QFont("sans", 12, QFont::Bold);

    left_box->setFont(box_ft);
    right_box->setFont(box_ft);

    left_box->setTitle(tr("Vibrate Fault"));
    right_box->setTitle(tr("Electric Fault"));

    QHBoxLayout* containLayout = new QHBoxLayout();
    containLayout->setSpacing(20);
    containLayout->setContentsMargins(10,30,10,20);
    containLayout->addWidget(left_box);
    //containLayout->addWidget(right_box);

    QFont box_in_ft = QFont("sans", 10, QFont::Normal);

    QGridLayout* faultLayout = new QGridLayout();
    left_box->setLayout(faultLayout);
    faultLayout->setAlignment(Qt::AlignTop);

    FaultTypeEngine fte;
    QMap<int,QString>::ConstIterator it;
    int index = 0;
    for(it=fte.faults.constBegin();it!=fte.faults.constEnd();it++)
    {
        int key = it.key();
        QString fault_str = it.value();
        QLabel* name = new QLabel(this);
        name->setText(fault_str);
        name->setFont(box_in_ft);

        fault_names_v.append(name);
        faultLayout->addWidget(name,index,0,1,1);

        QProgressBar* pb = new QProgressBar(this);
        pb->setValue(100);
        pb->setTextVisible(false);
        pb->setStyleSheet("QProgressBar::chunk { background-color: rgb(112,173,70) }");
        connect(pb,SIGNAL(valueChanged(int)),this,SLOT(valueChangedSlot(int)));

        fault_percents_v.append(pb);
        faultLayout->addWidget(pb,index,1,1,1);

        QPushButton* detail = new QPushButton(this);
        connect(detail,SIGNAL(clicked(bool)),this,SLOT(detailClicked(bool)));
        detail->setFont(box_in_ft);
        detail->setText(tr("Detail"));
        fault_details_v.append(detail);
        faultLayout->addWidget(detail,index,2,1,1);

        fault_map[key] = index;
        index++;
    }

    /*
    QGridLayout* faultLayout_e = new QGridLayout();
    faultLayout_e->setAlignment(Qt::AlignTop);
    right_box->setLayout(faultLayout_e);

    //FaultTypeEngine fte;
    index = 0;
    for(it=fte.faults.constBegin();it!=fte.faults.constEnd();it++)
    {
        int key = it.key();
        QString fault_str = it.value();
        QLabel* name = new QLabel(this);
        name->setText(fault_str);
        name->setFont(box_in_ft);

        fault_names_e.append(name);
        faultLayout_e->addWidget(name,index,0,1,1);

        QProgressBar* pb = new QProgressBar(this);

        fault_percents_e.append(pb);
        faultLayout_e->addWidget(pb,index,1,1,1);

        QPushButton* detail = new QPushButton(this);
        detail->setFont(box_in_ft);
        detail->setText(tr("Detail"));
        fault_details_e.append(detail);
        faultLayout_e->addWidget(detail,index,2,1,1);

        index++;
    }
    */
    QVBoxLayout *hLayout = new QVBoxLayout(this);
    hLayout->setSpacing(0);
    hLayout->addWidget(titleLabel,0,Qt::AlignCenter);
    hLayout->addLayout(containLayout,1);

    this->setLayout(hLayout);

    //fillSqrtSinProxy();
    initTime = QDateTime::currentDateTime();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer->start(BASE_INTERVAL);
}

void FaultWidget::setTitle(QString title)
{
    this->setWindowTitle(title);
    titleLabel->setText(title);
}

void FaultWidget::valueChangedSlot(int value)
{
    //QProgressBar* qpb = (QProgressBar*)sender();
    //if(value)
    //qpb->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");

    //(112,173,70)
    //(255,231,153)
    //(255,193,0)
    //(255,0,0)
}

void FaultWidget::detailClicked(bool)
{
    QObject *pObject = this->sender();
    QPushButton *pButton = qobject_cast<QPushButton *>(pObject);

    QVector<QPushButton*>::ConstIterator cIt;
    int index = 0;
    bool flag = false;
    for(cIt=fault_details_v.constBegin();cIt!=fault_details_v.constEnd();cIt++)
    {
        QPushButton* btn_v = *cIt;
        if(btn_v == pButton)
        {
            flag = true;
            break;
        }
        index++;
    }
    if(flag)
    {
        QList<int> mapkeys = fault_map.keys();
        QList<int>::ConstIterator mapIt;
        for(mapIt=mapkeys.constBegin();mapIt!=mapkeys.constEnd();mapIt++)
        {
            int map_k = *mapIt;
            int map_v = fault_map[map_k];
            if(map_v == index)
            {
                FaultDetailResultDialog* fdrd = new FaultDetailResultDialog();
                fdrd->setAttribute(Qt::WA_DeleteOnClose);
                fdrd->setDetailInfo(m_mcode, map_k);
                fdrd->show();
            }
        }
    }
}

void FaultWidget::setOfflineData(QString data,QString sample,QString interval)
{

}

QString FaultWidget::title()
{
    return this->windowTitle();
}

void FaultWidget::timeUpdate()
{
    QVector<QProgressBar*>::ConstIterator qbIt;
    for(qbIt=fault_percents_v.constBegin();qbIt!=fault_percents_v.constEnd();qbIt++)
    {
        QProgressBar* qb = *qbIt;
        //qb->setValue(100);
        qb->setStyleSheet("QProgressBar::chunk { background-color: rgb(112,173,70) }");
    }
    if(GlobalVariable::diagnose_result.contains(m_mcode))
    {
        QList<QString>::ConstIterator in_it;
        QMap<QString,FaultInfo> fis = GlobalVariable::diagnose_result[m_mcode];
        QList<QString> in_keys = fis.keys();

        QMap<int,int> degrees;
        for(int i=0;i<20;i++)
        {
            degrees[i] = 0;
        }

        for(in_it=in_keys.constBegin();in_it!=in_keys.constEnd();in_it++)
        {
            QString key = *in_it;
            FaultInfo fi = fis[key];

            int in_index=0;
            QVector<int>::ConstIterator iIt;
            for(iIt=fi.fault_codes.constBegin();iIt!=fi.fault_codes.constEnd();iIt++)
            {
                int value = *iIt;
                if(value != -1) // fault is existed
                {
                    int fault_code_1 = value/1000;
                    qDebug() << "fault code: " << QString::number(fault_code_1);
                    int fault_code_2 = (value - fault_code_1 * 1000)/100;
                    int fault_degree = (value - fault_code_1 * 1000 - fault_code_2 * 100)/10;

                    /*
                    if(degrees.contains(in_index))
                    {
                        if(degrees[in_index] < fault_degree)
                        {
                            degrees[in_index] = fault_degree;
                        }
                    }
                    else
                    {
                        degrees[in_index] = fault_degree;
                    }
                    */
                    if(fault_degree > degrees[in_index])
                    {
                        degrees[in_index] = fault_degree;
                        switch (fault_degree) {
                        case 1:
                            fault_percents_v[fault_map[fault_code_1]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(112,173,70) }");
                            break;
                        case 2:
                            fault_percents_v[fault_map[fault_code_1]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(255,231,153) }");
                            break;
                        case 3:
                            fault_percents_v[fault_map[fault_code_1]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(255,193,0) }");
                            break;
                        case 4:
                            fault_percents_v[fault_map[fault_code_1]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(255,0,0) }");
                            break;
                        default:
                            break;
                        }
                    }
                }
                in_index++;
            }
        }
    }
}
