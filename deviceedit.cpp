#include "deviceedit.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlRecord>
#include <QException>

#include <QList>

#include "sqlitedb.h"
#include <QDebug>

DeviceEdit::DeviceEdit(QSqlTableModel *model,SqliteDB *db,QWidget *parent) : QDialog(parent),devicemodel(model),m_db(db)
{
    this->setWindowTitle(tr("Device list Edit"));

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(250,250);
    //this->setMaximumSize(250,220);

    this->setWindowIcon(QIcon(":/images/icon"));

    QVBoxLayout *playout = new QVBoxLayout;

    QGridLayout *items = new QGridLayout;

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

    QList<QString> devicetypelist;
    devicetypelist << tr("model") << tr("type") << tr("description");
    QSqlTableModel *devicetypemodel = m_db->model("devicetype",devicetypelist);

    QLabel *pmodelLabel = new QLabel(this);
    pmodelLabel->setText(tr("model:"));
    dmodelCombo = new QComboBox(this);

    int tmpRow = devicetypemodel->rowCount();

    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = devicetypemodel->record(i);
        deviceModelList<<record.value("model").toString();
        m_map.insert(record.value("model").toString(),record.value("dtype").toString());
        m_pipes.insert(record.value("model").toString(),record.value("pipenum").toInt());
    }

    dmodelCombo->addItems(deviceModelList);
    delete devicetypemodel;
    index++;
    connect(dmodelCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(modelValueChanged(const QString &)));
    items->addWidget(pmodelLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(dmodelCombo,index,1,1,1);

    QLabel *ptypeLabel = new QLabel(this);
    ptypeLabel->setText(tr("type:"));
    typeText = new QLineEdit();
    typeText->setEnabled(false);
    typeText->setText(m_map[dmodelCombo->currentText()]);

    index++;
    items->addWidget(ptypeLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(typeText,index,1,1,1);

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

    /*
    QLabel *pmotorLabel = new QLabel(this);
    pmotorLabel->setText(tr("Motor:"));
    motorCombo = new QComboBox(this);

    QList<QString> motorlist;
    motorlist << tr("code") << tr("name") << tr("type") << tr("bear type");

    QSqlTableModel *motormodel = m_db->model("motor",motorlist);
    tmpRow = motormodel->rowCount();

    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = motormodel->record(i);
        motorList<<record.value("mcode").toString();
    }
    motorCombo->addItems(motorList);
    delete motormodel;

    index++;
    items->addWidget(pmotorLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(motorCombo,index,1,1,1);
    */

    QLabel *pisactiveLabel = new QLabel(this);
    pisactiveLabel->setText(tr("Is Active:"));
    isactiveCheckBox = new QCheckBox(this);

    index++;
    items->addWidget(pisactiveLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(isactiveCheckBox,index,1,1,1);

    playout->addLayout(items);
    playout->setContentsMargins(10,10,10,10);

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

void DeviceEdit::setmode(int value)
{
    mode = value;

    if (mode >= 0) //update bear
    {
        codeEdit->setEnabled(false);
        papplybtn->setEnabled(false);
        this->setWindowTitle(tr("Device Edit"));
    }
    else
    {
        this->setWindowTitle(tr("Device Add"));
    }

}

void DeviceEdit::setmodel(QList<QString> values)
{
    QString codestr_l;
    QString namestr_l;
    QString modelstr_l;
    QString type_l;
    QString ipaddress_l;
    QString port_l;
    //QString motor_l;
    QString isactive_l;

    codestr_l = values.at(0);
    namestr_l = values.at(1);
    modelstr_l = values.at(2);
    type_l = values.at(3);
    ipaddress_l = values.at(4);
    port_l = values.at(5);
    //motor_l = values.at(6);
    isactive_l = values.at(6);

    codeEdit->setText(codestr_l);
    nameEdit->setText(namestr_l);
    ipaddressText->setText(ipaddress_l);
    portEdit->setText(port_l);
    isactiveCheckBox->setChecked(!isactive_l.compare("1"));

    QList<QString>::Iterator it = deviceModelList.begin(),itend = deviceModelList.end();
    int i = 0;
    for (;it != itend; it++,i++){
        if ((*it).compare(modelstr_l) == 0){//找到，高亮显示
            dmodelCombo->setCurrentIndex(i);
            break;
        }
    }

    /*
    it = motorList.begin(),itend = motorList.end();
    i = 0;
    for (;it != itend; it++,i++){
        if ((*it).compare(motor_l) == 0){//找到，高亮显示
            motorCombo->setCurrentIndex(i);
            break;
        }
    }
    */
}

bool DeviceEdit::editdevice()
{
    QString codestr = codeEdit->text().trimmed();
    QString namestr = nameEdit->text().trimmed();
    QString modelstr = dmodelCombo->currentText();
    QString type = typeText->text().trimmed();
    QString ipaddress = ipaddressText->text().trimmed();
    QString port = portEdit->text().trimmed();
    //QString motorcode = motorCombo->currentText();
    bool is_active = isactiveCheckBox->isChecked();

    if (namestr.length() == 0 || dmodelCombo->currentIndex() < 0 || type.length() == 0 || ipaddress.length() == 0 /*|| motorCombo->currentIndex() < 0*/ || codestr.length() == 0 || port.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return false;
    }

    bool tok;
    port.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        portEdit->setFocus();
        return false;
    }
    if (0 > mode)
    {
        QString filter = "code='" + codestr + "'";

        devicemodel->setFilter(filter);
        if (devicemodel->select()) {
            if (devicemodel->rowCount() > 0)
            {
                QMessageBox::information(this, tr("Infomation"), tr("Device has Existed!"));
                devicemodel->setFilter("id>0");
                devicemodel->select();
                codeEdit->setFocus();
                return false;
            }
            devicemodel->setFilter("id>0");
            devicemodel->select();
        }

        try{
            //model->transaction();
            int row=0;
            devicemodel->insertRow(row);
            devicemodel->setData(devicemodel->index(row,1),codestr);
            devicemodel->setData(devicemodel->index(row,2),namestr);
            devicemodel->setData(devicemodel->index(row,3),modelstr);
            devicemodel->setData(devicemodel->index(row,4),type);
            devicemodel->setData(devicemodel->index(row,5),ipaddress);
            devicemodel->setData(devicemodel->index(row,6),port);
            //devicemodel->setData(devicemodel->index(row,7),motorcode);
            devicemodel->setData(devicemodel->index(row,7),(is_active?"1":"0"));
            devicemodel->submitAll();

            QList<QString> pipelist;
            pipelist << tr("dcode") << tr("name") << tr("isactive");
            QSqlTableModel *devicepipemodel = m_db->model("devicepipes",pipelist);
            int pipenum = m_pipes[dmodelCombo->currentText()];
            for (int i=0;i<pipenum;i++)
            {
                devicepipemodel->insertRow(i);
                devicepipemodel->setData(devicepipemodel->index(i,1),codestr);
                devicepipemodel->setData(devicepipemodel->index(i,2),(i+1));
                QString pipeName = "pipe" + QString::number(i+1);
                devicepipemodel->setData(devicepipemodel->index(i,3),pipeName);
                devicepipemodel->setData(devicepipemodel->index(i,5),"1");
                devicepipemodel->submitAll();
            }
            //model->commit();
            delete devicepipemodel;
        }
        catch(QException ex)
        {
            //model->rollback();
        }
    }
    else
    {
        QString filter = "code='" + codestr + "'";
        devicemodel->setFilter(filter);
        if (devicemodel->select()) {
            if (devicemodel->rowCount() == 1)
            {
                QSqlRecord record = devicemodel->record(0);
                record.setValue("name", namestr);
                record.setValue("dmodel", modelstr);
                record.setValue("dtype", type);
                record.setValue("ipaddress", ipaddress);
                record.setValue("port", port);
                //record.setValue("motor", motorcode);
                record.setValue("isactive", (is_active?"1":"0"));
                devicemodel->setRecord(0, record);
                devicemodel->submitAll();
            }
            else
            {
                return false;
            }
            devicemodel->setFilter("id>0");
            devicemodel->select();
        }
    }
    return true;
}

void DeviceEdit::modelValueChanged(const QString &text)
{
    qDebug() << text;
    typeText->setText(m_map[text]);
}

void DeviceEdit::apply()
{
    this->editdevice();
}

void DeviceEdit::ok()
{
    if (this->editdevice())
    {
        this->close();
    }
}

void DeviceEdit::cancel()
{
    this->close();
}
