#include "motorinfowidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "sqlitedb.h"

#include <QSqlTableModel>
#include <QSqlRecord>

#include "motorinfo.h"

MotorInfoWidget::MotorInfoWidget(SqliteDB *db, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QSqlTableModel *motormodel = db->modelNoHeader("motor");

    QSqlTableModel *motortypemodel = db->modelNoHeader("motortype");

    QSqlTableModel *beartypemodel = db->modelNoHeader("bearingtype");

    int rows = motormodel->rowCount();
    QString motortype_l;
    QString beartype_l;
    QString filter;
    for(int i=0;i<rows;i++)
    {
        QSqlRecord record = motormodel->record(i);
        MotorInfo *mi = new MotorInfo();

        mi->name = record.value("name").toString();
        mi->code = record.value("mcode").toString();
        motortype_l = record.value("motor_type").toString();
        beartype_l = record.value("bearing_type").toString();

        filter = "model='" + motortype_l + "'";
        motortypemodel->setFilter(filter);
        if (motortypemodel->select()) {
            if (motortypemodel->rowCount() > 0)
            {
                QSqlRecord typeRecord = motortypemodel->record(0);
                mi->work_mode = typeRecord.value("work_mode").toString();
                mi->power = typeRecord.value("power_rating").toString();
                mi->voltage = typeRecord.value("rated_voltage").toString();
                mi->factor = typeRecord.value("factor").toString();
                mi->insulate = typeRecord.value("insulate").toString();
                mi->rotate = typeRecord.value("rotate").toString();
            }
        }

        filter = "model='" + beartype_l + "'";
        beartypemodel->setFilter(filter);
        if (beartypemodel->select()) {
            if (beartypemodel->rowCount() > 0)
            {
                QSqlRecord typeRecord = beartypemodel->record(0);
                mi->beartype = typeRecord.value("btype").toString();
            }
        }

        motors.append(mi);
    }
    //QVBoxLayout *playout = new QVBoxLayout();
    //QGridLayout *playout = new QGridLayout;
    //playout->setSizeConstraint(QLayout::SetDefaultConstraint);
    int index = 0;
    QGridLayout *playout = new QGridLayout();
    playout->setVerticalSpacing(5);
    QLabel *pmotorLabel = new QLabel();
    pmotorLabel->setText(tr("motor:"));
    motorCombo = new QComboBox();
    rows = motors.count();
    motorCombo->addItem(tr("please select..."), tr("0000"));
    for(int i=0;i<rows;i++)
    {
        MotorInfo *mi = motors.at(i);
        motorCombo->addItem(mi->name, mi->code);
    }

    connect(motorCombo,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(motorChangedCombo(const QString &)));

    playout->addWidget(pmotorLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(motorCombo,index,1,1,1,Qt::AlignTop);
    playout->setColumnStretch(0, 1);
    playout->setColumnStretch(1, 1);

    index++;
    QLabel *pnameLabel = new QLabel();
    pnameLabel->setText(tr("motor name:"));
    nameEdit = new QLabel();
    //nameEdit->setEnabled(false);

    playout->addWidget(pnameLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(nameEdit,index,1,1,1,Qt::AlignTop);

    index++;
    QLabel *pcodeLabel = new QLabel();
    pcodeLabel->setText(tr("motor code:"));
    codeEdit = new QLabel();
    //codeEdit->setEnabled(false);

    playout->addWidget(pcodeLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(codeEdit,index,1,1,1,Qt::AlignTop);

    index++;
    QLabel *pworkmLabel = new QLabel();
    pworkmLabel->setText(tr("work mode:"));
    workMEdit = new QLabel();
    //workMEdit->setEnabled(false);

    playout->addWidget(pworkmLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(workMEdit,index,1,1,1,Qt::AlignTop);

    index++;
    QLabel *ppowerLabel = new QLabel();
    ppowerLabel->setText(tr("power:"));
    powerEdit = new QLabel();
    //powerEdit->setEnabled(false);

    playout->addWidget(ppowerLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(powerEdit,index,1,1,1,Qt::AlignTop);

    index++;
    QLabel *pvoltageLabel = new QLabel();
    pvoltageLabel->setText(tr("voltage:"));
    voltageEdit = new QLabel();
    //voltageEdit->setEnabled(false);

    playout->addWidget(pvoltageLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(voltageEdit,index,1,1,1,Qt::AlignTop);

    index++;
    QLabel *pfactorLabel = new QLabel();
    pfactorLabel->setText(tr("factor:"));
    factorEdit = new QLabel();
    //factorEdit->setEnabled(false);

    playout->addWidget(pfactorLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(factorEdit,index,1,1,1,Qt::AlignTop);

    index++;
    QLabel *pinsulateLabel = new QLabel();
    pinsulateLabel->setText(tr("insulate:"));
    insulateEdit = new QLabel();
    //insulateEdit->setEnabled(false);

    playout->addWidget(pinsulateLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(insulateEdit,index,1,1,1,Qt::AlignTop);

    index++;
    QLabel *protateLabel = new QLabel();
    protateLabel->setText(tr("rotate:"));
    rotateEdit = new QLabel();
    //rotateEdit->setEnabled(false);

    playout->addWidget(protateLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(rotateEdit,index,1,1,1,Qt::AlignTop);

    index++;
    QLabel *pbeartypeLabel = new QLabel();
    pbeartypeLabel->setText(tr("bear type:"));
    beartypeEdit = new QLabel();
    //beartypeEdit->setEnabled(false);

    playout->addWidget(pbeartypeLabel,index,0,1,1,Qt::AlignTop | Qt::AlignRight);
    playout->addWidget(beartypeEdit,index,1,1,1,Qt::AlignTop);

    mainLayout->addLayout(playout);

    mainLayout->addStretch();

    QGridLayout *rowDeviceType = new QGridLayout();
    QLabel *ptypeLabel = new QLabel();
    ptypeLabel->setText(tr("device type:"));
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem(tr("E"));
    typeCombo->addItem(tr("V"));
    typeCombo->addItem(tr("T"));

    connect(typeCombo,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(deviceTypeChangedCombo(const QString &)));

    rowDeviceType->addWidget(ptypeLabel,0,0,1,1,Qt::AlignRight);
    rowDeviceType->addWidget(typeCombo,0,1,1,1,Qt::AlignLeft);
    rowDeviceType->setColumnStretch(0, 1);
    rowDeviceType->setColumnStretch(1, 1);

    mainLayout->addLayout(rowDeviceType);

    this->setLayout(mainLayout);
}

void MotorInfoWidget::motorChangedCombo(const QString &text)
{
    QString code = motorCombo->itemData(motorCombo->currentIndex()).toString();

    int rows = motors.count();
    bool tok;
    bool flag = false;
    for(int i=0;i<rows;i++)
    {
        MotorInfo *mi = motors.at(i);
        if (mi->code == code)
        {
            nameEdit->setText(mi->name);
            codeEdit->setText(mi->code);
            workMEdit->setText(mi->work_mode);
            powerEdit->setText(mi->power + " (kw)");
            voltageEdit->setText(mi->voltage + " (v)");
            factorEdit->setText(QString("%4").arg(mi->factor.toFloat(&tok)));
            insulateEdit->setText(mi->insulate + " " + tr("class"));
            rotateEdit->setText(mi->rotate + " (r/min)");
            beartypeEdit->setText(mi->beartype);

            emit motorChanged(code);
            flag = true;
            break;
        }
    }
    if (!flag)
    {
        nameEdit->setText("");
        codeEdit->setText("");
        workMEdit->setText("");
        powerEdit->setText("");
        voltageEdit->setText("");
        factorEdit->setText("");
        insulateEdit->setText("");
        rotateEdit->setText("");
        beartypeEdit->setText("");
    }
}

void MotorInfoWidget::deviceTypeChangedCombo(const QString &text)
{
    emit deviceTypeChanged(text);
}

MotorInfoWidget::~MotorInfoWidget()
{
    int rows = motors.count();
    bool tok;
    bool flag = false;
    for(int i=0;i<rows;i++)
    {
        MotorInfo *mi = motors.at(i);
    }
}
