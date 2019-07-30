#include "serverparametersetting.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include <QList>
#include <QDebug>
#include <QCheckBox>

#include "qiplineedit.h"
#include "sqlitedb.h"

#include "globalvariable.h"

ServerParameterSetting::ServerParameterSetting(SqliteDB *db, QDialog *parent) : QDialog(parent),mdb(db)
{
    this->setWindowTitle(tr("Parameter Setting Edit"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(400,300);
    //this->setMaximumSize(400,300);

    this->setWindowIcon(QIcon(":/images/icon"));

    //QTabWidget *mainWidget = new QTabWidget(this);

    QVBoxLayout *playout = new QVBoxLayout(this);
    playout->setContentsMargins(10,10,10,10);

    QGridLayout *items_server = new QGridLayout(this);

    items_server->setAlignment(Qt::AlignCenter);

    int index = 0;
    QLabel *pserverELabel = new QLabel(this);
    pserverELabel->setText(tr("Is Server Enable:"));
    server_enable = new QCheckBox(this);
    server_enable->setChecked(GlobalVariable::server_enable);
    /*
    if(!GlobalVariable::is_sync_done)
    {
        GlobalVariable::server_enable = false;
        server_enable->setChecked(false);
        server_enable->setEnabled(false);
        QLabel *msg = new QLabel(this);
        msg->setText(tr("Motor Configure Info must sync first!"));
        items_server->addWidget(msg,index,0,1,2,Qt::AlignLeft);
    }
    */

    index++;
    items_server->addWidget(pserverELabel,index,0,1,1,Qt::AlignRight);
    items_server->addWidget(server_enable,index,1,1,1);

    QLabel *pserverLabel = new QLabel(this);
    pserverLabel->setText(tr("Server Ip Address:"));
    serverIpEdit = new QIPLineEdit(this);
    serverIpEdit->setText(GlobalVariable::server_ip);

    index++;
    items_server->addWidget(pserverLabel,index,0,1,1,Qt::AlignRight);
    items_server->addWidget(serverIpEdit,index,1,1,1);

    //mainWidget->addTab(board_server,tr("server setting"));

    playout->addLayout(items_server);

    QHBoxLayout *btn = new QHBoxLayout(this);
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

void ServerParameterSetting::ok()
{
    QString serverIp = serverIpEdit->text();

    if (serverIp.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return;
    }

    if (serverIp.compare(GlobalVariable::server_ip) != 0)
    {
        QString sql = "update setting set attr_value='" + serverIp + "' where id = 9";
        mdb->updatasql(sql);
        GlobalVariable::server_ip = serverIp;
    }

    if (server_enable->isChecked() != GlobalVariable::server_enable)
    {
        if(server_enable->isChecked())
        {
            QString sql = "update setting set attr_value='1' where id = 14";
            mdb->updatasql(sql);
        }
        else
        {
            QString sql = "update setting set attr_value='0' where id = 14";
            mdb->updatasql(sql);
        }

        GlobalVariable::server_enable = server_enable->isChecked();
    }

    emit serverStateChange(server_enable->isChecked());
    this->close();
}

void ServerParameterSetting::cancel()
{
    this->close();
}
