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

#include "sqlitedb.h"

DevicePipeEdit::DevicePipeEdit(QSqlTableModel *model,SqliteDB *db,QWidget *parent) : QDialog(parent),pipemodel(model),m_db(db)
{
    this->setWindowTitle(tr("Device Pipe Edit"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(250,220);
    this->setMaximumSize(250,220);

    this->setWindowIcon(QIcon(":/images/icon"));

    QVBoxLayout *playout = new QVBoxLayout;

    QGridLayout *items = new QGridLayout;
    QLabel *pnameLabel = new QLabel(this);
    pnameLabel->setText(tr("pipe name:"));
    pipeNameEdit = new QLineEdit(this);
    pipeNameEdit->setPlaceholderText(tr("please enter pipe name"));

    items->addWidget(pnameLabel,0,0,1,1,Qt::AlignRight);
    items->addWidget(pipeNameEdit,0,1,1,1);

    //
    QLabel *pmotorLabel = new QLabel(this);
    pmotorLabel->setText(tr("Motor:"));
    motorCombo = new QComboBox(this);

    QSqlTableModel *motormodel = m_db->modelNoHeader("motor");
    int tmpRow = motormodel->rowCount();

    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = motormodel->record(i);
        motorList<<record.value("mcode").toString();
    }
    motorCombo->addItems(motorList);
    delete motormodel;

    items->addWidget(pmotorLabel,1,0,1,1,Qt::AlignRight);
    items->addWidget(motorCombo,1,1,1,1);

    //
    QLabel *pisactiveLabel = new QLabel(this);
    pisactiveLabel->setText(tr("Is Active:"));
    isActiveChBox = new QCheckBox(this);

    items->addWidget(pisactiveLabel,2,0,1,1,Qt::AlignRight);
    items->addWidget(isActiveChBox,2,1,1,1);

    playout->addLayout(items);
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

void DevicePipeEdit::setmode(int value)
{
    mode = value;
    if (mode >= 0) //update bear
    {
        this->setWindowTitle(tr("Device Pipe Edit"));
    }
    else
    {
        this->setWindowTitle(tr("Device Pipe Edit"));
    }
}

void DevicePipeEdit::setmodel(QList<QString> values)
{
    QString dcode;
    QString pipecode;
    QString name;
    QString motor;
    QString isactive;

    dcode = values.at(0);
    pipecode = values.at(1);
    name = values.at(2);
    motor = values.at(3);
    isactive = values.at(4);

    this->dcode = dcode;
    this->pipecode = pipecode;

    pipeNameEdit->setText(name);
    isActiveChBox->setChecked(!isactive.compare("1"));

    QList<QString>::Iterator it,itend;
    it = motorList.begin(),itend = motorList.end();
    int i = 0;
    for (;it != itend; it++,i++){
        if ((*it).compare(motor) == 0){//找到，高亮显示
            motorCombo->setCurrentIndex(i);
            break;
        }
    }
}

bool DevicePipeEdit::editpipe()
{

    QString namestr = pipeNameEdit->text().trimmed();
    QString motorcode = motorCombo->currentText();
    bool is_active = isActiveChBox->isChecked();

    if (namestr.length() == 0 || motorCombo->currentIndex() < 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return false;
    }

    if (0 > mode)
    {

    }
    else
    {
        QString filter = "dcode='" + dcode + "' and pipeCode='" + pipecode + "'";
        pipemodel->setFilter(filter);
        if (pipemodel->select()) {
            if (pipemodel->rowCount() == 1)
            {
                QSqlRecord record = pipemodel->record(0);
                record.setValue("name", namestr);
                record.setValue("motor", motorcode);
                record.setValue("isactive", (is_active?"1":"0"));
                pipemodel->setRecord(0, record);
                pipemodel->submitAll();
            }
            else
            {
                return false;
            }
            pipemodel->setFilter("id>0");
            pipemodel->select();
        }
    }

    return true;
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
