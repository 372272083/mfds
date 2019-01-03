#include "devicemanager.h"

#include <QSqlTableModel>
#include <QTableView>
#include "sqlitedb.h"
#include <QList>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlRecord>
#include <QDebug>

#include "deviceedit.h"

DeviceManager::DeviceManager(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Device Manage"));
    this->setMinimumSize(600,300);

    this->setWindowIcon(QIcon(":/images/icon"));

    view = new QTableView(this);
    QList<QString> devicelist;
    devicelist << tr("code") << tr("name") << tr("model") << tr("type") << tr("ip address") << tr("port") << tr("isactive");

    devicemodel = m_db->model("device",devicelist);
    view->setModel(devicemodel);

    view->setColumnHidden(0,true);
    view->resizeColumnsToContents();

    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView *header = view->horizontalHeader();
    header->setStretchLastSection(true);

    //view->show();
    QPushButton *btnAdd = new QPushButton(tr("添加"), this);
    connect(btnAdd, SIGNAL(clicked()),this, SLOT(adddevice()));
    QPushButton *btnEdit = new QPushButton(tr("编辑"), this);
    connect(btnEdit, SIGNAL(clicked()),this, SLOT(editdevice()));
    QPushButton *btnDelete = new QPushButton(tr("删除"), this);
    connect(btnDelete, SIGNAL(clicked()),this, SLOT(deletedevice()));
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnAdd);
    buttonLayout->addWidget(btnEdit);
    buttonLayout->addWidget(btnDelete);

    QVBoxLayout *layout = new QVBoxLayout;
    //layout->setContentsMargins(10,10,10,10);
    layout->addWidget(view);
    layout->addLayout(buttonLayout);
    setLayout(layout);
}

void DeviceManager::adddevice()
{
    DeviceEdit *deviceEdit = new DeviceEdit(devicemodel,m_db);
    deviceEdit->setmode(-1);
    deviceEdit->setAttribute(Qt::WA_DeleteOnClose);
    deviceEdit->exec();
}

void DeviceManager::editdevice()
{
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);

        QList<QString> values;
        QSqlRecord record = devicemodel->record(index.row());
        values << record.value("code").toString()<<record.value("name").toString()<<record.value("dmodel").toString()<<record.value("dtype").toString()<<record.value("ipaddress").toString()<<record.value("port").toString()<<record.value("isactive").toString();
        DeviceEdit *deviceEdit = new DeviceEdit(devicemodel,m_db);
        deviceEdit->setmode(index.row());
        deviceEdit->setmodel(values);
        deviceEdit->setAttribute(Qt::WA_DeleteOnClose);
        deviceEdit->exec();
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

void DeviceManager::deletedevice()
{
    //qDebug() << "delete motor type";
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    //qDebug() << "delete motor type 1";

    if (indexs.count() > 0)
    {
        qDebug() << "delete motor type 2";
        QModelIndex index = indexs.at(0);
        QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Are you sure delete this item?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
        {
            int row = index.row();
            QSqlRecord record = devicemodel->record(row);
            QString deviceCode = record.value("code").toString();

            devicemodel->removeRow(row);
            devicemodel->submitAll();

            QSqlTableModel *devicepipemodel = m_db->modelNoHeader("devicepipes");

            QString filter = "dcode='" + deviceCode + "'";
            devicepipemodel->setFilter(filter);
            if (devicepipemodel->select())
            {
                int mCount = devicepipemodel->rowCount();
                for(int n=0;n<mCount;n++)
                {
                    devicepipemodel->removeRow(n);
                }
                //devicepipemodel->removeRows(0,devicepipemodel->rowCount());
                devicepipemodel->submitAll();
            }
            delete devicepipemodel;
        }
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

DeviceManager::~DeviceManager()
{
    delete devicemodel;
}
