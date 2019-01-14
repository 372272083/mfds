#include "motormanage.h"

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

#include "motoredit.h"

MotorManage::MotorManage(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Motor Manage"));
    this->setMinimumSize(400,300);

    this->setWindowIcon(QIcon(":/images/icon"));

    view = new QTableView(this);
    QList<QString> motorlist;
    motorlist << tr("code") << tr("name") << tr("type") << tr("bear type");

    motormodel = m_db->model("motor",motorlist);
    view->setModel(motormodel);

    view->setColumnHidden(0,true);
    view->setColumnHidden(5,true);
    //view->setColumnHidden(6,true);
    view->resizeColumnsToContents();

    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView *header = view->horizontalHeader();
    header->setStretchLastSection(true);

    //view->show();
    QPushButton *btnAdd = new QPushButton(tr("添加"), this);
    connect(btnAdd, SIGNAL(clicked()),this, SLOT(addmotor()));
    QPushButton *btnEdit = new QPushButton(tr("编辑"), this);
    connect(btnEdit, SIGNAL(clicked()),this, SLOT(editmotor()));
    QPushButton *btnDelete = new QPushButton(tr("删除"), this);
    connect(btnDelete, SIGNAL(clicked()),this, SLOT(deletemotor()));
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

void MotorManage::addmotor()
{
    MotorEdit *motoredit = new MotorEdit(motormodel,m_db);
    motoredit->setmode(-1);
    motoredit->setAttribute(Qt::WA_DeleteOnClose);
    motoredit->exec();
}

void MotorManage::editmotor()
{
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);

        QList<QString> values;
        QSqlRecord record = motormodel->record(index.row());
        values << record.value("mcode").toString()<<record.value("name").toString()<<record.value("motor_type").toString()<<record.value("bearing_type").toString();
        MotorEdit *motoredit = new MotorEdit(motormodel,m_db);
        motoredit->setmode(index.row());
        motoredit->setmodel(values);
        motoredit->setAttribute(Qt::WA_DeleteOnClose);
        motoredit->exec();
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

void MotorManage::deletemotor()
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
            motormodel->removeRow(row);
            motormodel->submitAll();
        }
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

MotorManage::~MotorManage()
{
    delete motormodel;
}
