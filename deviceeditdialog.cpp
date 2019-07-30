#include "deviceeditdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlRecord>
#include <QCheckBox>
#include <QList>
#include <QQueue>
#include <QTabWidget>

#include <QDebug>

#include "deviceinfo.h"
#include "globalvariable.h"
#include "globalfunction.h"
#include "sqlitedb.h"

#include "cmieedeviceinfo.h"
#include "cmievdeviceinfo.h"
#include "cmietdeviceinfo.h"

DeviceEditDialog::DeviceEditDialog(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    ratio1 = 0.0;
    ratio2 = 0.0;
    ratio3 = 0.0;
    ratio4 = 0.0;
    ratio5 = 0.0;
    ratio6 = 0.0;

    sample_num = 0;
    sample_interval = 0.0;

    this->setWindowTitle(tr("Device list Edit"));

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(400,300);
    this->setMaximumSize(400,300);

    this->setWindowIcon(QIcon(":/images/icon"));

    //QWidget *contentboard = new QWidget(this);
    mainWidget = new QTabWidget(this);

    QVBoxLayout *playout = new QVBoxLayout;

    QDialog *board = new QDialog(this);
    QGridLayout *items = new QGridLayout(board);
    items->setMargin(5);
    items->setVerticalSpacing(5);
    items->setAlignment(Qt::AlignCenter);

    int index = 0;
    QLabel *pcodeLabel = new QLabel(this);
    pcodeLabel->setText(tr("code:"));
    codeEdit = new QLineEdit(this);

    items->addWidget(pcodeLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(codeEdit,index,1,1,1);

    QLabel *pnameLabel = new QLabel(this);
    pnameLabel->setText(tr("name:"));
    nameEdit = new QLineEdit(this);

    index++;
    items->addWidget(pnameLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(nameEdit,index,1,1,1);

    QLabel *ptypeLabel = new QLabel(this);
    ptypeLabel->setText(tr("type:"));
    typeCombo = new QComboBox();

    QList<QString> devicetypelist;
    devicetypelist << tr("model") << tr("dtype") << tr("pipenum");
    QSqlTableModel *devicetypemodel = m_db->model("devicetype",devicetypelist);

    int tmpRow = devicetypemodel->rowCount();

    //QStringList deviceModelList;
    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = devicetypemodel->record(i);
        QString model = record.value("model").toString();
        QString type = record.value("dtype").toString();
        QString channels = record.value("pipenum").toString();

        typeCombo->addItem(model,type+","+channels);
    }
    connect(typeCombo,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(deviceSelectChanged(const QString &)));
    typeCombo->setCurrentIndex(0);
    //typeCombo->addItems(deviceModelList);
    delete devicetypemodel;

    typeEditBtn = new QPushButton(this);
    typeEditBtn->setText(tr("..."));
    typeEditBtn->setMaximumWidth(40);
    typeEditBtn->setEnabled(false);

    index++;
    items->addWidget(ptypeLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(typeCombo,index,1,1,1);
    items->addWidget(typeEditBtn,index,2,1,1);

    QLabel *pipLabel = new QLabel(this);
    pipLabel->setText(tr("ip address:"));
    ipaddressText = new QLineEdit(this);
    ipaddressText->setPlaceholderText(tr("Ip address"));

    index++;
    items->addWidget(pipLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(ipaddressText,index,1,1,1);

    QLabel *portLabel = new QLabel(this);
    portLabel->setText(tr("port:"));
    portEdit = new QLineEdit(this);
    portEdit->setPlaceholderText(tr("port"));

    index++;
    items->addWidget(portLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(portEdit,index,1,1,1);

    mainWidget->addTab(board,tr("basic information"));

    ///////////////////////////////////////////////////////////
    index = 0;
    QDialog *boardsample = new QDialog(this);
    QGridLayout *sampleItems = new QGridLayout(boardsample);
    sampleItems->setMargin(5);
    sampleItems->setVerticalSpacing(5);
    sampleItems->setAlignment(Qt::AlignCenter);

    QLabel *pmodelLabel = new QLabel(this);
    pmodelLabel->setText(tr("Measure Sample Interval:"));
    QLabel *pmintervalMetricLabel = new QLabel(this);
    pmintervalMetricLabel->setText(tr("(s)"));
    measureIntervalEdit = new QLineEdit(this);

    index++;
    sampleItems->addWidget(pmodelLabel,index,0,1,1,Qt::AlignRight);
    sampleItems->addWidget(measureIntervalEdit,index,1,1,1);
    sampleItems->addWidget(pmintervalMetricLabel,index,2,1,1);

    /*
    pfreqLabel = new QLabel(this);
    pfreqLabel->setText(tr("Freq Sample Interval:"));
    pfintervalMetricLabel = new QLabel(this);
    pfintervalMetricLabel->setText(tr("(s)"));
    freqIntervalEdit = new QLineEdit(this);

    index++;
    sampleItems->addWidget(pfreqLabel,index,0,1,1,Qt::AlignRight);
    sampleItems->addWidget(freqIntervalEdit,index,1,1,1);
    sampleItems->addWidget(pfintervalMetricLabel,index,2,1,1);
    */

    prinLabel = new QLabel(this);
    prinLabel->setText(tr("Wave Sample Interval:"));
    pwintervalMetricLabel = new QLabel(this);
    pwintervalMetricLabel->setText(tr("(s)"));
    waveIntervalEdit = new QLineEdit(this);

    index++;
    sampleItems->addWidget(prinLabel,index,0,1,1,Qt::AlignRight);
    sampleItems->addWidget(waveIntervalEdit,index,1,1,1);
    sampleItems->addWidget(pwintervalMetricLabel,index,2,1,1);

    psampleNumLabel = new QLabel(this);
    psampleNumLabel->setText(tr("Sample Numbers:"));
    sampleNumEdit = new QComboBox(this);

    sampleNumEdit->addItem("16384","0");

    index++;
    sampleItems->addWidget(psampleNumLabel,index,0,1,1,Qt::AlignRight);
    sampleItems->addWidget(sampleNumEdit,index,1,1,1);

    psampleIntervalLabel = new QLabel(this);
    psampleIntervalLabel->setText(tr("Sample Interval:"));
    sampleIntervalEdit = new QComboBox(this);
    sampleIntervalEdit->addItem("1","1");
    sampleIntervalEdit->addItem("2","2");
    sampleIntervalEdit->addItem("3","3");
    sampleIntervalEdit->addItem("4","4");
    sampleIntervalEdit->addItem("5","5");
    sampleIntervalEdit->addItem("6","6");
    sampleIntervalEdit->addItem("7","7");
    sampleIntervalEdit->addItem("8","8");

    index++;
    sampleItems->addWidget(psampleIntervalLabel,index,0,1,1,Qt::AlignRight);
    sampleItems->addWidget(sampleIntervalEdit,index,1,1,1);

    //deviceModeCB

    /*
    pmodeLabel = new QLabel(this);
    pmodeLabel->setText(tr("Device run mode:"));
    deviceModeCB = new QCheckBox(this);
    deviceModeCB->setText(tr("Wave mode"));

    index++;
    sampleItems->addWidget(pmodeLabel,index,0,1,1,Qt::AlignRight);
    sampleItems->addWidget(deviceModeCB,index,1,1,1);
    */

    mainWidget->addTab(boardsample,tr("sample setting"));

    ///////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////
    index = 0;
    QDialog *boardratio = new QDialog(this);
    QGridLayout *ratiosItems = new QGridLayout(boardratio);
    ratiosItems->setMargin(5);
    ratiosItems->setVerticalSpacing(5);
    ratiosItems->setAlignment(Qt::AlignCenter);

    channel1Label = new QLabel(this);
    channel1Label->setText(tr("Channel1:"));
    channel1Edit = new QLineEdit(this);

    index++;
    ratiosItems->addWidget(channel1Label,index,0,1,1,Qt::AlignRight);
    ratiosItems->addWidget(channel1Edit,index,1,1,1);

    channel2Label = new QLabel(this);
    channel2Label->setText(tr("Channel2:"));
    channel2Edit = new QLineEdit(this);

    index++;
    ratiosItems->addWidget(channel2Label,index,0,1,1,Qt::AlignRight);
    ratiosItems->addWidget(channel2Edit,index,1,1,1);

    channel3Label = new QLabel(this);
    channel3Label->setText(tr("Channel3:"));
    channel3Edit = new QLineEdit(this);

    index++;
    ratiosItems->addWidget(channel3Label,index,0,1,1,Qt::AlignRight);
    ratiosItems->addWidget(channel3Edit,index,1,1,1);

    channel4Label = new QLabel(this);
    channel4Label->setText(tr("Channel4:"));
    channel4Edit = new QLineEdit(this);

    index++;
    ratiosItems->addWidget(channel4Label,index,0,1,1,Qt::AlignRight);
    ratiosItems->addWidget(channel4Edit,index,1,1,1);

    channel5Label = new QLabel(this);
    channel5Label->setText(tr("Channel5:"));
    channel5Edit = new QLineEdit(this);

    index++;
    ratiosItems->addWidget(channel5Label,index,0,1,1,Qt::AlignRight);
    ratiosItems->addWidget(channel5Edit,index,1,1,1);

    channel6Label = new QLabel(this);
    channel6Label->setText(tr("Channel6:"));
    channel6Edit = new QLineEdit(this);

    index++;
    ratiosItems->addWidget(channel6Label,index,0,1,1,Qt::AlignRight);
    ratiosItems->addWidget(channel6Edit,index,1,1,1);

    ratiodialog_index = mainWidget->addTab(boardratio,tr("channel ratio"));
    ///////////////////////////////////////////////////////////

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
}

void DeviceEditDialog::setmode(int value)
{
    mode = value;
    if (mode >= 0) //update bear
    {
        DeviceInfo* di = nullptr;
        QVector<DeviceInfo*>::ConstIterator cIt;
        cIt = GlobalVariable::deviceInfos.constBegin();
        for(;cIt != GlobalVariable::deviceInfos.constEnd(); cIt++)
        {
            DeviceInfo* item = *cIt;
            if(mode == item->tree_id)
            {
                codeEdit->setText(item->deviceCode);
                nameEdit->setText(item->title);
                ipaddressText->setText(item->ipAddress);
                portEdit->setText(QString::number(item->ipPort));

                int typeNum = typeCombo->count();
                for(int i=0;i<typeNum;i++)
                {
                    QString itemText = typeCombo->itemText(i);
                    if(QString::compare(item->deviceModel,itemText,Qt::CaseInsensitive) == 0)
                    {
                        typeCombo->setCurrentIndex(i);
                        break;
                    }
                }
                typeCombo->setEnabled(false);

                deviceSelectChanged("");
                measureIntervalEdit->setText(QString::number(item->measure_interval));
                //freqIntervalEdit->setText(QString::number(item->freq_interval));
                waveIntervalEdit->setText(QString::number(item->wave_interval));

                QString samples = item->getParam(17);
                if(samples.length() == 0)
                {
                    sampleNumEdit->setCurrentIndex(0);
                    sampleIntervalEdit->setCurrentIndex(0);

                    channel1Edit->setText("1.0");
                    channel2Edit->setText("1.0");
                    channel3Edit->setText("1.0");
                    channel4Edit->setText("1.0");
                    channel5Edit->setText("1.0");
                    channel6Edit->setText("1.0");
                }
                else
                {
                    bool tok;
                    QStringList sample_items = samples.split(',');
                    QString tmp = sample_items[0];
                    sample_num = tmp.toInt(&tok);
                    sampleNumEdit->setCurrentIndex(0);

                    QString intervals = item->getParam(18);
                    QStringList interval_items = intervals.split(',');
                    tmp = interval_items[0];
                    float tmp_f = tmp.toFloat(&tok);
                    sample_interval = (int)roundf(tmp_f);
                    sample_interval--;
                    sampleIntervalEdit->setCurrentIndex(sample_interval);

                    tmp = item->getParam(10);
                    ratio1 = tmp.toFloat(&tok);
                    ratio1 /= PC_RATIO;
                    channel1Edit->setText(tmp);

                    tmp = item->getParam(11);
                    ratio2 = tmp.toFloat(&tok);
                    ratio2 /= PC_RATIO;
                    channel2Edit->setText(tmp);

                    tmp = item->getParam(12);
                    ratio3 = tmp.toFloat(&tok);
                    ratio3 /= PC_RATIO;
                    channel3Edit->setText(tmp);

                    tmp = item->getParam(13);
                    ratio4 = tmp.toFloat(&tok);
                    ratio4 /= PC_RATIO;
                    channel4Edit->setText(tmp);

                    tmp = item->getParam(14);
                    ratio5 = tmp.toFloat(&tok);
                    ratio5 /= PC_RATIO;
                    channel5Edit->setText(tmp);

                    tmp = item->getParam(15);
                    ratio6 = tmp.toFloat(&tok);
                    ratio6 /= PC_RATIO;
                    channel6Edit->setText(tmp);
                }

                /*
                if(!deviceModeCB->isHidden())
                {
                    QString run_m = item->getParam(19);
                    if(run_m.length()>0)
                    {
                        bool tok;
                        int m;
                        m = run_m.toInt(&tok);
                        if(0==m)
                        {
                            deviceModeCB->setChecked(false);
                            run_mode = false;
                        }
                        else
                        {
                            deviceModeCB->setChecked(true);
                            run_mode = true;
                        }
                    }
                }
                */
            }
        }
        codeEdit->setEnabled(false);
        this->setWindowTitle(tr("Device Edit"));
    }
    else
    {
        ipaddressText->setText("192.168.1.6");
        portEdit->setText("502");
        measureIntervalEdit->setText("2");
        //freqIntervalEdit->setText("600");
        waveIntervalEdit->setText("10");

        sampleNumEdit->setCurrentIndex(0);
        sampleIntervalEdit->setCurrentIndex(0);

        channel1Edit->setText("1");
        channel2Edit->setText("1");
        channel3Edit->setText("1");
        channel4Edit->setText("1");
        channel5Edit->setText("1");
        channel6Edit->setText("1");

        sampleNumEdit->setEnabled(false);
        sampleIntervalEdit->setEnabled(false);

        channel1Edit->setEnabled(false);
        channel2Edit->setEnabled(false);
        channel3Edit->setEnabled(false);
        channel4Edit->setEnabled(false);
        channel5Edit->setEnabled(false);
        channel6Edit->setEnabled(false);

        this->setWindowTitle(tr("Device Add"));

        deviceSelectChanged("");
    }
}

bool DeviceEditDialog::editdevice()
{
    QString title = nameEdit->text().trimmed();
    QString type = typeCombo->currentText();
    QString ip = ipaddressText->text().trimmed();
    QString port = portEdit->text().trimmed();

    QString device_code = codeEdit->text().trimmed();

    QString m_interval_str = measureIntervalEdit->text().trimmed();
    //QString f_interval_str = freqIntervalEdit->text().trimmed();
    QString f_interval_str = "500";
    QString w_interval_str = waveIntervalEdit->text().trimmed();

    QString sample = sampleNumEdit->itemData(sampleNumEdit->currentIndex()).toString();
    sample = sampleNumEdit->currentText();
    QString interval = sampleIntervalEdit->itemData(sampleIntervalEdit->currentIndex()).toString();

    QString ch1_ratio = channel1Edit->text().trimmed();
    QString ch2_ratio = channel2Edit->text().trimmed();
    QString ch3_ratio = channel3Edit->text().trimmed();
    QString ch4_ratio = channel4Edit->text().trimmed();
    QString ch5_ratio = channel5Edit->text().trimmed();
    QString ch6_ratio = channel6Edit->text().trimmed();

    float ch1_ratio_f;
    float ch2_ratio_f;
    float ch3_ratio_f;
    float ch4_ratio_f;
    float ch5_ratio_f;
    float ch6_ratio_f;

    if (ch1_ratio.length() == 0 || ch2_ratio.length() == 0 || ch3_ratio.length() == 0 || ch4_ratio.length() == 0 || ch5_ratio.length() == 0 \
            || ch6_ratio.length() == 0 || sample.length() == 0 || interval.length() == 0 || title.length()==0 || type.length()==0 || ip.length()==0 \
            || port.length()==0 || typeCombo->currentIndex() < 0 || device_code.length() == 0 || m_interval_str.length() == 0 \
            || f_interval_str.length() == 0 || w_interval_str.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return false;
    }
    bool tok;
    int port_i;

    QString udatas = typeCombo->itemData(typeCombo->currentIndex()).toString();
    QString udata = udatas.split(',')[0];
    QString channels = udatas.split(',')[1];
    int channelNum = channels.toInt(&tok);
    port_i = port.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        portEdit->setFocus();
        return false;
    }

    if(!GlobalFunction::isValidIpAddress(ip))
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter valid ip address!"));
        ipaddressText->setFocus();
        return false;
    }
    int m_interval_i,f_interval_i,w_interval_i,sample_i,interval_f;
    m_interval_i = m_interval_str.toInt(&tok);
    if(!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        measureIntervalEdit->setFocus();
        return false;
    }

    if(deviceType.compare("T",Qt::CaseInsensitive) !=0)
    {
        f_interval_i = 500;
        /*
        f_interval_i = f_interval_str.toInt(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            freqIntervalEdit->setFocus();
            return false;
        }
        */

        w_interval_i = w_interval_str.toInt(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            waveIntervalEdit->setFocus();
            return false;
        }

        sample_i = sample.toInt(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            sampleNumEdit->setFocus();
            return false;
        }

        interval_f = interval.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            sampleIntervalEdit->setFocus();
            return false;
        }

        if(w_interval_i <= interval_f)
        {
            w_interval_i = interval_f + 1;
        }

        ch1_ratio_f = ch1_ratio.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            channel1Edit->setFocus();
            return false;
        }
        ch1_ratio_f /= PC_RATIO;

        ch2_ratio_f = ch2_ratio.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            channel2Edit->setFocus();
            return false;
        }
        ch2_ratio_f /= PC_RATIO;

        ch3_ratio_f = ch3_ratio.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            channel3Edit->setFocus();
            return false;
        }
        ch3_ratio_f /= PC_RATIO;

        ch4_ratio_f = ch4_ratio.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            channel4Edit->setFocus();
            return false;
        }
        ch4_ratio_f /= PC_RATIO;

        ch5_ratio_f = ch5_ratio.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            channel5Edit->setFocus();
            return false;
        }
        ch5_ratio_f /= PC_RATIO;

        ch6_ratio_f = ch6_ratio.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            channel6Edit->setFocus();
            return false;
        }
        ch6_ratio_f /= PC_RATIO;
    }

    int ch1_ratio_i = 0;
    int ch2_ratio_i = 0;
    int ch3_ratio_i = 0;
    int ch4_ratio_i = 0;
    int ch5_ratio_i = 0;
    int ch6_ratio_i = 0;

    if(deviceType.compare("T",Qt::CaseInsensitive) !=0)
    {
        ch1_ratio_i = ch1_ratio_f;
        ch2_ratio_i = ch2_ratio_f;
        ch3_ratio_i = ch3_ratio_f;
        ch4_ratio_i = ch4_ratio_f;
        ch5_ratio_i = ch5_ratio_f;
        ch6_ratio_i = ch6_ratio_f;
    }

    QString sql;
    if (0 < mode)
    {
        sql = "update devicetree set title='" + title + "' where id=" + QString::number(mode);
        QString sql_device = "update device set name='" + title + "',dtype='" + udata + "',dmodel='" + type + "',ipaddress='" + ip + "',port=" + QString::number(port_i) + ",minterval=" + m_interval_str + ",finterval=" + f_interval_str + ",winterval=" + w_interval_str + " where treeid=" + QString::number(mode);
        QQueue<QString> sqls;
        sqls.enqueue(sql);
        sqls.enqueue(sql_device);
        if(m_db->execSql(sqls))
        {
            QVector<TreeNodeInfo*>::ConstIterator cIt;
            cIt = GlobalVariable::deviceTreeInfos.constBegin();

            for(;cIt != GlobalVariable::deviceTreeInfos.constEnd(); cIt++)
            {
                TreeNodeInfo* item = *cIt;
                if(item->id == mode)
                {
                    item->title = title;
                    break;
                }
            }

            QVector<DeviceInfo*>::ConstIterator cIt_d;
            cIt_d = GlobalVariable::deviceInfos.constBegin();

            for(;cIt_d != GlobalVariable::deviceInfos.constEnd(); cIt_d++)
            {
                DeviceInfo* item = *cIt_d;
                if(item->tree_id == mode)
                {
                    item->title = title;
                    //item->deviceModel = type;
                    item->ipAddress = ip;
                    item->ipPort = port_i;
                    //item->deviceType = udata;
                    item->measure_interval = m_interval_i;

                    if(deviceType.compare("T",Qt::CaseInsensitive) !=0)
                    {
                        item->freq_interval = f_interval_i;
                        item->wave_interval = w_interval_i;
                        item->samples.clear();
                        item->samples.append(sample_i);
                        item->intervals.clear();
                        item->intervals.append(interval_f);

                        int tmp_i = interval_f;

                        if(item->linkState)
                        {
                            if(qAbs(ch1_ratio_f-ratio1) > 0.0001 || qAbs(ch2_ratio_f-ratio2) > 0.0001 || qAbs(ch3_ratio_f-ratio3) > 0.0001 || qAbs(ch4_ratio_f-ratio4) > 0.0001 || qAbs(ch5_ratio_f-ratio5) > 0.0001 || qAbs(ch6_ratio_f-ratio6) > 0.0001 || sample_i != sample_num || qAbs(interval_f-sample_interval) > 0.0001)
                            {
                                struct ModbusTCPMapInfo cmd_sample;
                                cmd_sample.Unit = COM_W;
                                cmd_sample.Addr = 11;
                                cmd_sample.Command = 0x10;
                                cmd_sample.Length = 16;
                                cmd_sample.data = new unsigned char[16];

                                cmd_sample.data[0] = ch1_ratio_i / 256;
                                cmd_sample.data[1] = ch1_ratio_i % 256;
                                cmd_sample.data[2] = ch2_ratio_i / 256;
                                cmd_sample.data[3] = ch2_ratio_i % 256;
                                cmd_sample.data[4] = ch3_ratio_i / 256;
                                cmd_sample.data[5] = ch3_ratio_i % 256;
                                cmd_sample.data[6] = ch4_ratio_i / 256;
                                cmd_sample.data[7] = ch4_ratio_i % 256;
                                cmd_sample.data[8] = ch5_ratio_i / 256;
                                cmd_sample.data[9] = ch5_ratio_i % 256;
                                cmd_sample.data[10] = ch6_ratio_i / 256;
                                cmd_sample.data[11] = ch6_ratio_i % 256;
                                cmd_sample.data[12] = sample_i / 256;
                                cmd_sample.data[13] = sample_i % 256;
                                cmd_sample.data[14] = tmp_i / 256;
                                cmd_sample.data[15] = tmp_i % 256;
                                cmd_sample.ExpectLen = 15;

                                item->addSendMsg(cmd_sample);
                            }

                            /*
                            if(!deviceModeCB->isHidden())
                            {
                                int a = run_mode? 1:0;
                                int b = deviceModeCB->isChecked()? 1:0;
                                if(a != b)
                                {
                                    struct ModbusTCPMapInfo cmd_mode;
                                    cmd_mode.Unit = MODE_V_W;
                                    cmd_mode.Addr = 36;
                                    cmd_mode.Command = 0x10;
                                    cmd_mode.Length = 2;
                                    cmd_mode.data = new unsigned char[2];
                                    cmd_mode.data[0] = 0x0;
                                    if(deviceModeCB->isChecked())
                                    {
                                        cmd_mode.data[1] = 0x4;
                                    }
                                    else
                                    {
                                        cmd_mode.data[1] = 0x0;
                                    }
                                    cmd_mode.ExpectLen = 15;
                                    item->addSendMsg(cmd_mode);
                                }
                            }
                            */
                        }
                    }
                    break;
                }
            }
            emit deivceEditStateChange(mode,title);
        }
        else
        {
            QMessageBox::information(this, tr("Infomation"), tr("update data failed!"));
            return false;
        }
    }
    else
    {
        sql = "select * from device where code='" + device_code + "'";
        if(m_db->queryCount(sql) > 0)
        {
            QMessageBox::information(this, tr("Infomation"), tr("The device code has existed!"));
            return false;
        }
        else
        {
            GlobalVariable::maxDeviceTreeId++;
            sql = "insert into devicetree (id,type,title,pid) values (" + QString::number(GlobalVariable::maxDeviceTreeId) + ",21,'" + title + "',0)";
            QString sql_device = "insert into device (code,name,dmodel,dtype,ipaddress,port,isactive,treeid,minterval,finterval,winterval) values ('" + device_code + "','" + title + "','" + type + "','" + udata + "','" + ip + "'," + QString::number(port_i) + ",1," + QString::number(GlobalVariable::maxDeviceTreeId) + "," + m_interval_str + "," + f_interval_str + "," + w_interval_str + ")";
            //qDebug()<<sql_device;
            QQueue<QString> sqls;
            sqls.enqueue(sql);
            sqls.enqueue(sql_device);
            if(m_db->execSql(sqls))
            {
                TreeNodeInfo* tni = new TreeNodeInfo();
                tni->id = GlobalVariable::maxDeviceTreeId;
                tni->type = DEVICE;
                tni->title = title;
                tni->pid = 0;
                GlobalVariable::deviceTreeInfos.append(tni);

                if (type.compare("CKYB0001TAGQ") == 0) //e7 electirc sample device
                {
                    CMIEEDeviceInfo *eInfo = new CMIEEDeviceInfo();
                    eInfo->init();
                    eInfo->tree_id = GlobalVariable::maxDeviceTreeId;;
                    eInfo->type = DEVICE;
                    eInfo->pid = 0;
                    eInfo->deviceCode = device_code;
                    eInfo->title = title;
                    eInfo->ipAddress = ip;
                    eInfo->ipPort = port_i;
                    eInfo->deviceType = udata;
                    eInfo->deviceModel = type;
                    eInfo->measure_interval = m_interval_i;
                    eInfo->freq_interval = f_interval_i;
                    eInfo->wave_interval = w_interval_i;
                    //eInfo->samples.append(sample_i);
                    //eInfo->intervals.append(interval_f);

                    //int tmp_i = interval_f;
                    /*
                    struct ModbusTCPMapInfo cmd_sample;
                    cmd_sample.Unit = COM_W;
                    cmd_sample.Addr = 18;
                    cmd_sample.Command = 0x10;
                    cmd_sample.Length = 4;
                    cmd_sample.data = new unsigned char[4];
                    cmd_sample.data[0] = sample_i / 256;
                    cmd_sample.data[1] = sample_i % 256;
                    cmd_sample.data[2] = tmp_i / 256;
                    cmd_sample.data[3] = tmp_i % 256;
                    cmd_sample.ExpectLen = 15;

                    eInfo->addSendMsg(cmd_sample);
                    */

                    GlobalVariable::deviceInfos.append(eInfo);
                }
                else if (type.compare("CKYB3006LEHQ") == 0) //e7 vibrate sample device
                {
                    CMIEVDeviceInfo *vInfo = new CMIEVDeviceInfo();
                    vInfo->init();
                    vInfo->tree_id = GlobalVariable::maxDeviceTreeId;;
                    vInfo->type = DEVICE;
                    vInfo->pid = 0;
                    vInfo->deviceCode = device_code;
                    vInfo->title = title;
                    vInfo->ipAddress = ip;
                    vInfo->ipPort = port_i;
                    vInfo->deviceType = udata;
                    vInfo->deviceModel = type;
                    vInfo->measure_interval = m_interval_i;
                    vInfo->freq_interval = f_interval_i;
                    vInfo->wave_interval = w_interval_i;
                    //vInfo->samples.append(sample_i);
                    //vInfo->intervals.append(interval_f);

                    //int tmp_i = interval_f;
                    /*
                    struct ModbusTCPMapInfo cmd_sample;
                    cmd_sample.Unit = COM_W;
                    cmd_sample.Addr = 18;
                    cmd_sample.Command = 0x10;
                    cmd_sample.Length = 4;
                    cmd_sample.data = new unsigned char[4];
                    cmd_sample.data[0] = sample_i / 256;
                    cmd_sample.data[1] = sample_i % 256;
                    cmd_sample.data[2] = tmp_i / 256;
                    cmd_sample.data[3] = tmp_i % 256;
                    cmd_sample.ExpectLen = 15;

                    vInfo->addSendMsg(cmd_sample);
                    */
                    GlobalVariable::deviceInfos.append(vInfo);
                }
                else if (type.compare("CMIE-T") == 0) //e7 vibrate sample device
                {
                    CMIETDeviceInfo *tInfo = new CMIETDeviceInfo();

                    tInfo->init();
                    tInfo->tree_id = GlobalVariable::maxDeviceTreeId;;
                    tInfo->type = DEVICE;
                    tInfo->pid = 0;
                    tInfo->deviceCode = device_code;
                    tInfo->title = title;
                    tInfo->ipAddress = ip;
                    tInfo->ipPort = port_i;
                    tInfo->deviceType = udata;
                    tInfo->deviceModel = type;
                    tInfo->measure_interval = m_interval_i;
                    tInfo->freq_interval = f_interval_i;
                    tInfo->wave_interval = w_interval_i;

                    GlobalVariable::deviceInfos.append(tInfo);
                }
                else
                {
                    DeviceInfo *info = new DeviceInfo();

                    info->init();
                    info->tree_id = GlobalVariable::maxDeviceTreeId;;
                    info->type = DEVICE;
                    info->pid = 0;
                    info->deviceCode = device_code;
                    info->title = title;
                    info->ipAddress = ip;
                    info->ipPort = port_i;
                    info->deviceType = udata;
                    info->deviceModel = type;
                    info->measure_interval = m_interval_i;
                    info->freq_interval = f_interval_i;
                    info->wave_interval = w_interval_i;

                    GlobalVariable::deviceInfos.append(info);
                }

                emit deivceEditStateChange(GlobalVariable::maxDeviceTreeId,title);
            }
            else
            {
                QMessageBox::information(this, tr("Infomation"), tr("Insert data failed!"));
                return false;
            }
        }
    }

    return true;
}

void DeviceEditDialog::ok()
{
    if (this->editdevice())
    {
        this->close();
    }
}

void DeviceEditDialog::cancel()
{
    this->close();
}

void DeviceEditDialog::deviceSelectChanged(const QString &text)
{
    QString udata = typeCombo->currentText();

    if (udata.compare("CKYB0001TAGQ") == 0) //e7 electirc sample device
    {
        channel1Label->setText(tr("Uab:"));
        channel2Label->setText(tr("Ubc:"));
        channel3Label->setText(tr("Uca:"));

        channel4Label->setText(tr("Ia:"));
        channel5Label->setText(tr("Ib:"));
        channel6Label->setText(tr("Ic:"));

        //pmodeLabel->setHidden(true);
        //deviceModeCB->setHidden(true);
        deviceType = "E";
    }
    else if (udata.compare("CKYB3006LEHQ") == 0) //e7 vibrate sample device
    {
        channel1Label->setText(tr("Channel1:"));
        channel2Label->setText(tr("Channel2:"));
        channel3Label->setText(tr("Channel3:"));
        channel3Label->setText(tr("Channel4:"));
        channel4Label->setText(tr("Channel5:"));
        channel5Label->setText(tr("Channel6:"));

        //pmodeLabel->setHidden(false);
        //deviceModeCB->setHidden(false);
        deviceType = "V";
    }
    else if (udata.compare("CKYB5003TEMQ") == 0 || udata.compare("TW888") == 0) //e7 temperature sample device
    {
        mainWidget->removeTab(ratiodialog_index);
        deviceType = "T";

        //freqIntervalEdit->setVisible(false);
        waveIntervalEdit->setVisible(false);
        sampleNumEdit->setVisible(false);
        sampleIntervalEdit->setVisible(false);
        //pmodeLabel->setVisible(false);
        //deviceModeCB->setVisible(false);

        //pfreqLabel->setVisible(false);
        //pfintervalMetricLabel->setVisible(false);

        prinLabel->setVisible(false);
        pwintervalMetricLabel->setVisible(false);

        psampleNumLabel->setVisible(false);
        psampleIntervalLabel->setVisible(false);
    }
    else
    {

    }
}

DeviceEditDialog::~DeviceEditDialog()
{

}
