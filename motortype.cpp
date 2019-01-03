#include "motortype.h"

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

#include "motortypeedit.h"

MotorType::MotorType(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Motor-Type Manage"));

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(1000,400);
    //this->setMaximumSize(1000,400);

    this->setWindowIcon(QIcon(":/images/icon"));

    view = new QTableView(this);

    QList<QString> motortypelist;
    motortypelist << tr("model") << tr("work mode") << tr("power(KW)") << tr("voltage(V)") << tr("current(A)") << tr("poles") << tr("centerh(mm)") << tr("factor") << tr("insulate") << tr("rotate(r/min)");
    motortypemodel = m_db->model("motortype",motortypelist);

    view->setModel(motortypemodel);

    view->setColumnHidden(0,true);

    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView *header = view->horizontalHeader();
    header->setStretchLastSection(true);

    //view->show();
    QPushButton *btnAdd = new QPushButton(tr("添加"), this);
    btnAdd->setMaximumWidth(100);
    connect(btnAdd, SIGNAL(clicked()),this, SLOT(addmotortype()));
    QPushButton *btnEdit = new QPushButton(tr("编辑"), this);
    btnEdit->setMaximumWidth(100);
    connect(btnEdit, SIGNAL(clicked()),this, SLOT(editmotortype()));
    QPushButton *btnDelete = new QPushButton(tr("删除"), this);
    btnDelete->setMaximumWidth(100);
    connect(btnDelete, SIGNAL(clicked()),this, SLOT(deletemotortype()));
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

void MotorType::addmotortype()
{
    MotortypeEdit *typeEdit = new MotortypeEdit(motortypemodel);
    typeEdit->setmode(-1);
    typeEdit->setAttribute(Qt::WA_DeleteOnClose);
    typeEdit->exec();
}

void MotorType::editmotortype()
{
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);

        QList<QString> values;
        QSqlRecord record = motortypemodel->record(index.row());
        values << record.value("model").toString()<<record.value("work_mode").toString()<<record.value("power_rating").toString()<<record.value("rated_voltage").toString()<<record.value("rated_current").toString()<<record.value("poleNums").toString()<<record.value("center_height").toString()<<record.value("factor").toString()<<record.value("insulate").toString()<<record.value("rotate").toString();
        MotortypeEdit *typeEdit = new MotortypeEdit(motortypemodel);
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

void MotorType::deletemotortype()
{
    qDebug() << "delete motor type";
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    qDebug() << "delete motor type 1";

    if (indexs.count() > 0)
    {
        qDebug() << "delete motor type 2";
        QModelIndex index = indexs.at(0);
        QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Are you sure delete this item?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
        {
            int row = index.row();
            motortypemodel->removeRow(row);
            motortypemodel->submitAll();
        }
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

MotorType::~MotorType()
{
    delete motortypemodel;
}
