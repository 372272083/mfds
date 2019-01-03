#include "devicepipe.h"

#include <QSqlTableModel>
#include <QTableView>
#include "sqlitedb.h"
#include <QComboBox>
#include <QList>
#include <QLabel>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlRecord>
#include <QDebug>

#include "devicepipeedit.h"

DevicePipe::DevicePipe(SqliteDB* db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Device Pipe Manage"));
    this->setMinimumSize(450,300);

    this->setWindowIcon(QIcon(":/images/icon"));

    view = new QTableView(this);
    QList<QString> devicepipelist;
    devicepipelist << tr("dcode") << tr("pipecode") << tr("name") << tr("motor") << tr("isactive");

    devicepipemodel = m_db->model("devicepipes",devicepipelist);
    view->setModel(devicepipemodel);

    devicemodel = m_db->modelNoHeader("device");
    motormodel = m_db->modelNoHeader("motor");

    view->setColumnHidden(0,true);
    view->resizeColumnsToContents();

    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView *header = view->horizontalHeader();
    header->setStretchLastSection(true);

    //view->show();
    QPushButton *btnEdit = new QPushButton(tr("编辑"), this);
    connect(btnEdit, SIGNAL(clicked()),this, SLOT(editpipe()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnEdit);

    QVBoxLayout *layout = new QVBoxLayout;
    //layout->setContentsMargins(10,10,10,10);

    QHBoxLayout *header_select = new QHBoxLayout();
    QLabel *motorfilter = new QLabel();
    motorfilter->setText(tr("current motor:"));
    header_select->addWidget(motorfilter);
    motorSelect = new QComboBox();
    header_select->addWidget(motorSelect);

    int tmpRow = motormodel->rowCount();

    motorSelect->addItem(tr("please select a motor"),"0");
    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = motormodel->record(i);
        motorSelect->addItem(record.value("name").toString(),record.value("mcode").toString());
    }
    //connect(dmodelCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(modelValueChanged(const QString &)));
    connect(motorSelect,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(motorSelectChanged(const QString &)));

    QLabel *devicefilter = new QLabel();
    devicefilter->setText(tr("current device:"));
    header_select->addWidget(devicefilter);
    devicetypeSelect = new QComboBox();
    header_select->addWidget(devicetypeSelect);

    tmpRow = devicemodel->rowCount();

    devicetypeSelect->addItem(tr("please select a device"),"0");
    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = devicemodel->record(i);
        devicetypeSelect->addItem(record.value("name").toString(),record.value("code").toString());
    }

    connect(devicetypeSelect,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(deviceSelectChanged(const QString &)));

    header_select->addStretch();
    layout->addLayout(header_select);
    layout->addWidget(view);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    motorSelect->setCurrentIndex(0);
    devicetypeSelect->setCurrentIndex(0);
}

void DevicePipe::editpipe()
{
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);

        QList<QString> values;
        QSqlRecord record = devicepipemodel->record(index.row());
        values << record.value("dcode").toString()<<record.value("pipeCode").toString()<<record.value("name").toString()<<record.value("motor").toString()<<record.value("isactive").toString();
        DevicePipeEdit *typeEdit = new DevicePipeEdit(devicepipemodel,m_db);
        typeEdit->setmode(index.row());
        typeEdit->setmodel(values);
        typeEdit->setAttribute(Qt::WA_DeleteOnClose);
        typeEdit->exec();
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

void DevicePipe::motorSelectChanged(const QString &text)
{
    QString mcode = motorSelect->itemData(motorSelect->currentIndex()).toString();
    QString code = devicetypeSelect->itemData(devicetypeSelect->currentIndex()).toString();
    if (code == "0" && mcode == "0")
    {
        devicepipemodel->setFilter("id>0");
        devicepipemodel->select();
        return;
    }

    QString filter ="";

    if (code != "0")
    {
        filter = "dcode = '" + code + "'";
    }

    if (mcode != "0")
    {
        if (filter.length()>0)
        {
            filter += " and motor = '" + mcode + "'";
        }
        else
        {
            filter = "motor = '" + mcode + "'";
        }
    }

    devicepipemodel->setFilter(filter);
    devicepipemodel->select();
}

void DevicePipe::deviceSelectChanged(const QString &text)
{
    QString mcode = motorSelect->itemData(motorSelect->currentIndex()).toString();
    QString code = devicetypeSelect->itemData(devicetypeSelect->currentIndex()).toString();
    if (code == "0" && mcode == "0")
    {
        devicepipemodel->setFilter("id>0");
        devicepipemodel->select();
        return;
    }

    QString filter ="";

    if (code != "0")
    {
        filter = "dcode = '" + code + "'";
    }

    if (mcode != "0")
    {
        if (filter.length()>0)
        {
            filter += " and motor = '" + mcode + "'";
        }
        else
        {
            filter = "motor = '" + mcode + "'";
        }
    }

    devicepipemodel->setFilter(filter);
    devicepipemodel->select();
}

DevicePipe::~DevicePipe()
{
    delete devicemodel;
    delete devicepipemodel;
    delete motormodel;
}
