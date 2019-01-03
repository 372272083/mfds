#include "beartype.h"

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

#include "beartypeedit.h"

BearType::BearType(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Bear Manage"));
    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(550,400);
    //this->setMaximumSize(550,400);

    this->setWindowIcon(QIcon(":/images/icon"));

    view = new QTableView(this);

    QList<QString> beartypelist;
    beartypelist << tr("model") << tr("bear type") << tr("rin(mm)") << tr("rout(mm)") << tr("contact angles(°c)");
    beartypemodel = m_db->model("bearingtype",beartypelist);

    qDebug() << "data rows: " << beartypemodel->rowCount();

    view->setModel(beartypemodel);

    view->setColumnHidden(0,true);

    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView *header = view->horizontalHeader();
    header->setStretchLastSection(true);

    //view->show();
    QPushButton *btnAdd = new QPushButton(tr("添加"), this);
    btnAdd->setMaximumWidth(100);
    connect(btnAdd, SIGNAL(clicked()),this, SLOT(addbeartype()));
    QPushButton *btnEdit = new QPushButton(tr("编辑"), this);
    btnEdit->setMaximumWidth(100);
    connect(btnEdit, SIGNAL(clicked()),this, SLOT(editbeartype()));
    QPushButton *btnDelete = new QPushButton(tr("删除"), this);
    btnDelete->setMaximumWidth(100);
    connect(btnDelete, SIGNAL(clicked()),this, SLOT(deletebeartype()));
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

void BearType::addbeartype()
{
    BeartypeEdit *typeEdit = new BeartypeEdit(beartypemodel);
    typeEdit->setmode(-1);
    typeEdit->setAttribute(Qt::WA_DeleteOnClose);
    typeEdit->exec();
}

void BearType::editbeartype()
{
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);

        QList<QString> values;
        QSqlRecord record = beartypemodel->record(index.row());
        values << record.value("model").toString()<<record.value("btype").toString()<<record.value("rin").toString()<<record.value("rout").toString()<<record.value("contact_angle").toString();
        BeartypeEdit *typeEdit = new BeartypeEdit(beartypemodel);
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

void BearType::deletebeartype()
{
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);
        QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Are you sure delete this item?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
        {
            int row = index.row();
            beartypemodel->removeRow(row);
            beartypemodel->submitAll();
            view->updateGeometry();
        }
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

BearType::~BearType()
{
    delete beartypemodel;
    delete view;
}
