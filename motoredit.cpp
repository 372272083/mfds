#include "motoredit.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>

#include "sqlitedb.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QMessageBox>

MotorEdit::MotorEdit(QSqlTableModel *model,SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db),motormodel(model)
{
    this->setWindowTitle(tr("Motor Edit"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(250,220);
    this->setMaximumSize(250,220);

    this->setWindowIcon(QIcon(":/images/icon"));

    QVBoxLayout *playout = new QVBoxLayout;

    QGridLayout *items = new QGridLayout;
    QLabel *pcodeLabel = new QLabel(this);
    pcodeLabel->setText(tr("code:"));
    pcodeEdit = new QLineEdit(this);

    items->addWidget(pcodeLabel,0,0,1,1,Qt::AlignRight);
    items->addWidget(pcodeEdit,0,1,1,1);

    QLabel *pnameLabel = new QLabel(this);
    pnameLabel->setText(tr("name:"));
    pnameEdit = new QLineEdit(this);

    items->addWidget(pnameLabel,1,0,1,1,Qt::AlignRight);
    items->addWidget(pnameEdit,1,1,1,1);

    QList<QString> motortypelist;
    motortypelist << tr("model") << tr("work mode") << tr("power(KW)") << tr("voltage(V)") << tr("current(A)") << tr("poles") << tr("centerh(mm)") << tr("factor") << tr("insulate") << tr("rotate(r/min)");
    motortypelist << tr("contact angles");
    QSqlTableModel *motortypemodel = m_db->model("motortype",motortypelist);

    QList<QString> beartypelist;
    beartypelist << tr("model") << tr("bear type") << tr("rin(mm)") << tr("rout(mm)") << tr("contact angles(°c)");
    QSqlTableModel *beartypemodel = m_db->model("bearingtype",beartypelist);

    QLabel *ptypeLabel = new QLabel(this);
    ptypeLabel->setText(tr("motor type:"));
    typeCombo = new QComboBox();

    int tmpRow = motortypemodel->rowCount();

    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = motortypemodel->record(i);
        motortypeclass<<record.value("model").toString();
    }
    typeCombo->addItems(motortypeclass);

    items->addWidget(ptypeLabel,2,0,1,1,Qt::AlignRight);
    items->addWidget(typeCombo,2,1,1,1);

    QLabel *pbearLabel = new QLabel(this);
    pbearLabel->setText(tr("bear type:"));
    beartypeCombo = new QComboBox();

    tmpRow = beartypemodel->rowCount();
    qDebug() << "bear type number: " << tmpRow;
    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = beartypemodel->record(i);
        beartypeclass<<record.value("model").toString();
    }
    beartypeCombo->addItems(beartypeclass);

    items->addWidget(pbearLabel,3,0,1,1,Qt::AlignRight);
    items->addWidget(beartypeCombo,3,1,1,1);

    items->setMargin(20);

    playout->addLayout(items);
    playout->setContentsMargins(10,10,10,10);

    delete beartypemodel;
    delete motortypemodel;

    QHBoxLayout *btn = new QHBoxLayout;
    btn->addStretch();
    papplybtn = new QPushButton(this);
    connect(papplybtn, SIGNAL(clicked()),this, SLOT(apply()));
    papplybtn->setText(tr("apply"));
    btn->addWidget(papplybtn);

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

void MotorEdit::setmode(int value)
{
    mode = value;

    if (mode >= 0) //update bear
    {
        pcodeEdit->setEnabled(false);
        papplybtn->setEnabled(false);
        this->setWindowTitle(tr("Motor Edit"));
    }
    else
    {
        this->setWindowTitle(tr("Motor Add"));
    }

}

void MotorEdit::setmodel(QList<QString> values)
{
    QString mcodestr_l;
    QString name_l;
    QString motortype_l;
    QString beartype_l;

    mcodestr_l = values.at(0);
    name_l = values.at(1);
    motortype_l = values.at(2);
    beartype_l = values.at(3);

    pcodeEdit->setText(mcodestr_l);
    pnameEdit->setText(name_l);

    QList<QString>::Iterator it = motortypeclass.begin(),itend = motortypeclass.end();
    int i = 0;
    for (;it != itend; it++,i++){
        if ((*it).compare(motortype_l) == 0){//找到，高亮显示
            typeCombo->setCurrentIndex(i);
            break;
        }
    }

    it = beartypeclass.begin(),itend = beartypeclass.end();
    i = 0;
    for (;it != itend; it++,i++){
        if ((*it).compare(beartype_l) == 0){//找到，高亮显示
            beartypeCombo->setCurrentIndex(i);
            break;
        }
    }
}

bool MotorEdit::editmotor()
{
    QString modelstr = pcodeEdit->text().trimmed();
    QString name = pnameEdit->text().trimmed();
    QString motortype = typeCombo->currentText();
    QString beartype = beartypeCombo->currentText();

    if (modelstr.length()==0 || name.length()==0 || typeCombo->currentIndex() < 0 || beartypeCombo->currentIndex() < 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return false;
    }

    if (0 > mode)
    {
        QString filter = "mcode='" + modelstr + "'";

        motormodel->setFilter(filter);
        if (motormodel->select()) {
            if (motormodel->rowCount() > 0)
            {
                QMessageBox::information(this, tr("Infomation"), tr("Model has Existed!"));
                motormodel->setFilter("id>0");
                motormodel->select();
                pcodeEdit->setFocus();
                return false;
            }
            motormodel->setFilter("id>0");
            motormodel->select();
        }

        int row=0;
        motormodel->insertRow(row);
        motormodel->setData(motormodel->index(row,1),modelstr);
        motormodel->setData(motormodel->index(row,2),name);
        motormodel->setData(motormodel->index(row,3),motortype);
        motormodel->setData(motormodel->index(row,4),beartype);
        motormodel->submitAll();
    }
    else
    {
        QString filter = "mcode='" + modelstr + "'";
        motormodel->setFilter(filter);
        if (motormodel->select()) {
            if (motormodel->rowCount() == 1)
            {
                QSqlRecord record = motormodel->record(0);
                record.setValue("mcode", modelstr);
                record.setValue("name", name);
                record.setValue("motor_type", motortype);
                record.setValue("bearing_type", beartype);
                motormodel->setRecord(0, record);
                motormodel->submitAll();
            }
            else
            {
                return false;
            }
            motormodel->setFilter("id>0");
            motormodel->select();
        }
    }

    return true;
}

void MotorEdit::apply()
{
    this->editmotor();
}

void MotorEdit::ok()
{
    if (this->editmotor())
    {
        this->close();
    }
}

void MotorEdit::cancel()
{
    this->close();
}

MotorEdit::~MotorEdit()
{

}
