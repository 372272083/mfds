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
#include <QMutex>

class SqliteDB
{
public:
    SqliteDB(QString dbfile);

    bool reConnect();
    bool open();
    void close();
    bool status();
    bool createtable(QString sql);

    bool updatasql(QString sql);
    bool execSql(QQueue<QString> sqls);
    QVector<QString> execSql(QString sql,QString seperator=",");
    bool execSql(QString sql,QMap<QString,QVariantList> binds);

    QSqlTableModel* model(QString table,QStringList headers,QString filter="");
    QSqlTableModel* modelNoHeader(QString table);

    int queryCount(QString sql);

    QVector<QString> getWaveTimeList(TABLENAME info,QString dcode,QString pipe,QString start,QString end);
    QString queryVibrateMinDateTime();
    QVector<QString> queryWave(TABLENAME info,QString dcode,QString pipe,QString start,QString end);
    QString getWaveById(TABLENAME info,int);
    QVector<QString> getMeasureByInterval(TABLENAME info,QString,QString,QString,QString);
    QVector<QString> getFreqsByInterval(TABLENAME info,QString,QString,QString,QString,int);

    QString getConditionByCodeTime(QString,QString);
    QVector<QString> getMotorConditionTable();
    QString getMotorCondition(QString);

    QVector<QString> getStudyUniqes();
    QVector<QString> getVibrateStudyData();
    QVector<QString> getElectricStudyData();

    QVector<QString> getVibrateStudyResultData();
    QVector<QString> getElectricStudyResultData();

    QVector<QString> getVibrateRecordData(QString);
    QVector<QString> getElectricRecordData(QString);

    QString getProjectData();

private:
    QString dbfile;
    QSqlDatabase database;

    QMutex mutex;
};

#endif // SQLITEDB_H
