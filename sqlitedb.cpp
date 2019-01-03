#include "sqlitedb.h"

#include <QSql>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QVariantList>
#include <QSqlTableModel>

#include <QStringList>
#include <QDebug>
#include <QVector>
#include <QDateTime>
#include <QException>
#include <QQueue>

#include "chargeinfo.h"
#include "vibrateinfo.h"

SqliteDB::SqliteDB(QString *dbfile_in):dbfile(dbfile_in)
{

}

bool SqliteDB::open()
{
    if (database.isOpen())
    {
        return true;
    }
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(*dbfile);
    database.open();

    return database.isOpen();
}

bool SqliteDB::createtable(QString sql)
{
    QSqlQuery sql_query;

    sql_query.prepare(sql); //创建表
    if(!sql_query.exec()) //查看创建表是否成功
    {
        return false;
    }

    return true;
}

bool SqliteDB::updatasql(QString sql)
{
    QSqlQuery sql_query;

    sql_query.prepare(sql); //创建表
    if(!sql_query.exec()) //查看创建表是否成功
    {
        return false;
    }

    return true;
}

bool SqliteDB::execSql(QString sql,QMap<QString,QVariantList> binds)
{
    if (!database.isOpen())
    {
        return false;
    }
    database.transaction();
    QSqlQuery query(database);
    query.prepare(sql);

    QList<QString> keys = binds.keys();
    int len = keys.size();
    for(int i=0;i<len;i++)
    {
        QString bindName = ":"+keys[i];
        query.bindValue(bindName,binds[keys[i]]);
    }
    bool flag = true;
    if(!query.execBatch())
    {
        qDebug() << query.lastError().text();
        flag = false;
    }
    if(!flag)
    {
        database.rollback();
    }
    else
    {
        database.commit();
    }
    return flag;
}

bool SqliteDB::execSql(QQueue<QString> sqls)
{
    if (!database.isOpen())
    {
        return false;
    }
    database.transaction();
    QSqlQuery query(database);
    bool flag = true;
    while(sqls.size()>0)
    {
        QString sql = sqls.dequeue();
        flag = query.exec(sql);
        if (!flag)
        {
            break;
        }
    }
    if(!flag)
    {
        database.rollback();
    }
    else
    {
        database.commit();
    }
    return flag;
}

QSqlTableModel* SqliteDB::model(QString table,QStringList headers)
{
    QSqlTableModel *model = new QSqlTableModel(nullptr,database);
    model->setTable(table);
    model->select();
    qDebug() << model->rowCount() << " " << table;

    QListIterator<QString> i(headers);
    int index = 1;
    while(i.hasNext())
    {
        model->setHeaderData(index,Qt::Horizontal, i.next());
        index++;
    }

    return model;
}

QSqlTableModel* SqliteDB::modelNoHeader(QString table)
{
    QSqlTableModel *model = new QSqlTableModel(nullptr,database);
    model->setTable(table);
    model->select();

    return model;
}

QVector<QueryInfo*> SqliteDB::query(QString table,QString filter,TABLENAME info)
{
    QVector<QueryInfo*> infoVect;

    QSqlTableModel *model = new QSqlTableModel;
    model->setTable(table);
    model->setFilter(filter);
    model->select();

    int rowCount = model->rowCount();
    bool tok;
    switch (info) {
    case CHARGEINFO:
        for(int i=0;i<rowCount;i++)
        {
            QSqlRecord record = model->record(i);
            ChargeInfo *tmp = new ChargeInfo();
            tmp->id = record.value("id").toInt(&tok);
            tmp->u = record.value("u").toFloat(&tok);
            tmp->i = record.value("i").toFloat(&tok);
            tmp->f = record.value("f").toFloat(&tok);
            tmp->factor = record.value("factor").toFloat(&tok);
            tmp->p = record.value("p").toFloat(&tok);
            tmp->q = record.value("q").toFloat(&tok);
            tmp->s = record.value("s").toFloat(&tok);
            tmp->others = record.value("others").toString();
            tmp->pqs = record.value("pqs").toString();
            tmp->dcode = record.value("dcode").toString();
            tmp->mcode = record.value("mcode").toString();
            tmp->pipe = record.value("pipe").toInt(&tok);
            tmp->rksj = record.value("rksj").toString();

            infoVect.push_back(tmp);   //将查询到的内容存到testInfo向量中
        }
        break;
    case VIBRATEINFO:
        for(int i=0;i<rowCount;i++)
        {
            QSqlRecord record_v = model->record(i);
            VibrateInfo *tmp_v = new VibrateInfo();
            tmp_v->id = record_v.value("id").toInt(&tok);
            tmp_v->vibrate_e = record_v.value("vibrate_e").toFloat(&tok);
            tmp_v->speed_e = record_v.value("speed_e").toFloat(&tok);
            tmp_v->dcode = record_v.value("dcode").toString();
            tmp_v->mcode = record_v.value("mcode").toString();
            tmp_v->pipe = record_v.value("pipe").toInt(&tok);
            tmp_v->rksj = record_v.value("rksj").toString();

            infoVect.push_back(tmp_v);   //将查询到的内容存到testInfo向量中
        }
        break;
    case TEMPERATUREINFO:
        break;
    default:
        break;
    }

    delete model;
    return infoVect;
}

int SqliteDB::queryCount(QString sql)
{
    QSqlQuery sql_query;

    sql_query.prepare(sql); //创建表
    if(!sql_query.exec()) //查看创建表是否成功
    {
        return 0;
    }

    return sql_query.size();
}

void SqliteDB::close()
{
    database.close();
}

bool SqliteDB::status()
{
    return database.isOpen();
}
