#include "devicepipeedit.h"

#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlRecord>
#include <QComboBox>
#include <QQueue>
#include <QDebug>
#include <QTabWidget>

#include "sqlitedb.h"
#include "globalvariable.h"
#include "motorinfo.h"

DevicePipeEdit::DevicePipeEdit(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Device Pipe Edit"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(350,300);
    this->setMaximumSize(350,300);

    this->setWindowIcon(QIcon(":/images/icon"));

    QTabWidget *mainWidget = new QTabWidget(this);

    QVBoxLayout *playout = new QVBoxLayout;

    QDialog *board = new QDialog(this);
    QGridLayout *items = new QGridLayout(board);
    items->setVerticalSpacing(5);
    items->setAlignment(Qt::AlignCenter);

    QLabel *pnameLabel = new QLabel(this);
    pnameLabel->setText(tr("Channel name:"));
    pipeNameEdit = new QLineEdit(this);
    pipeNameEdit->setPlaceholderText(tr("please enter pipe name"));

    items->addWidget(pnameLabel,0,0,1,1,Qt::AlignRight);
    items->addWidget(pipeNameEdit,0,1,1,1);

    //
    QLabel *pdeviceLabel = new QLabel(this);
    pdeviceLabel->setText(tr("Device:"));
    deviceCombo = new QComboBox(this);

    QSqlTableModel *devicemodel = m_db->modelNoHeader("device");
    int tmpRow = devicemodel->rowCount();

    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = devicemodel->record(i);
        QString code = record.value("code").toString();
        QString name = record.value("name").toString();
        QString dmodel = record.value("dmodel").toString();
        QString type = record.value("dtype").toString();

        deviceCombo->addItem(name,code+"," + dmodel + "," + type);
    }

    connect(deviceCombo,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(deviceSelectChanged(const QString &)));

    delete devicemodel;

    items->addWidget(pdeviceLabel,1,0,1,1,Qt::AlignRight);
    items->addWidget(deviceCombo,1,1,1,1);

    pchannelLabel = new QLabel(this);
    pchannelLabel->setText(tr("Channel:"));
    channelCombo = new QComboBox(this);

    /*
    if(1==tmpRow)
    {
        this->deviceSelectChanged("");
    }
    */

    items->addWidget(pchannelLabel,2,0,1,1,Qt::AlignRight);
    items->addWidget(channelCombo,2,1,1,1);

    mainWidget->addTab(board,tr("basic information"));

    ///////////////////////////////////////////////////////////
    /// \brief index
    if(GlobalVariable::version >=2)
    {
        int index = 0;
        QDialog *boarddiagnose = new QDialog(this);
        QGridLayout *diagnoseItems = new QGridLayout(boarddiagnose);
        diagnoseItems->setMargin(5);
        diagnoseItems->setVerticalSpacing(5);
        diagnoseItems->setAlignment(Qt::AlignCenter);

        paccampLabel = new QLabel(this);
        paccampLabel->setText(tr("Acc AMP Threshold:"));
        paccMetricLabel = new QLabel(this);
        paccMetricLabel->setText(tr("(m/ss)"));
        acc_ampThresholdEdit = new QLineEdit(this);

        //index++;
        diagnoseItems->addWidget(paccampLabel,index,0,1,1,Qt::AlignRight);
        diagnoseItems->addWidget(acc_ampThresholdEdit,index,1,1,1);
        diagnoseItems->addWidget(paccMetricLabel,index,2,1,1);

        paccrmsLabel = new QLabel(this);
        paccrmsLabel->setText(tr("Acc RMS Threshold:"));
        paccrmsMetricLabel = new QLabel(this);
        paccrmsMetricLabel->setText(tr("(m/ss)"));
        acc_rmsThresholdEdit = new QLineEdit(this);

        index++;
        diagnoseItems->addWidget(paccrmsLabel,index,0,1,1,Qt::AlignRight);
        diagnoseItems->addWidget(acc_rmsThresholdEdit,index,1,1,1);
        diagnoseItems->addWidget(paccrmsMetricLabel,index,2,1,1);

        paccpkpkLabel = new QLabel(this);
        paccpkpkLabel->setText(tr("Acc PKPK Threshold:"));
        paccpkpkMetricLabel = new QLabel(this);
        paccpkpkMetricLabel->setText(tr("(m/ss)"));
        acc_pkpkThresholdEdit = new QLineEdit(this);

        index++;
        diagnoseItems->addWidget(paccpkpkLabel,index,0,1,1,Qt::AlignRight);
        diagnoseItems->addWidget(acc_pkpkThresholdEdit,index,1,1,1);
        diagnoseItems->addWidget(paccpkpkMetricLabel,index,2,1,1);

        pspdampLabel = new QLabel(this);
        pspdampLabel->setText(tr("Speed AMP Threshold:"));
        pspdMetricLabel = new QLabel(this);
        pspdMetricLabel->setText(tr("(mm/s)"));
        spd_ampThresholdEdit = new QLineEdit(this);

        index++;
        diagnoseItems->addWidget(pspdampLabel,index,0,1,1,Qt::AlignRight);
        diagnoseItems->addWidget(spd_ampThresholdEdit,index,1,1,1);
        diagnoseItems->addWidget(pspdMetricLabel,index,2,1,1);

        pspdrmsLabel = new QLabel(this);
        pspdrmsLabel->setText(tr("Speed RMS Threshold:"));
        pspdrmsMetricLabel = new QLabel(this);
        pspdrmsMetricLabel->setText(tr("(mm/s)"));
        spd_rmsThresholdEdit = new QLineEdit(this);

        index++;
        diagnoseItems->addWidget(pspdrmsLabel,index,0,1,1,Qt::AlignRight);
        diagnoseItems->addWidget(spd_rmsThresholdEdit,index,1,1,1);
        diagnoseItems->addWidget(pspdrmsMetricLabel,index,2,1,1);

        pspdpkpkLabel = new QLabel(this);
        pspdpkpkLabel->setText(tr("Speed PKPK Threshold:"));
        pspdpkpkMetricLabel = new QLabel(this);
        pspdpkpkMetricLabel->setText(tr("(mm/s)"));
        spd_pkpkThresholdEdit = new QLineEdit(this);

        index++;
        diagnoseItems->addWidget(pspdpkpkLabel,index,0,1,1,Qt::AlignRight);
        diagnoseItems->addWidget(spd_pkpkThresholdEdit,index,1,1,1);
        diagnoseItems->addWidget(pspdpkpkMetricLabel,index,2,1,1);

        diagnose_tab_index = mainWidget->addTab(boarddiagnose,tr("diagnose setting"));
    }

    ///////////////////////////////////////////////////////////////////

    playout->addWidget(mainWidget);
    playout->setContentsMargins(10,10,10,10);

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

    if(tmpRow > 0)
    {
        this->deviceSelectChanged("");
    }

    if(GlobalVariable::version>=2)
    {
        this->paccampLabel->setVisible(false);
        this->paccMetricLabel->setVisible(false);
        this->acc_ampThresholdEdit->setVisible(false);
        this->paccpkpkLabel->setVisible(false);
        this->paccpkpkMetricLabel->setVisible(false);
        this->acc_pkpkThresholdEdit->setVisible(false);

        this->pspdampLabel->setVisible(false);
        this->pspdMetricLabel->setVisible(false);
        this->spd_ampThresholdEdit->setVisible(false);
        this->pspdpkpkLabel->setVisible(false);
        this->pspdpkpkMetricLabel->setVisible(false);
        this->spd_pkpkThresholdEdit->setVisible(false);
    }
}

void DevicePipeEdit::setmode(int value)
{
    mode = value;
    if (mode >= 0) //update bear
    {
        ChannelInfo* ci = nullptr;
        QVector<ChannelInfo*>::ConstIterator cIt;
        cIt = GlobalVariable::channelInfos.constBegin();
        for(;cIt != GlobalVariable::channelInfos.constEnd(); cIt++)
        {
            ChannelInfo* item = *cIt;
            if(mode == item->tree_id)
            {
                pipeNameEdit->setText(item->title);

                int dlens = deviceCombo->count();
                for(int i=0;i<dlens;i++)
                {
                    QString udata = deviceCombo->itemData(i).toString();
                    QString dcode = udata.split(',')[0];
                    if(dcode.compare(item->device_code,Qt::CaseInsensitive) == 0)
                    {
                        deviceCombo->setCurrentIndex(i);
                        break;
                    }
                }

                if(GlobalVariable::version>=2)
                {
                    acc_rmsThresholdEdit->setText(QString::number(item->rms1,10,2));
                    spd_rmsThresholdEdit->setText(QString::number(item->rms2,10,2));
                }

                dlens = channelCombo->count();
                for(int i=0;i<dlens;i++)
                {
                    QString udata = channelCombo->itemData(i).toString().trimmed();
                    if(udata.compare(item->channel_code,Qt::CaseInsensitive) == 0)
                    {
                        channelCombo->setCurrentIndex(i);
                        break;
                    }
                }
                deviceCombo->setEnabled(false);
                channelCombo->setEnabled(false);
                break;
            }
        }

        this->setWindowTitle(tr("Device Pipe Edit"));
    }
    else
    {
        this->setWindowTitle(tr("Device Pipe Edit"));
    }
}

bool DevicePipeEdit::editpipe()
{
    QString namestr = pipeNameEdit->text().trimmed();
    QString devicename = deviceCombo->currentText();
    QString channelCode = channelCombo->currentText();

    QString rms1 = "1.0";
    QString rms2 = "1.0";

    if(GlobalVariable::version >= 2)
    {
        rms1 = acc_rmsThresholdEdit->text().trimmed();
        rms2 = spd_rmsThresholdEdit->text().trimmed();
    }

    if (namestr.length() == 0 || deviceCombo->currentIndex() < 0 || channelCombo->currentIndex() < 0 || rms1.length() == 0 || rms2.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return false;
    }

    QString udata = deviceCombo->itemData(deviceCombo->currentIndex()).toString();
    QString dcode = udata.split(',')[0];

    channelCode = channelCombo->itemData(channelCombo->currentIndex()).toString();

    QString motorCode = "";
    QVector<MotorInfo*>::ConstIterator cIt;
    for(cIt=GlobalVariable::motorInfos.constBegin();cIt!=GlobalVariable::motorInfos.constEnd();cIt++)
    {
        MotorInfo* item = *cIt;
        if(item->tree_id == motor_id)
        {
            motorCode = item->code;
            break;
        }
    }

    bool tok;
    float rms1_f,rms2_f;

    rms1_f = rms1.toFloat(&tok);
    if(!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        acc_rmsThresholdEdit->setFocus();
        return false;
    }
    rms2_f = rms2.toFloat(&tok);
    if(!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        spd_rmsThresholdEdit->setFocus();
        return false;
    }

    if (0 > mode)
    {
        QVector<ChannelInfo*> channels = GlobalVariable::findChannelsByMotorId(motor_id);
        QVector<ChannelInfo*>::ConstIterator cciIt;
        bool flage = false;
        for(cciIt=channels.constBegin();cciIt!=channels.constEnd();cciIt++)
        {
            ChannelInfo* ci = *cciIt;
            if(ci->device_code.compare(dcode,Qt::CaseInsensitive) == 0 && ci->channel_code.compare(channelCode,Qt::CaseInsensitive) == 0)
            {
                flage = true;
                break;
            }
        }
        if(flage)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Same attributes channel has existed!"));
            return false;
        }

        QString dtype_in = GlobalVariable::findDeviceTypeByCode(dcode);
        if(dtype_in.compare("E",Qt::CaseInsensitive) == 0)
        {
            QString sql = "select * from devicepipes where dcode='" + dcode + "' and motor <> '" + motorCode + "'";
            if(m_db->queryCount(sql)>0)
            {
                QMessageBox::information(this, tr("Infomation"), tr("Electric Device only surport be connected to a motor!"));
                return false;
            }
        }

        GlobalVariable::maxMotorTreeId++;
        GlobalVariable::maxChannelId++;
        QString sql = "insert into motortree (id,type,title,pid) values (" + QString::number(GlobalVariable::maxMotorTreeId) + ",2,'" + namestr + "'," + QString::number(motor_id) + ")";
        QString sql_motor = "insert into devicepipes (id,dcode,pipeCode,name,motor,isactive,channel_type,treeid,rms1,rms2) values (" + QString::number(GlobalVariable::maxChannelId) + ",'" + dcode + "'," + channelCode + ",'" + namestr + "','" + motorCode + "',1,1," + QString::number(GlobalVariable::maxMotorTreeId) + "," + QString::number(rms1_f,10,2) + "," + QString::number(rms2_f,10,2) + ")";
        qDebug()<<sql_motor;
        QQueue<QString> sqls;
        sqls.enqueue(sql);
        sqls.enqueue(sql_motor);
        if(m_db->execSql(sqls))
        {
            ChannelInfo* ci = new ChannelInfo();
            ci->id = GlobalVariable::maxChannelId;
            ci->tree_id = GlobalVariable::maxMotorTreeId;
            ci->type = CHANNEL_PHYISIC;
            ci->device_code = dcode;
            ci->channel_code = channelCode;
            ci->motor_code = motorCode;
            ci->title = namestr;
            ci->rms1 = rms1_f;
            ci->rms2 = rms2_f;

            int c_type = 1;
            ci->channel_type = 1;
            if(dtype_in.compare("E",Qt::CaseInsensitive) == 0)
            {
                c_type = 21;
            }
            else if(dtype_in.compare("V",Qt::CaseInsensitive) == 0)
            {
                c_type = 1;
            }
            else if(dtype_in.compare("T",Qt::CaseInsensitive) == 0)
            {
                c_type = 31;
            }

            ci->isactive = 1;

            GlobalVariable::channelInfos.append(ci);

            TreeNodeInfo *tni = new TreeNodeInfo();
            tni->id = GlobalVariable::maxMotorTreeId;
            tni->title = namestr;
            tni->type = CHANNEL_PHYISIC;
            tni->pid = motor_id;

            GlobalVariable::motorTreeInfos.append(tni);

            QString dname = GlobalVariable::findDeviceByCode(dcode)->title;
            QString nodeTitle = namestr + QObject::tr("[%1]").arg(dname);

            emit channelEditStateChange(GlobalVariable::maxMotorTreeId,nodeTitle,motor_id,c_type);
        }
        else
        {
            QMessageBox::information(this, tr("Infomation"), tr("Insert data failed!"));
            return false;
        }
    }
    else
    {
        QVector<TreeNodeInfo*>::ConstIterator tcIt;

        QVector<ChannelInfo*>::ConstIterator ccIt;
        ccIt = GlobalVariable::channelInfos.constBegin();
        for(;ccIt != GlobalVariable::channelInfos.constEnd();ccIt++)
        {
            ChannelInfo* ddi = *ccIt;
            if(ddi->tree_id == mode)
            {
                ddi->title = namestr;
                for(tcIt=GlobalVariable::motorTreeInfos.constBegin();tcIt != GlobalVariable::motorTreeInfos.constEnd();tcIt++)
                {
                    TreeNodeInfo *tni = *tcIt;
                    if(tni->id == mode)
                    {
                        tni->title = namestr;
                        break;
                    }
                }
                ddi->rms1 = rms1_f;
                ddi->rms2 = rms2_f;
                QString sql = "update motortree set title='" + namestr + "' where id=" + QString::number(mode);
                QString sql_motor = "update devicepipes set name='" + namestr + "', rms1=" + QString::number(rms1_f,10,2) + ", rms2=" + QString::number(rms2_f,10,2) + " where treeid=" + QString::number(mode);
                QQueue<QString> sqls;
                sqls.enqueue(sql);
                sqls.enqueue(sql_motor);
                if(m_db->execSql(sqls))
                {
                    QString dname = GlobalVariable::findDeviceByCode(dcode)->title;
                    QString nodeTitle = namestr + QObject::tr("[%1]").arg(dname);

                    emit channelEditStateChange(mode,nodeTitle,ddi->pid,ddi->channel_type);
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

void DevicePipeEdit::deviceSelectChanged(const QString &text)
{
    channelCombo->clear();
    QString udata = deviceCombo->itemData(deviceCombo->currentIndex()).toString();
    QString dmodel = udata.split(',')[1];
    QString d_type = udata.split(',')[2];

    QSqlTableModel *devicemodel = m_db->modelNoHeader("devicetype");
    devicemodel->setFilter("model='"+dmodel+"'");
    int tmpRow = devicemodel->rowCount();
    bool tok;
    QString pipe_num = 0;
    if(tmpRow > 0)
    {
        QSqlRecord record = devicemodel->record(0);
        QString pipe_str = record.value("pipenum").toString();
        pipe_num = pipe_str.toInt(&tok);
    }
    if(pipe_num>0)
    {
        for(int i=0;i<pipe_num;i++)
        {
            channelCombo->addItem("Channel " + QString::number(i+1),i+1);
        }
    }
    delete devicemodel;

    channelCombo->setVisible(true);
    pchannelLabel->setVisible(true);
    if(d_type.compare("T",Qt::CaseInsensitive) == 0)
    {

    }
    else if(d_type.compare("E",Qt::CaseInsensitive) == 0)
    {
        channelCombo->setVisible(false);
        pchannelLabel->setVisible(false);
    }

    if(GlobalVariable::version >=2)
    {
        if(d_type.compare("E",Qt::CaseInsensitive) == 0)
        {
            paccampLabel->setText(tr("Voltage AMP Threshold:"));
            paccrmsLabel->setText(tr("Voltage RMS Threshold:"));
            paccpkpkLabel->setText(tr("Voltage PKPK Threshold:"));

            paccMetricLabel->setText(tr("(V)"));
            paccrmsMetricLabel->setText(tr("(V)"));
            paccpkpkMetricLabel->setText(tr("(V)"));

            pspdampLabel->setText(tr("Current AMP Threshold:"));
            pspdrmsLabel->setText(tr("Current RMS Threshold:"));
            pspdpkpkLabel->setText(tr("Current PKPK Threshold:"));

            pspdMetricLabel->setText(tr("(A)"));
            pspdrmsMetricLabel->setText(tr("(A)"));
            pspdpkpkMetricLabel->setText(tr("(A)"));

            int pid = 0;
            if(mode > 0)
            {
                pid = GlobalVariable::findMotorTreeParentId(mode);
            }
            else
            {
                pid = motor_id;
            }
            if(pid > 0)
            {
                MotorInfo *mi = GlobalVariable::findMotorByTreeId(pid);

                acc_rmsThresholdEdit->setText(QString::number(mi->voltage,10,2));
                spd_rmsThresholdEdit->setText(QString::number(mi->current,10,2));
            }
        }
        else if(d_type.compare("V",Qt::CaseInsensitive) == 0)
        {
            acc_rmsThresholdEdit->setText(QString::number(10.0,10,2));
            spd_rmsThresholdEdit->setText(QString::number(3.0,10,2));
        }
        else if(d_type.compare("T",Qt::CaseInsensitive) == 0)
        {
            paccampLabel->setText(tr("Voltage AMP Threshold:"));
            paccrmsLabel->setText(tr("Temperature upper limit:"));
            paccpkpkLabel->setText(tr("Voltage PKPK Threshold:"));

            paccMetricLabel->setText(tr("(V)"));
            paccrmsMetricLabel->setText(tr("(°C)"));
            paccpkpkMetricLabel->setText(tr("(V)"));

            pspdampLabel->setText(tr("Current AMP Threshold:"));
            pspdrmsLabel->setText(tr("Temperatur lower limit:"));
            pspdpkpkLabel->setText(tr("Current PKPK Threshold:"));

            pspdMetricLabel->setText(tr("(A)"));
            pspdrmsMetricLabel->setText(tr("(°C)"));
            pspdpkpkMetricLabel->setText(tr("(A)"));

            acc_rmsThresholdEdit->setText(QString::number(60.0,10,2));
            spd_rmsThresholdEdit->setText(QString::number(0.0,10,2));
        }
    }
}

void DevicePipeEdit::ok()
{
    if (this->editpipe())
    {
        this->close();
    }
}

void DevicePipeEdit::cancel()
{
    this->close();
}

void DevicePipeEdit::setMotorId(int motor_id)
{
    this->motor_id = motor_id;
    if(mode < 0)
    {
        if(deviceCombo->currentIndex() >= 0)
        {
            this->deviceSelectChanged("");
        }
    }
}
