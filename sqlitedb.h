#ifndef SQLITEDB_H
#define SQLITEDB_H

class QString;
class QStringList;
class QSqlTableModel;
class QueryInfo;

#include <QSqlDatabase>
#include "constants.h"
#include <QVector>
#include <QVariant>

class SqliteDB
{
public:
    SqliteDB(QString *dbfile);

    bool open();
    void close();
    bool status();
    bool createtable(QString sql);

    bool updatasql(QString sql);
    bool execSql(QQueue<QString> sqls);
    bool execSql(QString sql,QMap<QString,QVariantList> binds);

    QSqlTableModel* model(QString table,QStringList headers);
    QSqlTableModel* modelNoHeader(QString table);

    QVector<QueryInfo*> query(QString table,QString filter,TABLENAME info);
    int queryCount(QString sql);
private:
    QString *dbfile;
    QSqlDatabase database;
};

#endif // SQLITEDB_H
