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
    if(GlobalVariable::s_t == 1 || GlobalVariable::s_t == 0)
    {
        containLayout->addWidget(left_box);
    }
    if(GlobalVariable::s_t == 2 || GlobalVariable::s_t == 0)
    {
        containLayout->addWidget(right_box);
    }

    QFont box_in_ft = QFont("sans", 10, QFont::Normal);

    QGridLayout* faultLayout = new QGridLayout();
    left_box->setLayout(faultLayout);
    faultLayout->setAlignment(Qt::AlignTop);

    FaultTypeEngine fte;
    QMap<int,QString>::ConstIterator it;
    int index = 0;
    for(it=fte.faults_v.constBegin();it!=fte.faults_v.constEnd();it++)
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
        pb->setStyleSheet("QProgressBar::chunk { background-color: rgb(114,244,174) }");
        connect(pb,SIGNAL(valueChanged(int)),this,SLOT(valueChangedSlot(int)));

        fault_percents_v.append(pb);
        faultLayout->addWidget(pb,index,1,1,1);

        QPushButton* detail = new QPushButton(this);
        connect(detail,SIGNAL(clicked(bool)),this,SLOT(detailClicked(bool)));
        detail->setFont(box_in_ft);
        detail->setText(tr("Detail"));
        fault_details_v.append(detail);
        faultLayout->addWidget(detail,index,2,1,1);

        fault_map_v[key] = index;
        index++;
    }

    QVBoxLayout* faultLayout_e_layout = new QVBoxLayout();
    QGridLayout* faultLayout_e = new QGridLayout();
    faultLayout_e->setSpacing(0);
    faultLayout_e_layout->setAlignment(Qt::AlignTop);
    faultLayout_e_layout->addLayout(faultLayout_e);
    right_box->setLayout(faultLayout_e_layout);

    //FaultTypeEngine fte;
    index = 0;
    QLabel* source = new QLabel();
    source->setText(tr("Power Source"));
    source->setAutoFillBackground(true);
    source->setStyleSheet(tr("border:1px solid gray"));
    faultLayout_e->addWidget(source,index,0,4,1);

    QLabel* motor = new QLabel();
    motor->setText(tr("Motor"));
    motor->setAutoFillBackground(true);
    motor->setStyleSheet(tr("border:1px solid gray"));
    faultLayout_e->addWidget(motor,4,0,8,1);

    QLabel* machine = new QLabel();
    machine->setText(tr("Machine"));
    machine->setAutoFillBackground(true);
    machine->setStyleSheet(tr("border:1px solid gray"));
    faultLayout_e->addWidget(machine,12,0,4,1);

    for(it=fte.faults_e.constBegin();it!=fte.faults_e.constEnd();it++)
    {
        int key = it.key();
        QString fault_str = it.value();
        QLabel* name = new QLabel(this);
        name->setText(fault_str);
        name->setFont(box_in_ft);
        name->setAutoFillBackground(true);
        name->setStyleSheet(tr("border:1px solid gray"));

        fault_names_e.append(name);
        faultLayout_e->addWidget(name,index,1,1,1);

        QProgressBar* pb = new QProgressBar(this);
        pb->setValue(100);
        pb->setTextVisible(false);
        pb->setStyleSheet("QProgressBar::chunk { background-color: rgb(114,244,174) }");
        connect(pb,SIGNAL(valueChanged(int)),this,SLOT(valueChangedSlot(int)));

        fault_percents_e.append(pb);
        faultLayout_e->addWidget(pb,index,2,1,1);

        QPushButton* detail = new QPushButton(this);
        connect(detail,SIGNAL(clicked(bool)),this,SLOT(detailClicked(bool)));
        detail->setFont(box_in_ft);
        detail->setText(tr("Detail"));
        fault_details_e.append(detail);
        faultLayout_e->addWidget(detail,index,3,1,1);

        fault_map_e[key] = index;
        index++;
    }

    faultLayout_e_layout->addSpacing(10);

    QHBoxLayout* noise_level_layout = new QHBoxLayout();
    QLabel* noise_level_label = new QLabel();
    noise_level_label->setText(tr("Noise level(db): "));
    noise_level_layout->addWidget(noise_level_label);

    e_noise_level_label = new QLabel();
    e_noise_level_label->setText(tr("    "));
    noise_level_layout->addWidget(e_noise_level_label);

    faultLayout_e_layout->addLayout(noise_level_layout);

    QHBoxLayout* rotor_bar_num_layout = new QHBoxLayout();
    QLabel* rotor_bar_num_label = new QLabel();
    rotor_bar_num_label->setText(tr("Estimate rotor bar number: "));
    rotor_bar_num_layout->addWidget(rotor_bar_num_label);

    e_estimate_rotor_bar_num_label = new QLabel();
    e_estimate_rotor_bar_num_label->setText(tr("    "));
    rotor_bar_num_layout->addWidget(e_estimate_rotor_bar_num_label);

    faultLayout_e_layout->addLayout(rotor_bar_num_layout);

    QHBoxLayout* stator_slot_num_layout = new QHBoxLayout();
    QLabel* stator_slot_num_label = new QLabel();
    stator_slot_num_label->setText(tr("Estimate stator slot number: "));
    stator_slot_num_layout->addWidget(stator_slot_num_label);

    e_estimate_stator_slot_num_lable = new QLabel();
    e_estimate_stator_slot_num_lable->setText(tr("    "));
    stator_slot_num_layout->addWidget(e_estimate_stator_slot_num_lable);

    faultLayout_e_layout->addLayout(stator_slot_num_layout);


    QVBoxLayout *hLayout = new QVBoxLayout(this);
    hLayout->setSpacing(0);
    hLayout->addWidget(titleLabel,0,Qt::AlignCenter);
    hLayout->addLayout(containLayout,1);

    QHBoxLayout* tipsLayout = new QHBoxLayout();

    QLabel* unknown_color_label = new QLabel();
    unknown_color_label->setFixedHeight(20);
    unknown_color_label->setText(tr("    "));
    unknown_color_label->setStyleSheet("background-color: rgb(114,244,174)");
    tipsLayout->addWidget(unknown_color_label);

    QLabel* unknown_label = new QLabel();
    unknown_label->setFixedHeight(20);
    unknown_label->setText(tr(" Unknown "));
    tipsLayout->addWidget(unknown_label);

    QLabel* normal_color_label = new QLabel();
    normal_color_label->setFixedHeight(20);
    normal_color_label->setText(tr("    "));
    normal_color_label->setStyleSheet("background-color: rgb(0,176,80)");
    tipsLayout->addWidget(normal_color_label);

    QLabel* normal_label = new QLabel();
    normal_label->setFixedHeight(20);
    normal_label->setText(tr(" Normal "));
    tipsLayout->addWidget(normal_label);

    QLabel* Suspected_color_label = new QLabel();
    Suspected_color_label->setFixedHeight(20);
    Suspected_color_label->setText(tr("    "));
    Suspected_color_label->setStyleSheet("background-color: rgb(0,176,240)");
    tipsLayout->addWidget(Suspected_color_label);

    QLabel* Suspected_label = new QLabel();
    Suspected_label->setFixedHeight(20);
    Suspected_label->setText(tr(" Suspected malfunction "));
    tipsLayout->addWidget(Suspected_label);

    QLabel* mild_color_label = new QLabel();
    mild_color_label->setFixedHeight(20);
    mild_color_label->setText(tr("    "));
    mild_color_label->setStyleSheet("background-color: rgb(255,215,0)");
    tipsLayout->addWidget(mild_color_label);

    QLabel* mild_label = new QLabel();
    mild_label->setFixedHeight(20);
    mild_label->setText(tr(" Mild malfunction "));
    tipsLayout->addWidget(mild_label);

    QLabel* general_color_label = new QLabel();
    general_color_label->setFixedHeight(20);
    general_color_label->setText(tr("    "));
    general_color_label->setStyleSheet("background-color: rgb(112,48,160)");
    tipsLayout->addWidget(general_color_label);

    QLabel* general_label = new QLabel();
    general_label->setFixedHeight(20);
    general_label->setText(tr(" General failure "));
    tipsLayout->addWidget(general_label);

    //Serious malfunction
    QLabel* serious_color_label = new QLabel();
    serious_color_label->setFixedHeight(20);
    serious_color_label->setText(tr("    "));
    serious_color_label->setStyleSheet("background-color: rgb(255,0,0)");
    tipsLayout->addWidget(serious_color_label);

    QLabel* serious_label = new QLabel();
    serious_label->setFixedHeight(20);
    serious_label->setText(tr(" Serious malfunction "));
    tipsLayout->addWidget(serious_label);

    tipsLayout->addStretch();

    hLayout->addLayout(tipsLayout);

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
        QList<int> mapkeys = fault_map_v.keys();
        QList<int>::ConstIterator mapIt;
        for(mapIt=mapkeys.constBegin();mapIt!=mapkeys.constEnd();mapIt++)
        {
            int map_k = *mapIt;
            int map_v = fault_map_v[map_k];
            if(map_v == index)
            {
                FaultDetailResultDialog* fdrd = new FaultDetailResultDialog();
                fdrd->setAttribute(Qt::WA_DeleteOnClose);
                fdrd->setDetailInfo(m_mcode, map_k,0);
                fdrd->show();
            }
        }
    }
    else
    {
        flag = false;
        index = 0;
        for(cIt=fault_details_e.constBegin();cIt!=fault_details_e.constEnd();cIt++)
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
            QList<int> mapkeys = fault_map_e.keys();
            QList<int>::ConstIterator mapIt;
            for(mapIt=mapkeys.constBegin();mapIt!=mapkeys.constEnd();mapIt++)
            {
                int map_k = *mapIt;
                int map_v = fault_map_e[map_k];
                if(map_v == index)
                {
                    FaultDetailResultDialog* fdrd = new FaultDetailResultDialog();
                    fdrd->setAttribute(Qt::WA_DeleteOnClose);
                    fdrd->setDetailInfo(m_mcode, map_k,1);
                    fdrd->show();
                }
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
        qb->setStyleSheet("QProgressBar::chunk { background-color: rgb(114,244,174) }");
    }
    for(qbIt=fault_percents_e.constBegin();qbIt!=fault_percents_e.constEnd();qbIt++)
    {
        QProgressBar* qb = *qbIt;
        //qb->setValue(100);
        qb->setStyleSheet("QProgressBar::chunk { background-color: rgb(114,244,174) }");
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

            e_estimate_rotor_bar_num_label->setText(QString::number(fi.fault_electric_details.diagnosis.estimated_number_of_rotor_bar));
            e_noise_level_label->setText(QString::number(fi.fault_electric_details.diagnosis.noise_db,10,4));
            e_estimate_stator_slot_num_lable->setText(QString::number(fi.fault_electric_details.diagnosis.estimated_number_of_stator_slot));

            electricBar(fi.fault_electric_details.diagnosis.level_vdev_con,1);
            electricBar(fi.fault_electric_details.diagnosis.level_vbias_con,2);
            electricBar(fi.fault_electric_details.diagnosis.level_vcf_con,3);
            electricBar(fi.fault_electric_details.diagnosis.level_vharmoinc_con,4);

            electricBar(fi.fault_electric_details.diagnosis.level_pf_con,5);
            electricBar(fi.fault_electric_details.diagnosis.level_load_con,6);
            electricBar(fi.fault_electric_details.diagnosis.level_ibalance_con,7);
            electricBar(fi.fault_electric_details.diagnosis.level_ccf_con,8);
            electricBar(fi.fault_electric_details.diagnosis.level_charmonic_con,9);
            electricBar(fi.fault_electric_details.diagnosis.level_rotorhealth_con,10);
            electricBar(fi.fault_electric_details.diagnosis.level_statorhealth_con,11);
            electricBar(fi.fault_electric_details.diagnosis.level_dynamic_or_static_ecc_con,12);
            electricBar(fi.fault_electric_details.diagnosis.level_unbalance_or_misalign_con,13);
            electricBar(fi.fault_electric_details.diagnosis.level_bearing_con,15);
            electricBar(fi.fault_electric_details.customparams.level_looseness,14);

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
                            fault_percents_v[fault_map_v[fault_code_1]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(0,176,80) }");
                            break;
                        case 2:
                            fault_percents_v[fault_map_v[fault_code_1]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(255,215,0) }");
                            break;
                        case 3:
                            fault_percents_v[fault_map_v[fault_code_1]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(112,48,160) }");
                            break;
                        case 4:
                            fault_percents_v[fault_map_v[fault_code_1]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(255,0,0) }");
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

void FaultWidget::electricBar(int type, int level)
{
    switch (type) {
    case 6:
        fault_percents_e[fault_map_e[level]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(114,244,174) }");
        break;
    case 5:
        fault_percents_e[fault_map_e[level]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(0,176,80) }");
        break;
    case 4:
        fault_percents_e[fault_map_e[level]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(0,176,240) }");
        break;
    case 3:
        fault_percents_e[fault_map_e[level]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(255,215,0) }");
        break;
    case 2:
        fault_percents_e[fault_map_e[level]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(112,48,160) }");
        break;
    case 1:
        fault_percents_e[fault_map_e[level]]->setStyleSheet("QProgressBar::chunk { background-color: rgb(255,0,0) }");
        break;
    default:
        break;
    }
}
