#include "devicetype.h"

#include <QSqlTableModel>
#include <QTableView>
#include "sqlitedb.h"
#include <QList>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlRecord>

#include <QDebug>

#include <QMessageBox>

#include "devicetypeedit.h"

DeviceType::DeviceType(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Device Model"));
    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(500,300);
    //this->setMaximumSize(550,400);

    this->setWindowIcon(QIcon(":/images/icon"));

    view = new QTableView(this);

    QList<QString> devicetypelist;
    devicetypelist << tr("model") << tr("type") << tr("pipe number") << tr("description");
    devicetypemodel = m_db->model("devicetype",devicetypelist);

    view->setModel(devicetypemodel);

    view->setColumnHidden(0,true);

    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView *header = view->horizontalHeader();
    header->setStretchLastSection(true);

    //view->show();
    QPushButton *btnAdd = new QPushButton(tr("添加"), this);
    btnAdd->setMaximumWidth(100);
    connect(btnAdd, SIGNAL(clicked()),this, SLOT(adddevicetype()));
    QPushButton *btnEdit = new QPushButton(tr("编辑"), this);
    btnEdit->setMaximumWidth(100);
    connect(btnEdit, SIGNAL(clicked()),this, SLOT(editdevicetype()));
    QPushButton *btnDelete = new QPushButton(tr("删除"), this);
    btnDelete->setMaximumWidth(100);
    connect(btnDelete, SIGNAL(clicked()),this, SLOT(deletedevicetype()));
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnAdd,0,Qt::AlignRight);
    buttonLayout->addWidget(btnEdit,0,Qt::AlignRight);
    buttonLayout->addWidget(btnDelete,0,Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;
    //layout->setContentsMargins(10,10,10,10);
    layout->addWidget(view);
    layout->addLayout(buttonLayout);
    setLayout(layout);
}

void DeviceType::adddevicetype()
{
    DevicetypeEdit *typeEdit = new DevicetypeEdit(devicetypemodel);
    typeEdit->setmode(-1);
    typeEdit->setAttribute(Qt::WA_DeleteOnClose);
    typeEdit->exec();
}

void DeviceType::editdevicetype()
{
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);

        QList<QString> values;
        QSqlRecord record = devicetypemodel->record(index.row());
        values << record.value("model").toString()<<record.value("dtype").toString()<<record.value("pipenum").toString()<<record.value("description").toString();
        DevicetypeEdit *typeEdit = new DevicetypeEdit(devicetypemodel);
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

void DeviceType::deletedevicetype()
{
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);
        QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Are you sure delete this item?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
        {
            int row = index.row();
            devicetypemodel->removeRow(row);
            devicetypemodel->submitAll();
        }
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

DeviceType::~DeviceType()
{
    delete devicetypemodel;
    delete view;
}
