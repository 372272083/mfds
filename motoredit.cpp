#include "motoredit.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>
#include <QCheckBox>
#include <QDateTime>

#include "sqlitedb.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QMessageBox>
#include <QQueue>

#include "globalvariable.h"
#include "motorinfo.h"
#include "motorconditiontable.h"

MotorEdit::MotorEdit(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Motor Edit"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(400,300);
    this->setMaximumSize(400,300);

    this->setWindowIcon(QIcon(":/images/icon"));

    int index = 0;
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

    index = 4;
    QLabel *pRotateMeasureLabel = new QLabel(this);
    pRotateMeasureLabel->setText(tr("Rotate Measure Device:"));
    rotateDeviceCombo = new QComboBox();

    QVector<DeviceInfo*>::ConstIterator it;
    for(it=GlobalVariable::deviceInfos.constBegin();it!=GlobalVariable::deviceInfos.constEnd();it++)
    {
        DeviceInfo* di = *it;
        if(di->deviceType.compare("V",Qt::CaseInsensitive) == 0 || di->deviceType.compare("E",Qt::CaseInsensitive) == 0)
        {
            rotateDeviceCombo->addItem(di->title,di->deviceCode+"-"+di->deviceType);
        }
    }

    items->addWidget(pRotateMeasureLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(rotateDeviceCombo,index,1,1,1);

    index++;
    QLabel *pismanual = new QLabel(this);
    pismanual->setText(tr("Is manual rotate:"));
    isManualCB = new QCheckBox();

    items->addWidget(pismanual,index,0,1,1,Qt::AlignRight);
    items->addWidget(isManualCB,index,1,1,1);
    isManualCB->setChecked(false);
    connect(isManualCB,SIGNAL(stateChanged(int)),this,SLOT(stateChangedSlot(int)));

    index++;
    QLabel *pmanualLabel = new QLabel(this);
    pmanualLabel->setText(tr("Manual rotate:"));
    manualRotateEdit = new QLineEdit();
    manualRotateEdit->setEnabled(false);

    items->addWidget(pmanualLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(manualRotateEdit,index,1,1,1);

    items->setMargin(10);
    items->setVerticalSpacing(5);
    items->setAlignment(Qt::AlignCenter);

    playout->addLayout(items);
    playout->setContentsMargins(10,10,10,10);

    delete beartypemodel;
    delete motortypemodel;

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

    playout->addLayout(btn);
    this->setLayout(playout);
}

void MotorEdit::setmode(int value)
{
    mode = value;

    if (mode >= 0) //update bear
    {
        pcodeEdit->setEnabled(false);

        MotorInfo* mi = nullptr;
        QVector<MotorInfo*>::ConstIterator cIt;
        cIt = GlobalVariable::motorInfos.constBegin();
        for(;cIt != GlobalVariable::motorInfos.constEnd(); cIt++)
        {
            MotorInfo* item = *cIt;
            if(mode == item->tree_id)
            {
                pnameEdit->setText(item->title);
                pcodeEdit->setText(item->code);

                int dlens = typeCombo->count();
                for(int i=0;i<dlens;i++)
                {
                    QString udata = typeCombo->itemText(i);
                    if(item->motortype.compare(udata,Qt::CaseInsensitive) == 0)
                    {
                        typeCombo->setCurrentIndex(i);
                        break;
                    }
                }

                dlens = beartypeCombo->count();
                for(int i=0;i<dlens;i++)
                {
                    QString udata = beartypeCombo->itemText(i);
                    if(item->beartype.compare(udata,Qt::CaseInsensitive) == 0)
                    {
                        beartypeCombo->setCurrentIndex(i);
                        break;
                    }
                }

                typeCombo->setEnabled(false);
                beartypeCombo->setEnabled(false);

                QString mconditin = m_db->getMotorCondition(item->code);
                if(mconditin.length()>0)
                {
                    QStringList v_items = mconditin.split(',');
                    if(v_items.size() == 5)
                    {
                        bool tok;
                        int isM = 0;
                        QString tmp = v_items[3];
                        tmp.toInt(&tok);
                        if(tok)
                        {
                            if(0 == tmp) //from device
                            {
                                isManualCB->setChecked(false);
                                QString edevice = v_items[0];
                                QString vdevice = v_items[1];

                                QString deviceCode = "";
                                if(edevice.length()>0)
                                {
                                    deviceCode = edevice;
                                }
                                else if(vdevice.length()>0)
                                {
                                    deviceCode = vdevice;
                                }

                                dlens = rotateDeviceCombo->count();
                                for(int i=0;i<dlens;i++)
                                {
                                    QString udata = rotateDeviceCombo->itemData(rotateDeviceCombo->currentIndex()).toString();
                                    QStringList u_items = udata.split('-');
                                    if(deviceCode.compare(u_items[0],Qt::CaseInsensitive) == 0)
                                    {
                                        rotateDeviceCombo->setCurrentIndex(i);
                                        break;
                                    }
                                }
                            }
                            else //manual enter
                            {
                                isManualCB->setChecked(true);
                                QString rotate_r = QString::number(GlobalVariable::motorCondition[item->code].rotate,10,4);
                                manualRotateEdit->setText(rotate_r);
                            }
                        }
                    }
                }

                break;
            }
        }

        this->setWindowTitle(tr("Motor Edit"));
    }
    else
    {
        this->setWindowTitle(tr("Motor Add"));
    }
}

bool MotorEdit::editmotor()
{
    QString sql;

    QString modelstr = pcodeEdit->text().trimmed();
    QString name = pnameEdit->text().trimmed();
    QString motortype = typeCombo->currentText();
    QString beartype = beartypeCombo->currentText();

    if (modelstr.length()==0 || name.length()==0 || typeCombo->currentIndex() < 0 || beartypeCombo->currentIndex() < 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data 1!"));
        return false;
    }

    bool tok;
    QString rotate_s = "";
    if(isManualCB->isChecked())
    {
        rotate_s = manualRotateEdit->text().trimmed();
        if(rotate_s.length() == 0)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter data 2!"));
            return false;
        }
        float rotate_f;
        rotate_f = rotate_s.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter data 3!"));
            manualRotateEdit->setFocus();
            return false;
        }
        MotorConditionTable mct = GlobalVariable::motorCondition[modelstr];
        mct.rotate = rotate_f;

        GlobalVariable::motorCondition[modelstr] = mct;
    }
    //update condition
    sql = "insert into motorcondition(edevice,vdevice,rotate,isManual,rksj,mcode)";
    if(isManualCB->isChecked())
    {
        sql += " values('',''," + rotate_s + ",1,'" + QDateTime::currentDateTime().toString(GlobalVariable::dtFormat) + "','" + modelstr + "')";
    }
    else
    {
        QString udata = rotateDeviceCombo->itemData(rotateDeviceCombo->currentIndex()).toString();
        QStringList udata_items = udata.split('-');
        QString type_s = udata_items[1];
        if(type_s.compare("V",Qt::CaseInsensitive) == 0)
        {
            sql += " values('','" + udata_items[0] + "',0,0,'" + QDateTime::currentDateTime().toString(GlobalVariable::dtFormat) + "','" + modelstr + "')";
        }
        else
        {
            sql += " values('" + udata_items[0] + "','',0,0,'" + QDateTime::currentDateTime().toString(GlobalVariable::dtFormat) + "','" + modelstr + "')";
        }
    }
    if(!m_db->updatasql(sql))
    {
        QMessageBox::information(this, tr("Infomation"), tr("Update data failed 4!"));
        return false;
    }
    if (0 > mode)
    {
        sql = "select * from motor where mcode='" + modelstr + "'";
        if(m_db->queryCount(sql) > 0)
        {
            QMessageBox::information(this, tr("Infomation"), tr("The device code has existed!"));
            return false;
        }
        else
        {
            GlobalVariable::maxMotorTreeId++;
            sql = "insert into motortree (id,type,title,pid) values (" + QString::number(GlobalVariable::maxMotorTreeId) + ",1,'" + name + "',0)";
            QString sql_motor = "insert into motor (mcode,name,motor_type,bearing_type,manufacture_date,treeid) values ('" + modelstr + "','" + name + "','" + motortype + "','" + beartype + "',' '," + QString::number(GlobalVariable::maxMotorTreeId) + ")";
            qDebug()<<sql_motor;

            QQueue<QString> sqls;
            sqls.enqueue(sql);
            sqls.enqueue(sql_motor);

            sql = "select * from motortype where model='" + motortype + "'";
            QVector<QString> mts = m_db->execSql(sql);
            QString power_vol,power_cur,rotate_m;
            power_vol = "380";
            power_cur = "5";
            rotate_m = "3000";
            if(mts.size()>0)
            {
                QVector<QString>::ConstIterator in_it;
                for(in_it=mts.constBegin();in_it!=mts.constEnd();in_it++)
                {
                    QString m_items = *in_it;
                    QStringList ms = m_items.split(',');
                    if(ms.size()>=11)
                    {
                        power_vol = ms[4];
                        power_cur = ms[5];
                        rotate_m = ms[10];
                    }
                }
            }

            sql = "select * from bearingtype where model='" + beartype + "'";
            QVector<QString> bts = m_db->execSql(sql);

            int rotate_i;
            rotate_i = rotate_m.toInt(&tok);
            for(int i=1;i<=5;i++)
            {
                int r = rotate_i * (i * 0.25);
                sql = "insert into motorconditiontable(u,i,rotate,rksj,mcode) values(" + power_vol + "," + power_cur + "," + QString::number(r) + ",'" + QDateTime::currentDateTime().toString(GlobalVariable::dtFormat) + "','" + modelstr + "')";
                sqls.enqueue(sql);
            }

            if(m_db->execSql(sqls))
            {
                /*
                mi->btype = motor_items[16];
                tmp = motor_items[17];
                mi->rin = tmp.toFloat(&tok);
                tmp = motor_items[18];
                mi->rout = tmp.toFloat(&tok);
                tmp = motor_items[19];
                mi->contact_angle = tmp.toFloat(&tok);

                tmp = motor_items[20];
                mi->pitch = tmp.toFloat(&tok);
                tmp = motor_items[21];
                mi->roller_d = tmp.toFloat(&tok);
                tmp = motor_items[22];
                mi->roller_n = tmp.toFloat(&tok);
                */
                MotorInfo* di = new MotorInfo();
                di->type = MOTOR;
                di->title = name;
                di->name = name;
                di->beartype = beartype;
                di->motortype = motortype;
                di->code = modelstr;
                di->tree_id = GlobalVariable::maxMotorTreeId;

                if(mts.size()>0)
                {
                    QVector<QString>::ConstIterator in_it;
                    for(in_it=mts.constBegin();in_it!=mts.constEnd();in_it++)
                    {
                        QString m_items = *in_it;
                        QStringList ms = m_items.split(',');
                        if(ms.size()>=11)
                        {
                            di->work_mode = ms[2];
                            QString tmp = ms[3];
                            di->power = tmp.toFloat(&tok);
                            tmp = ms[4];
                            di->voltage = tmp.toFloat(&tok);
                            tmp = ms[5];
                            di->current = tmp.toFloat(&tok);
                            tmp = ms[8];
                            di->factor = tmp.toFloat(&tok);
                            tmp = ms[9];
                            di->insulate = tmp;
                            tmp = ms[10];
                            di->rotate = tmp.toFloat(&tok);
                        }
                    }
                }

                if(bts.size()>0)
                {
                    QVector<QString>::ConstIterator in_it;
                    for(in_it=bts.constBegin();in_it!=bts.constEnd();in_it++)
                    {
                        QString m_items = *in_it;
                        QStringList ms = m_items.split(',');
                        if(ms.size()>=9)
                        {
                            di->btype = ms[2];
                            QString tmp = ms[3];
                            di->rin = tmp.toFloat(&tok);
                            tmp = ms[4];
                            di->rout = tmp.toFloat(&tok);
                            tmp = ms[5];
                            di->contact_angle = tmp.toFloat(&tok);
                            tmp = ms[6];
                            di->pitch = tmp.toFloat(&tok);
                            tmp = ms[7];
                            di->roller_d = tmp.toFloat(&tok);;
                            tmp = ms[8];
                            di->roller_n = tmp.toFloat(&tok);
                        }
                    }
                }

                GlobalVariable::motorInfos.append(di);

                TreeNodeInfo *tni = new TreeNodeInfo();
                tni->id = GlobalVariable::maxMotorTreeId;
                tni->title = name;
                tni->type = MOTOR;
                tni->pid = 0;

                GlobalVariable::motorTreeInfos.append(tni);

                emit motorEditStateChange(GlobalVariable::maxMotorTreeId,name);
            }
            else
            {
                QMessageBox::information(this, tr("Infomation"), tr("Insert data failed 5!"));
                return false;
            }
        }
    }
    else
    {
        QVector<TreeNodeInfo*>::ConstIterator tcIt;

        QVector<MotorInfo*>::ConstIterator ccIt;
        ccIt = GlobalVariable::motorInfos.constBegin();
        for(;ccIt != GlobalVariable::motorInfos.constEnd();ccIt++)
        {
            MotorInfo* ddi = *ccIt;
            if(ddi->code.compare(modelstr,Qt::CaseInsensitive) == 0)
            {
                ddi->title = name;
                for(tcIt=GlobalVariable::motorTreeInfos.constBegin();tcIt != GlobalVariable::motorTreeInfos.constEnd();tcIt++)
                {
                    TreeNodeInfo *tni = *tcIt;
                    if(tni->id == ddi->tree_id)
                    {
                        tni->title = name;
                        break;
                    }
                }
                ddi->name = name;
                sql = "update motortree set title='" + name + "' where id=" + QString::number(ddi->tree_id);
                QString sql_motor = "update motor set name='" + name + "' where treeid=" + QString::number(ddi->tree_id);
                QQueue<QString> sqls;
                sqls.enqueue(sql);
                sqls.enqueue(sql_motor);
                if(m_db->execSql(sqls))
                {
                    emit motorEditStateChange(ddi->tree_id,name);
                }
                else
                {
                    QMessageBox::information(this, tr("Infomation"), tr("Update data failed!"));
                }
                break;
            }
        }
    }

    return true;
}

void MotorEdit::stateChangedSlot(int state)
{
    if(state == Qt::Checked)
    {
        manualRotateEdit->setEnabled(true);
        QString modelstr = pcodeEdit->text().trimmed();
        if(GlobalVariable::motorCondition.contains(modelstr))
        {
            manualRotateEdit->setText(QString::number(GlobalVariable::motorCondition[modelstr].rotate,10,1));
        }
        rotateDeviceCombo->setEnabled(false);
    }
    else
    {
        manualRotateEdit->setEnabled(false);
    }
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
