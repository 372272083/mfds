#include "databasemanager.h"

#include <QSqlTableModel>
#include <QTableView>
#include <QSqlRecord>
#include "sqlitedb.h"
#include <QList>
#include <QHeaderView>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QPushButton>
#include <QMessageBox>

#include <QStandardItemModel>

#include <QDebug>
#include <QDateTime>
#include <QQueue>

#include "globalvariable.h"

DatabaseManager::DatabaseManager(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Record Wave"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(400,500);
    this->setMaximumSize(400,500);

    listView = new QTableView();
    connect(listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotRowDoubleClicked(const QModelIndex &)));
    listView->setFixedHeight(420);

    QList<QString> datalist;
    datalist << tr("name") << tr("kssj");
    dataModel = m_db->model("waverecord",datalist);

    qDebug() << "data rows: " << dataModel->rowCount();

    listView->setModel(dataModel);

    listView->setColumnHidden(0,true);
    listView->setColumnHidden(3,true);
    listView->setColumnHidden(4,true);
    listView->setColumnHidden(5,true);

    listView->verticalHeader()->hide();

    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->setColumnWidth(1,120);
    listView->setColumnWidth(2,250);

    listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    listView->setSelectionMode(QAbstractItemView::SingleSelection);

    //view->show();
    QPushButton *btnEnter = new QPushButton(tr("Enter"), this);
    btnEnter->setMaximumWidth(100);
    connect(btnEnter, SIGNAL(clicked()),this, SLOT(enterWaves()));
    btnDelete = new QPushButton(tr("Delete"), this);
    btnDelete->setMaximumWidth(100);
    connect(btnDelete, SIGNAL(clicked()),this, SLOT(deleteWaves()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnEnter,0,Qt::AlignRight);
    buttonLayout->addWidget(btnDelete,0,Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;
    //layout->setContentsMargins(10,10,10,10);
    layout->addWidget(listView);
    layout->addLayout(buttonLayout);
    setLayout(layout);
}

void DatabaseManager::setmode(int m)
{
    if(0!=m)
    {
        btnDelete->setVisible(false);
    }
}

void DatabaseManager::enterWaves()
{
    QModelIndexList indexs = listView->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);

        QList<QString> values;
        QSqlRecord record = dataModel->record(index.row());

        QString id_str = record.value("id").toString();
        QString name_str = record.value("name").toString();
        QString kssj_str = record.value("kssj").toString();
        QString interval_str = record.value("interval").toString();

        bool tok;
        int interval_i = interval_str.toInt(&tok);

        QDateTime dt = QDateTime::fromString(kssj_str,GlobalVariable::dtFormat);
        QDateTime edt = dt.addSecs(interval_i);

        emit enterOfflineWithInterval(dt.toString(GlobalVariable::dtFormat),edt.toString(GlobalVariable::dtFormat),name_str);
        this->close();
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

void DatabaseManager::deleteWaves()
{
    QModelIndexList indexs = listView->selectionModel()->selectedRows();
    if (indexs.count() > 0)
    {
        QModelIndex index = indexs.at(0);
        QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Are you sure delete this item?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
        {
            int row = index.row();
            QSqlRecord record = dataModel->record(index.row());
            if(dataModel->removeRow(row))
            {
                if(dataModel->submitAll())
                {
                    QString sql = "";
                    QString kssj_str = record.value("kssj").toString();
                    QString interval_str = record.value("interval").toString();

                    bool tok;
                    int interval_i = interval_str.toInt(&tok);

                    QDateTime dt = QDateTime::fromString(kssj_str,GlobalVariable::dtFormat);
                    QDateTime edt = dt.addSecs(interval_i);

                    QString dt_s = dt.toString(GlobalVariable::dtFormat);
                    QString edt_s = edt.toString(GlobalVariable::dtFormat);
                    sql = "delete from electriccharge where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";

                    QQueue<QString> sqls;
                    sqls.enqueue(sql);

                    sql = "delete from electricchargewavefreq where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from electricchargewave where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from vibrate where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from vibratewave where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from vibratewavefreq where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from acctimefeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from accfreqfeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from spdtimefeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from spdfreqfeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from voltimefeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from volfreqfeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from curtimefeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from curfreqfeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    sql = "delete from envfreqfeat_record where rksj>='" + dt_s + "' and rksj <='" + edt_s + "'";
                    sqls.enqueue(sql);

                    if(m_db->execSql(sqls))
                    {

                    }
                    else
                    {
                        QMessageBox::information(this, tr("Infomation"), tr("Delete data failed!"));
                        return;
                    }
                }
            }
            listView->updateGeometry();
        }
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please Select a item!"));
        return;
    }
}

void DatabaseManager::slotRowDoubleClicked(const QModelIndex &index)
{
    QSqlRecord record = dataModel->record(index.row());

    QString id_str = record.value("id").toString();
    QString name_str = record.value("name").toString();
    QString kssj_str = record.value("kssj").toString();
    QString interval_str = record.value("interval").toString();

    bool tok;
    int interval_i = interval_str.toInt(&tok);

    QDateTime dt = QDateTime::fromString(kssj_str,GlobalVariable::dtFormat);
    QDateTime edt = dt.addSecs(interval_i);

    emit enterOfflineWithInterval(dt.toString(GlobalVariable::dtFormat),edt.toString(GlobalVariable::dtFormat),name_str);
    this->close();
}

DatabaseManager::~DatabaseManager()
{
    delete dataModel;
}
