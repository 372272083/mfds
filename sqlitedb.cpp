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

#include <QMutexLocker>

SqliteDB::SqliteDB(QString dbfile_in):dbfile(dbfile_in)
{

}

bool SqliteDB::reConnect()
{
    QMutexLocker locker(&mutex);
    database.close();
    database = QSqlDatabase::addDatabase("QSQLITE");   //数据库驱动类型为SQL Server

    qDebug() << dbfile;
    database.setDatabaseName(dbfile);
    database.open();

    return database.isOpen();
}

bool SqliteDB::open()
{
    QMutexLocker locker(&mutex);
    if (database.isOpen())
    {
        return true;
    }
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(dbfile);
    database.open();

    return database.isOpen();
}

bool SqliteDB::createtable(QString sql)
{
    QMutexLocker locker(&mutex);

    database.transaction();

    QSqlQuery sql_query(database);
    sql_query.prepare(sql); //创建表
    //qDebug() << sql;
    if(!sql_query.exec()) //查看创建表是否成功
    {
        sql_query.clear();
        database.rollback();
        return false;
    }
    sql_query.clear();
    database.commit();

    return true;
}

bool SqliteDB::updatasql(QString sql)
{
    QMutexLocker locker(&mutex);

    if (!database.isOpen())
    {
        return false;
    }

    database.transaction();
    QSqlQuery sql_query(database);

    sql_query.prepare(sql); //创建表
    qDebug() << sql;
    if(!sql_query.exec()) //查看创建表是否成功
    {
        sql_query.clear();
        database.rollback();

        return false;
    }

    sql_query.clear();
    database.commit();

    return true;
}

bool SqliteDB::execSql(QString sql,QMap<QString,QVariantList> binds)
{
    QMutexLocker locker(&mutex);

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
    query.clear();
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

QVector<QString> SqliteDB::execSql(QString sql,QString seperator)
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;
    if (!database.isOpen())
    {
        return result;
    }

    database.transaction();
    QSqlQuery sql_query(database);

    sql_query.prepare(sql); //创建表
    if(!sql_query.exec()) //查看创建表是否成功
    {
        sql_query.clear();
        database.rollback();
        return result;
    }

    while(sql_query.next())
    {
        QSqlRecord record = sql_query.record();
        int count = record.count();

        QString row_str = "";
        for(int i=0;i<count;i++)
        {
            row_str += record.value(i).toString() + seperator;
        }
        result.append(row_str);
    }

    sql_query.clear();
    database.commit();

    return result;
}

bool SqliteDB::execSql(QQueue<QString> sqls)
{
    QMutexLocker locker(&mutex);

    try{
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
            //qDebug() << sql;
            flag = query.exec(sql);
            if (!flag)
            {
                break;
            }
        }
        query.clear();
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
    catch(QException e)
    {
        return false;
    }
}

QSqlTableModel* SqliteDB::model(QString table,QStringList headers,QString filter)
{
    QMutexLocker locker(&mutex);

    QSqlTableModel *model = new QSqlTableModel(nullptr,database);
    model->database().transaction();
    try
    {
        model->setTable(table);
        if(filter.length() == 0)
        {
            model->select();
        }
        else
        {
            model->setFilter(filter);
            model->select();
        }

        qDebug() << model->rowCount() << " " << table;

        QListIterator<QString> i(headers);
        int index = 1;
        while(i.hasNext())
        {
            model->setHeaderData(index,Qt::Horizontal, i.next());
            index++;
        }
    }
    catch(QException e)
    {

    }

    model->database().commit();

    return model;
}

QSqlTableModel* SqliteDB::modelNoHeader(QString table)
{
    QMutexLocker locker(&mutex);

    QSqlTableModel *model = new QSqlTableModel(nullptr,database);
    model->database().transaction();

    model->setTable(table);
    model->select();

    model->database().commit();

    return model;
}

int SqliteDB::queryCount(QString sql)
{
    QMutexLocker locker(&mutex);

    database.transaction();
    QSqlQuery sql_query(database);

    sql_query.prepare(sql); //创建表
    if(!sql_query.exec()) //查看创建表是否成功
    {
        sql_query.clear();
        database.rollback();
        return 0;
    }
    //sql_query.clear();
    int index = 0;
    while(sql_query.next())
    {
        index++;
    }

    sql_query.clear();
    database.commit();

    return index;
}

void SqliteDB::close()
{
    QMutexLocker locker(&mutex);
    database.close();
}

bool SqliteDB::status()
{
    return database.isOpen();
}

QVector<QString> SqliteDB::getWaveTimeList(TABLENAME info,QString dcode,QString pipe,QString start,QString end)
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery query(database);
    QString sql="";

    switch (info) {
    case CHARGEWAVEINFO:
        sql = "select id,rksj from electricchargewave where pipe=" + pipe + " and dcode='" + dcode + "' and rksj >'" + start + "' and rksj < '" + end + "' and stype=0";
        break;
    case CHARGEWAVEINFO_I:
        sql = "select id,rksj from electricchargewave where pipe=" + pipe + " and dcode='" + dcode + "' and rksj >'" + start + "' and rksj < '" + end + "' and stype=1";
        break;
    case VIBRATEWAVEINFO:
        sql = "select id,rksj from vibratewave where pipe=" + pipe + " and dcode='" + dcode + "' and rksj >'" + start + "' and rksj < '" + end + "'";
        break;
    default:
        break;
    }
    //qDebug()<< sql;

    query.prepare(sql); //创建表
    if(!query.exec()) //查看创建表是否成功
    {
        query.clear();
        database.rollback();
        return result;
    }
    while(query.next())
    {
        QSqlRecord rec = query.record();

        QString id = rec.value("id").toString();
        QString rksj = rec.value("rksj").toString();

        QString data = id + "," + rksj;
        data = data.trimmed();
        if(data.length()>0)
        {
            result.append(data);
        }
    }
    query.clear();
    database.commit();
    return result;
}

QVector<QString> SqliteDB::queryWave(TABLENAME info,QString dcode,QString pipe,QString start,QString end)
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery query(database);
    QString sql="";

    switch (info) {
    case CHARGEINFO:
        sql = "select sample_data from electricchargewave where pipe=" + pipe + " and dcode='" + dcode + "' and rksj >'" + start + "' and rksj < '" + end + "' order by rksj desc limit 1";
        break;
    case VIBRATEINFO:
        sql = "select sample_data from vibratewave where pipe=" + pipe + " and dcode='" + dcode + "' and rksj >'" + start + "' and rksj < '" + end + "' order by rksj desc limit 1";
        break;
    default:
        break;
    }
    qDebug()<< sql;

    query.prepare(sql); //创建表
    if(!query.exec()) //查看创建表是否成功
    {
        query.clear();
        database.rollback();
        return result;
    }
    while(query.next())
    {
        QString data = query.value(0).toString();
        data = data.trimmed();
        if(data.length()>0)
        {
            result.append(data);
            query.clear();
            database.commit();
            return result;
        }
    }

    query.clear();
    database.commit();
}

QString SqliteDB::queryVibrateMinDateTime()
{
    QMutexLocker locker(&mutex);

    QString sql = "select rksj from vibrate limit 1";

    database.transaction();
    QSqlQuery sql_query(database);

    sql_query.prepare(sql); //创建表
    if(!sql_query.exec()) //查看创建表是否成功
    {
        sql_query.clear();
        database.rollback();
        return "";
    }
    while(sql_query.next())
    {
        QString dt = sql_query.value(0).toString();
        dt = dt.trimmed();
        if(dt.length()>0)
        {
            sql_query.clear();
            database.commit();
            return dt;
        }
    }

    sql_query.clear();
    database.commit();

    return "";
}

QString SqliteDB::getWaveById(TABLENAME info,int id)
{
    QMutexLocker locker(&mutex);

    QString result = "";
    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select sample_data from electricchargewave";
    switch (info) {
    case CHARGEWAVEINFO:
        sql = "select sample_data from electricchargewave";
        break;
    case VIBRATEWAVEINFO:
        sql = "select sample_data from vibratewave";
        break;
    default:
        break;
    }
    sql += " where id=" + QString::number(id);
    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //

            QString data = rec.value("sample_data").toString();
            result = data;
            break;
        }
    }
    sql_query.clear();
    database.commit();

    return result;
}

QString SqliteDB::getConditionByCodeTime(QString dcode,QString f_t)
{
    QMutexLocker locker(&mutex);

    QString sql = "select sample,rotate,interval from condition where dcode='" + dcode + "' and rksj <='" + f_t + "' order by rksj desc limit 1";

    database.transaction();
    QSqlQuery sql_query(database);

    sql_query.prepare(sql); //创建表
    if(!sql_query.exec()) //查看创建表是否成功
    {
        sql_query.clear();
        database.rollback();
        return "";
    }
    while(sql_query.next())
    {
        QString dt = sql_query.value(0).toString() + "," + sql_query.value(1).toString() + "," + sql_query.value(2).toString();
        dt = dt.trimmed();
        if(dt.length()>0)
        {
            sql_query.clear();
            database.commit();
            return dt;
        }
    }

    sql_query.clear();
    database.commit();

    return "";
}

QString SqliteDB::getProjectData()
{
    QMutexLocker locker(&mutex);

    if (!database.isOpen())
    {
        return "";
    }

    bool apdata = false;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select mcode,name,motor_type,bearing_type,manufacture_date from motor";

    QString result = "1:";
    QVector<QString> mTypes,bTypes;
    if(sql_query.exec(sql)) //
    {
        apdata = false;
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            try{
                QString mcode_str = rec.value("mcode").toString();
                QString name_str = rec.value("name").toString();
                QString motor_type_str = rec.value("motor_type").toString();
                QString bearing_type_str = rec.value("bearing_type").toString();
                QString manufacture_date_str = rec.value("manufacture_date").toString();

                mTypes.append(motor_type_str);
                bTypes.append(bearing_type_str);
                result += mcode_str + "," + name_str + "," + motor_type_str + "," + bearing_type_str + "," + manufacture_date_str + ";";
                apdata = true;
            }
            catch(QException e)
            {

            }
        }
        if(apdata)
        {
            result = result.left(result.length()-1);
        }
    }
    result += "#";

    sql = "select code,name,dmodel,dtype,ipaddress,port from device";

    result += "2:";
    if(sql_query.exec(sql)) //
    {
        apdata = false;
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            try{
                QString code_str = rec.value("code").toString();
                QString name_str = rec.value("name").toString();
                QString dmodel_str = rec.value("dmodel").toString();
                QString dtype_str = rec.value("dtype").toString();
                QString ipaddress_str = rec.value("ipaddress").toString();
                QString port_str = rec.value("port").toString();

                result += code_str + "," + name_str + "," + dmodel_str + "," + dtype_str + "," + ipaddress_str + "," + port_str + ";";
                apdata = true;
            }
            catch(QException e)
            {

            }
        }
        if(apdata)
        {
            result = result.left(result.length()-1);
        }
    }
    result += "#";

    sql = "select model,work_mode,power_rating,rated_voltage,rated_current,poleNums,center_height,factor,insulate,rotate from motortype";
    result += "3:";
    if(sql_query.exec(sql)) //
    {
        apdata = false;
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            try{
                QString model_str = rec.value("model").toString();
                QString work_mode_str = rec.value("work_mode").toString();
                QString power_rating_str = rec.value("power_rating").toString();
                QString rated_voltage_str = rec.value("rated_voltage").toString();
                QString rated_current_str = rec.value("rated_current").toString();
                QString poleNums_str = rec.value("poleNums").toString();
                QString center_height_str = rec.value("center_height").toString();
                QString factor_str = rec.value("factor").toString();
                QString insulate_str = rec.value("insulate").toString();
                QString rotate_str = rec.value("rotate").toString();

                result += model_str + "," + work_mode_str + "," + power_rating_str + "," + rated_voltage_str + "," + rated_current_str + "," + poleNums_str + "," + center_height_str + "," + factor_str + "," + insulate_str + "," + rotate_str + ";";
                apdata = true;
            }
            catch(QException ex)
            {

            }
        }
        if(apdata)
        {
            result = result.left(result.length()-1);
        }
    }
    result += "#";

    sql = "select model,btype,rin,rout,contact_angle from bearingtype";
    result += "4:";
    if(sql_query.exec(sql)) //
    {
        apdata = false;
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            try{
                QString model_str = rec.value("model").toString();
                QString btype_str = rec.value("btype").toString();
                QString rin_str = rec.value("rin").toString();
                QString rout_str = rec.value("rout").toString();
                QString contact_angle_str = rec.value("contact_angle").toString();

                result += model_str + "," + btype_str + "," + rin_str + "," + rout_str + "," + contact_angle_str + ";";
                apdata = true;
            }
            catch(QException e)
            {

            }
        }
        if(apdata)
        {
            result = result.left(result.length()-1);
        }
    }
    result += "#";

    sql = "select model,dtype,pipenum,description from devicetype";
    result += "5:";
    if(sql_query.exec(sql)) //
    {
        apdata = false;
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            try{
                QString model_str = rec.value("model").toString();
                QString dtype_str = rec.value("dtype").toString();
                QString pipenum_str = rec.value("pipenum").toString();
                QString description_str = rec.value("description").toString();

                result += model_str + "," + dtype_str + "," + pipenum_str + "," + description_str + ";";
                apdata = true;
            }
            catch(QException e)
            {

            }
        }
        if(apdata)
        {
            result = result.left(result.length()-1);
        }
    }
    result += "#";

    sql = "select dcode,pipeCode,name,motor,isactive from devicepipes";
    result += "6:";
    if(sql_query.exec(sql)) //
    {
        apdata = false;
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            try{
                QString dcode_str = rec.value("dcode").toString();
                QString pipeCode_str = rec.value("pipeCode").toString();
                QString name_str = rec.value("name").toString();
                QString motor_str = rec.value("motor").toString();
                QString isactive_str = rec.value("isactive").toString();

                result += dcode_str + "," + pipeCode_str + "," + name_str + "," + motor_str + "," + isactive_str + ";";
                apdata = true;
            }
            catch(QException e)
            {

            }
        }
        if(apdata)
        {
            result = result.left(result.length()-1);
        }
    }
    result += "#";

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getFreqsByInterval(TABLENAME info,QString f_t,QString e_t,QString dcode,QString ch_code,int limit)
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select sample_freq,rksj from electricchargewavefreq";
    switch (info) {
    case CHARGEFREQINFO:
    case CHARGEFREQINFO_I:
        sql = "select sample_freq,rksj from electricchargewavefreq";
        break;
    case VIBRATEFREQINFO:
    case VIBRATEFREQINFO_S:
        sql = "select sample_freq,rksj from vibratewavefreq";
        break;
    default:
        break;
    }

    sql += " where rksj >= '" + f_t + "' and rksj <='" + e_t + "' and dcode='" + dcode + "' and pipe=" + ch_code;
    switch (info) {
    case CHARGEFREQINFO:
    case VIBRATEFREQINFO:
        sql += " and stype=0";
        break;
    case CHARGEFREQINFO_I:
    case VIBRATEFREQINFO_S:
        sql += " and stype=1";
        break;
    default:
        break;
    }

    qDebug() << sql;
    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //

            QString data = "";
            data = rec.value("sample_freq").toString() + "#" + rec.value("rksj").toString();
            if(result.size() < limit)
                result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getMeasureByInterval(TABLENAME info,QString f_t,QString e_t,QString dcode,QString ch_code)
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select u,i from electriccharge";
    switch (info) {
    case CHARGEINFO:
        sql = "select u,i from electriccharge";
        break;
    case VIBRATEINFO:
        sql = "select vibrate_e,speed_e from vibrate";
        break;
    case ACCFREQFEAT:
        sql = "select freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11 from accfreqfeat_record";
        break;
    case SPDFREQFEAT:
        sql = "select freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11 from spdfreqfeat_record";
        break;
    case VOLFREQFEAT:
        sql = "select freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11 from volfreqfeat_record";
        break;
    case CURFREQFEAT:
        sql = "select freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11 from curfreqfeat_record";
        break;
    default:
        break;
    }

    sql += " where rksj >= '" + f_t + "' and rksj <='" + e_t + "' and dcode='" + dcode + "' and pipe=" + ch_code;
    qDebug() << sql;
    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //

            QString data = "";
            switch (info) {
            case CHARGEINFO:
                data = rec.value("u").toString() + "," + rec.value("i").toString();
                break;
            case VIBRATEINFO:
                data = rec.value("vibrate_e").toString() + "," + rec.value("speed_e").toString();
                break;
            case ACCFREQFEAT:
            case SPDFREQFEAT:
            case VOLFREQFEAT:
            case CURFREQFEAT:
                data = rec.value("freq1").toString() + "," + rec.value("freq2").toString() + "," + rec.value("freq3").toString() \
                        + "," + rec.value("freq4").toString() + "," + rec.value("freq5").toString() + "," + rec.value("freq6").toString() \
                        + "," + rec.value("freq7").toString() + "," + rec.value("freq8").toString() + "," + rec.value("freq9").toString() \
                        + "," + rec.value("freq10").toString() + "," + rec.value("freq11").toString();
                break;
            default:
                break;
            }
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QString SqliteDB::getMotorCondition(QString mcode)
{
    QMutexLocker locker(&mutex);

    QString result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select edevice,vdevice,rotate,isManual,rksj from motorcondition where mcode='" + mcode + "' order by rksj desc limit 1";

    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            result = rec.value("edevice").toString() + "," + rec.value("vdevice").toString() + "," + rec.value("rotate").toString() + "," + rec.value("isManual").toString() + "," + rec.value("rksj").toString();
            break;
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getMotorConditionTable()
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select u,i,rotate,id,mcode from motorconditiontable";

    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            QString data = rec.value("id").toString() + "," + rec.value("u").toString() + "," + rec.value("i").toString() + "," + rec.value("rotate").toString() + "," + rec.value("mcode").toString();
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getStudyUniqes()
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select dcode,pipe,mcid from acctimefeat group by dcode,pipe,mcid";

    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            QString data = rec.value("dcode").toString() + "," + rec.value("pipe").toString() + "," + rec.value("mcid").toString();
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getVibrateStudyData()
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql ="select atf.amp as acc_amp,atf.rms as acc_rms,atf.pkpk as acc_pkpk,atf.waveindex as acc_waveindex,atf.peakindex as acc_peakindex,atf.pulsindex as acc_pulsindex,atf.kurtindex as acc_kurtindex,aff.freq1 as aff_f1, aff.freq2 as aff_f2,aff.freq3 as aff_f3,aff.freq4 as aff_f4,aff.freq5 as aff_f5,aff.freq6 as aff_f6,aff.freq7 as aff_f7,aff.freq8 as aff_f8,aff.freq9 as aff_f9,aff.freq10 as aff_f10,aff.freq11 as aff_f11,stf.amp as stf_amp,stf.rms as stf_rms,stf.pkpk as stf_pkpk,stf.waveindex as stf_waveindex,stf.peakindex as stf_peakindex,stf.pulsindex as stf_pulsindex,stf.kurtindex as stf_kurtindex,sff.freq1 as sff_f1, sff.freq2 as sff_f2,sff.freq3 as sff_f3,sff.freq4 as sff_f4,sff.freq5 as sff_f5,sff.freq6 as sff_f6,sff.freq7 as sff_f7,sff.freq8 as sff_f8,sff.freq9 as sff_f9,sff.freq10 as sff_f10,sff.freq11 as sff_f11,eft.freq1 as eft_f1,eft.freq2 as eft_f2,eft.freq3 as eft_f3,eft.freq4 as eft_f4,eft.freq5 as eft_f5,eft.freq6 as eft_f6,eft.freq7 as eft_f7,eft.freq8 as eft_f8,eft.freq9 as eft_f9,eft.freq10 as eft_f10,eft.freq11 as eft_f11,eft.freq12 as eft_f12,eft.freq13 as eft_f13,eft.freq14 as eft_f14,eft.freq15 as eft_f15,eft.freq16 as eft_f16,eft.freq17 as eft_f17,eft.freq18 as eft_f18,eft.freq19 as eft_f19,eft.freq20 as eft_f20,atf.dcode,atf.pipe,atf.mcid,atf.rksj from acctimefeat atf inner join accfreqfeat aff on atf.rksj=aff.rksj and atf.dcode=aff.dcode and atf.pipe=aff.pipe and atf.mcid=aff.mcid inner join spdtimefeat stf on atf.rksj=stf.rksj and atf.dcode=stf.dcode and atf.pipe=stf.pipe and atf.mcid=stf.mcid inner join spdfreqfeat sff on atf.rksj=sff.rksj and atf.dcode=sff.dcode and atf.pipe=sff.pipe and atf.mcid=sff.mcid inner join envfreqfeat eft on atf.rksj=eft.rksj and atf.dcode=eft.dcode and atf.pipe=eft.pipe and atf.mcid=eft.mcid";

    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            //acc_amp,acc_rms,acc_pkpk,acc_waveindex,acc_peakindex,acc_pulsindex,acc_kurtindex,aff_f1,aff_f2,aff_f3,aff_f4,aff_f5,aff_f6,aff_f7,aff_f8,aff_f9,

            //aff_f10,aff_f11,stf_amp,stf_rms,stf_pkpk,stf_waveindex,stf_peakindex,stf_pulsindex,stf_kurtindex,sff_f1, sff_f2,sff_f3,sff_f4,sff_f5,sff_f6,

            //sff_f7,sff_f8,sff_f9,sff_f10,sff_f11,dcode,pipe,mcid,rksj
            QString data = rec.value("acc_amp").toString() + "," + rec.value("acc_rms").toString() + "," + rec.value("acc_pkpk").toString() + "," + \
                    rec.value("acc_waveindex").toString() + "," + rec.value("acc_peakindex").toString() + "," + rec.value("acc_pulsindex").toString() + "," + \
                    rec.value("acc_kurtindex").toString() + "," + rec.value("aff_f1").toString() + "," + rec.value("aff_f2").toString() + "," + \
                    rec.value("aff_f3").toString() + "," + rec.value("aff_f4").toString() + "," + rec.value("aff_f5").toString() + "," + \
                    rec.value("aff_f6").toString() + "," + rec.value("aff_f7").toString() + "," + rec.value("aff_f8").toString() + "," + \
                    rec.value("aff_f9").toString() + "," + rec.value("aff_f10").toString() + "," + rec.value("aff_f11").toString() + "," + \
                    rec.value("stf_amp").toString() + "," + rec.value("stf_rms").toString() + "," + rec.value("stf_pkpk").toString() + "," + \
                    rec.value("stf_waveindex").toString() + "," + rec.value("stf_peakindex").toString() + "," + rec.value("stf_pulsindex").toString() + "," + \
                    rec.value("stf_kurtindex").toString() + "," + rec.value("sff_f1").toString() + "," + rec.value("sff_f2").toString() + "," + \
                    rec.value("sff_f3").toString() + "," + rec.value("sff_f4").toString() + "," + rec.value("sff_f5").toString() + "," + \
                    rec.value("sff_f6").toString() + "," + rec.value("sff_f7").toString() + "," + rec.value("sff_f8").toString() + "," + \
                    rec.value("sff_f9").toString() + "," + rec.value("sff_f10").toString() + "," + rec.value("sff_f11").toString() + "," + \

                    rec.value("eft_f1").toString() + "," + rec.value("eft_f2").toString() + "," + rec.value("eft_f3").toString() + "," + \
                    rec.value("eft_f4").toString() + "," + rec.value("eft_f5").toString() + "," + rec.value("eft_f6").toString() + "," + \
                    rec.value("eft_f7").toString() + "," + rec.value("eft_f8").toString() + "," + rec.value("eft_f9").toString() + "," + \
                    rec.value("eft_f10").toString() + "," + rec.value("eft_f11").toString() + "," + rec.value("eft_f12").toString() + "," + \
                    rec.value("eft_f13").toString() + "," + rec.value("eft_f14").toString() + "," + rec.value("eft_f15").toString() + "," + \
                    rec.value("eft_f16").toString() + "," + rec.value("eft_f17").toString() + "," + rec.value("eft_f18").toString() + "," + \
                    rec.value("eft_f19").toString() + "," + rec.value("eft_f20").toString() + "," + \

                    rec.value("dcode").toString() + "," + rec.value("pipe").toString() + "," + rec.value("mcid").toString() + "," + \
                    rec.value("rksj").toString();
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();
    return result;
}

QVector<QString> SqliteDB::getElectricStudyData()
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select atf.amp as acc_amp,atf.rms as acc_rms,atf.pkpk as acc_pkpk,aff.freq1 as aff_f1, aff.freq2 as aff_f2,aff.freq3 as aff_f3,aff.freq4 as aff_f4,aff.freq5 as aff_f5,aff.freq6 as aff_f6,aff.freq7 as aff_f7,aff.freq8 as aff_f8,aff.freq9 as aff_f9,aff.freq10 as aff_f10,aff.freq11 as aff_f11,stf.amp as stf_amp,stf.rms as stf_rms,stf.pkpk as stf_pkpk,sff.freq1 as sff_f1, sff.freq2 as sff_f2,sff.freq3 as sff_f3,sff.freq4 as sff_f4,sff.freq5 as sff_f5,sff.freq6 as sff_f6,sff.freq7 as sff_f7,sff.freq8 as sff_f8,sff.freq9 as sff_f9,sff.freq10 as sff_f10,sff.freq11 as sff_f11,atf.dcode,atf.pipe,atf.mcid,atf.rksj from voltimefeat atf inner join volfreqfeat aff on atf.rksj=aff.rksj and atf.dcode=aff.dcode and atf.pipe=aff.pipe and atf.mcid=aff.mcid inner join curtimefeat stf on atf.rksj=stf.rksj and atf.dcode=stf.dcode and atf.pipe=stf.pipe and atf.mcid=stf.mcid inner join curfreqfeat sff on atf.rksj=sff.rksj and atf.dcode=sff.dcode and atf.pipe=sff.pipe and atf.mcid=sff.mcid";

    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            QString data = rec.value("acc_amp").toString() + "," + rec.value("acc_rms").toString() + "," + rec.value("acc_pkpk").toString() + "," + \
                    rec.value("aff_f1").toString() + "," + rec.value("aff_f2").toString() + "," + \
                    rec.value("aff_f3").toString() + "," + rec.value("aff_f4").toString() + "," + rec.value("aff_f5").toString() + "," + \
                    rec.value("aff_f6").toString() + "," + rec.value("aff_f7").toString() + "," + rec.value("aff_f8").toString() + "," + \
                    rec.value("aff_f9").toString() + "," + rec.value("aff_f10").toString() + "," + rec.value("aff_f11").toString() + "," + \
                    rec.value("stf_amp").toString() + "," + rec.value("stf_rms").toString() + "," + rec.value("stf_pkpk").toString() + "," + \
                    rec.value("sff_f1").toString() + "," + rec.value("sff_f2").toString() + "," + \
                    rec.value("sff_f3").toString() + "," + rec.value("sff_f4").toString() + "," + rec.value("sff_f5").toString() + "," + \
                    rec.value("sff_f6").toString() + "," + rec.value("sff_f7").toString() + "," + rec.value("sff_f8").toString() + "," + \
                    rec.value("sff_f9").toString() + "," + rec.value("sff_f10").toString() + "," + rec.value("sff_f11").toString() + "," + \
                    rec.value("dcode").toString() + "," + rec.value("pipe").toString() + "," + rec.value("mcid").toString() + "," + \
                    rec.value("rksj").toString();
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getVibrateStudyResultData()
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select atf.amp as acc_amp,atf.rms as acc_rms,atf.pkpk as acc_pkpk,atf.waveindex as acc_waveindex,atf.peakindex as acc_peakindex,atf.pulsindex as acc_pulsindex,atf.kurtindex as acc_kurtindex,aff.freq1 as aff_f1, aff.freq2 as aff_f2,aff.freq3 as aff_f3,aff.freq4 as aff_f4,aff.freq5 as aff_f5,aff.freq6 as aff_f6,aff.freq7 as aff_f7,aff.freq8 as aff_f8,aff.freq9 as aff_f9,aff.freq10 as aff_f10,aff.freq11 as aff_f11,stf.amp as stf_amp,stf.rms as stf_rms,stf.pkpk as stf_pkpk,stf.waveindex as stf_waveindex,stf.peakindex as stf_peakindex,stf.pulsindex as stf_pulsindex,stf.kurtindex as stf_kurtindex,sff.freq1 as sff_f1, sff.freq2 as sff_f2,sff.freq3 as sff_f3,sff.freq4 as sff_f4,sff.freq5 as sff_f5,sff.freq6 as sff_f6,sff.freq7 as sff_f7,sff.freq8 as sff_f8,sff.freq9 as sff_f9,sff.freq10 as sff_f10,sff.freq11 as sff_f11,eft.freq1 as eft_f1,eft.freq2 as eft_f2,eft.freq3 as eft_f3,eft.freq4 as eft_f4,eft.freq5 as eft_f5,eft.freq6 as eft_f6,eft.freq7 as eft_f7,eft.freq8 as eft_f8,eft.freq9 as eft_f9,eft.freq10 as eft_f10,eft.freq11 as eft_f11,eft.freq12 as eft_f12,eft.freq13 as eft_f13,eft.freq14 as eft_f14,eft.freq15 as eft_f15,eft.freq16 as eft_f16,eft.freq17 as eft_f17,eft.freq18 as eft_f18,eft.freq19 as eft_f19,eft.freq20 as eft_f20,atf.dcode,atf.pipe,atf.mcid,atf.rksj from acctimefeat_result atf inner join accfreqfeat_result aff on atf.rksj=aff.rksj and atf.dcode=aff.dcode and atf.pipe=aff.pipe and atf.mcid=aff.mcid inner join spdtimefeat_result stf on atf.rksj=stf.rksj and atf.dcode=stf.dcode and atf.pipe=stf.pipe and atf.mcid=stf.mcid inner join spdfreqfeat_result sff on atf.rksj=sff.rksj and atf.dcode=sff.dcode and atf.pipe=sff.pipe and atf.mcid=sff.mcid inner join envfreqfeat_result eft on atf.rksj=eft.rksj and atf.dcode=eft.dcode and atf.pipe=eft.pipe and atf.mcid=eft.mcid";
    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            //acc_amp,acc_rms,acc_pkpk,acc_waveindex,acc_peakindex,acc_pulsindex,acc_kurtindex,aff_f1,aff_f2,aff_f3,aff_f4,aff_f5,aff_f6,aff_f7,aff_f8,aff_f9,

            //aff_f10,aff_f11,stf_amp,stf_rms,stf_pkpk,stf_waveindex,stf_peakindex,stf_pulsindex,stf_kurtindex,sff_f1, sff_f2,sff_f3,sff_f4,sff_f5,sff_f6,

            //sff_f7,sff_f8,sff_f9,sff_f10,sff_f11,dcode,pipe,mcid,rksj
            QString data = rec.value("acc_amp").toString() + "," + rec.value("acc_rms").toString() + "," + rec.value("acc_pkpk").toString() + "," + \
                    rec.value("acc_waveindex").toString() + "," + rec.value("acc_peakindex").toString() + "," + rec.value("acc_pulsindex").toString() + "," + \
                    rec.value("acc_kurtindex").toString() + "," + rec.value("aff_f1").toString() + "," + rec.value("aff_f2").toString() + "," + \
                    rec.value("aff_f3").toString() + "," + rec.value("aff_f4").toString() + "," + rec.value("aff_f5").toString() + "," + \
                    rec.value("aff_f6").toString() + "," + rec.value("aff_f7").toString() + "," + rec.value("aff_f8").toString() + "," + \
                    rec.value("aff_f9").toString() + "," + rec.value("aff_f10").toString() + "," + rec.value("aff_f11").toString() + "," + \
                    rec.value("stf_amp").toString() + "," + rec.value("stf_rms").toString() + "," + rec.value("stf_pkpk").toString() + "," + \
                    rec.value("stf_waveindex").toString() + "," + rec.value("stf_peakindex").toString() + "," + rec.value("stf_pulsindex").toString() + "," + \
                    rec.value("stf_kurtindex").toString() + "," + rec.value("sff_f1").toString() + "," + rec.value("sff_f2").toString() + "," + \
                    rec.value("sff_f3").toString() + "," + rec.value("sff_f4").toString() + "," + rec.value("sff_f5").toString() + "," + \
                    rec.value("sff_f6").toString() + "," + rec.value("sff_f7").toString() + "," + rec.value("sff_f8").toString() + "," + \
                    rec.value("sff_f9").toString() + "," + rec.value("sff_f10").toString() + "," + rec.value("sff_f11").toString() + "," + \

                    rec.value("eft_f1").toString() + "," + rec.value("eft_f2").toString() + "," + rec.value("eft_f3").toString() + "," + \
                    rec.value("eft_f4").toString() + "," + rec.value("eft_f5").toString() + "," + rec.value("eft_f6").toString() + "," + \
                    rec.value("eft_f7").toString() + "," + rec.value("eft_f8").toString() + "," + rec.value("eft_f9").toString() + "," + \
                    rec.value("eft_f10").toString() + "," + rec.value("eft_f11").toString() + "," + rec.value("eft_f12").toString() + "," + \
                    rec.value("eft_f13").toString() + "," + rec.value("eft_f14").toString() + "," + rec.value("eft_f15").toString() + "," + \
                    rec.value("eft_f16").toString() + "," + rec.value("eft_f17").toString() + "," + rec.value("eft_f18").toString() + "," + \
                    rec.value("eft_f19").toString() + "," + rec.value("eft_f20").toString() + "," + \

                    rec.value("dcode").toString() + "," + rec.value("pipe").toString() + "," + rec.value("mcid").toString() + "," + \
                    rec.value("rksj").toString();
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getElectricStudyResultData()
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select atf.amp as acc_amp,atf.rms as acc_rms,atf.pkpk as acc_pkpk,aff.freq1 as aff_f1, aff.freq2 as aff_f2,aff.freq3 as aff_f3,aff.freq4 as aff_f4,aff.freq5 as aff_f5,aff.freq6 as aff_f6,aff.freq7 as aff_f7,aff.freq8 as aff_f8,aff.freq9 as aff_f9,aff.freq10 as aff_f10,aff.freq11 as aff_f11,stf.amp as stf_amp,stf.rms as stf_rms,stf.pkpk as stf_pkpk,sff.freq1 as sff_f1, sff.freq2 as sff_f2,sff.freq3 as sff_f3,sff.freq4 as sff_f4,sff.freq5 as sff_f5,sff.freq6 as sff_f6,sff.freq7 as sff_f7,sff.freq8 as sff_f8,sff.freq9 as sff_f9,sff.freq10 as sff_f10,sff.freq11 as sff_f11,atf.dcode,atf.pipe,atf.mcid,atf.rksj from voltimefeat_result atf inner join volfreqfeat_result aff on atf.rksj=aff.rksj and atf.dcode=aff.dcode and atf.pipe=aff.pipe and atf.mcid=aff.mcid inner join curtimefeat_result stf on atf.rksj=stf.rksj and atf.dcode=stf.dcode and atf.pipe=stf.pipe and atf.mcid=stf.mcid inner join curfreqfeat_result sff on atf.rksj=sff.rksj and atf.dcode=sff.dcode and atf.pipe=sff.pipe and atf.mcid=sff.mcid";

    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            QString data = rec.value("acc_amp").toString() + "," + rec.value("acc_rms").toString() + "," + rec.value("acc_pkpk").toString() + "," + \
                    rec.value("aff_f1").toString() + "," + rec.value("aff_f2").toString() + "," + \
                    rec.value("aff_f3").toString() + "," + rec.value("aff_f4").toString() + "," + rec.value("aff_f5").toString() + "," + \
                    rec.value("aff_f6").toString() + "," + rec.value("aff_f7").toString() + "," + rec.value("aff_f8").toString() + "," + \
                    rec.value("aff_f9").toString() + "," + rec.value("aff_f10").toString() + "," + rec.value("aff_f11").toString() + "," + \
                    rec.value("stf_amp").toString() + "," + rec.value("stf_rms").toString() + "," + rec.value("stf_pkpk").toString() + "," + \
                    rec.value("sff_f1").toString() + "," + rec.value("sff_f2").toString() + "," + \
                    rec.value("sff_f3").toString() + "," + rec.value("sff_f4").toString() + "," + rec.value("sff_f5").toString() + "," + \
                    rec.value("sff_f6").toString() + "," + rec.value("sff_f7").toString() + "," + rec.value("sff_f8").toString() + "," + \
                    rec.value("sff_f9").toString() + "," + rec.value("sff_f10").toString() + "," + rec.value("sff_f11").toString() + "," + \
                    rec.value("dcode").toString() + "," + rec.value("pipe").toString() + "," + rec.value("mcid").toString() + "," + \
                    rec.value("rksj").toString();
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getVibrateRecordData(QString ts_t)
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select atf.amp as acc_amp,atf.rms as acc_rms,atf.pkpk as acc_pkpk,atf.waveindex as acc_waveindex,atf.peakindex as acc_peakindex,atf.pulsindex as acc_pulsindex,atf.kurtindex as acc_kurtindex,aff.freq1 as aff_f1, aff.freq2 as aff_f2,aff.freq3 as aff_f3,aff.freq4 as aff_f4,aff.freq5 as aff_f5,aff.freq6 as aff_f6,aff.freq7 as aff_f7,aff.freq8 as aff_f8,aff.freq9 as aff_f9,aff.freq10 as aff_f10,aff.freq11 as aff_f11,stf.amp as stf_amp,stf.rms as stf_rms,stf.pkpk as stf_pkpk,stf.waveindex as stf_waveindex,stf.peakindex as stf_peakindex,stf.pulsindex as stf_pulsindex,stf.kurtindex as stf_kurtindex,sff.freq1 as sff_f1, sff.freq2 as sff_f2,sff.freq3 as sff_f3,sff.freq4 as sff_f4,sff.freq5 as sff_f5,sff.freq6 as sff_f6,sff.freq7 as sff_f7,sff.freq8 as sff_f8,sff.freq9 as sff_f9,sff.freq10 as sff_f10,sff.freq11 as sff_f11,eft.freq1 as eft_f1,eft.freq2 as eft_f2,eft.freq3 as eft_f3,eft.freq4 as eft_f4,eft.freq5 as eft_f5,eft.freq6 as eft_f6,eft.freq7 as eft_f7,eft.freq8 as eft_f8,eft.freq9 as eft_f9,eft.freq10 as eft_f10,eft.freq11 as eft_f11,eft.freq12 as eft_f12,eft.freq13 as eft_f13,eft.freq14 as eft_f14,eft.freq15 as eft_f15,eft.freq16 as eft_f16,eft.freq17 as eft_f17,eft.freq18 as eft_f18,eft.freq19 as eft_f19,eft.freq20 as eft_f20,atf.dcode,atf.pipe,atf.rksj from acctimefeat_record atf inner join accfreqfeat_record aff on atf.rksj=aff.rksj and atf.dcode=aff.dcode and atf.pipe=aff.pipe inner join spdtimefeat_record stf on atf.rksj=stf.rksj and atf.dcode=stf.dcode and atf.pipe=stf.pipe inner join spdfreqfeat_record sff on atf.rksj=sff.rksj and atf.dcode=sff.dcode and atf.pipe=sff.pipe inner join envfreqfeat_record eft on atf.rksj=eft.rksj and atf.dcode=eft.dcode and atf.pipe=eft.pipe where atf.rksj='" + ts_t + "'";
    //qDebug() << sql;
    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            QString data = rec.value("acc_amp").toString() + "," + rec.value("acc_rms").toString() + "," + rec.value("acc_pkpk").toString() + "," + \
                    rec.value("acc_waveindex").toString() + "," + rec.value("acc_peakindex").toString() + "," + rec.value("acc_pulsindex").toString() + "," + \
                    rec.value("acc_kurtindex").toString() + "," + rec.value("aff_f1").toString() + "," + rec.value("aff_f2").toString() + "," + \
                    rec.value("aff_f3").toString() + "," + rec.value("aff_f4").toString() + "," + rec.value("aff_f5").toString() + "," + \
                    rec.value("aff_f6").toString() + "," + rec.value("aff_f7").toString() + "," + rec.value("aff_f8").toString() + "," + \
                    rec.value("aff_f9").toString() + "," + rec.value("aff_f10").toString() + "," + rec.value("aff_f11").toString() + "," + \
                    rec.value("stf_amp").toString() + "," + rec.value("stf_rms").toString() + "," + rec.value("stf_pkpk").toString() + "," + \
                    rec.value("stf_waveindex").toString() + "," + rec.value("stf_peakindex").toString() + "," + rec.value("stf_pulsindex").toString() + "," + \
                    rec.value("stf_kurtindex").toString() + "," + rec.value("sff_f1").toString() + "," + rec.value("sff_f2").toString() + "," + \
                    rec.value("sff_f3").toString() + "," + rec.value("sff_f4").toString() + "," + rec.value("sff_f5").toString() + "," + \
                    rec.value("sff_f6").toString() + "," + rec.value("sff_f7").toString() + "," + rec.value("sff_f8").toString() + "," + \
                    rec.value("sff_f9").toString() + "," + rec.value("sff_f10").toString() + "," + rec.value("sff_f11").toString() + "," + \

                    rec.value("eft_f1").toString() + "," + rec.value("eft_f2").toString() + "," + rec.value("eft_f3").toString() + "," + \
                    rec.value("eft_f4").toString() + "," + rec.value("eft_f5").toString() + "," + rec.value("eft_f6").toString() + "," + \
                    rec.value("eft_f7").toString() + "," + rec.value("eft_f8").toString() + "," + rec.value("eft_f9").toString() + "," + \
                    rec.value("eft_f10").toString() + "," + rec.value("eft_f11").toString() + "," + rec.value("eft_f12").toString() + "," + \
                    rec.value("eft_f13").toString() + "," + rec.value("eft_f14").toString() + "," + rec.value("eft_f15").toString() + "," + \
                    rec.value("eft_f16").toString() + "," + rec.value("eft_f17").toString() + "," + rec.value("eft_f18").toString() + "," + \
                    rec.value("eft_f19").toString() + "," + rec.value("eft_f20").toString() + "," + \

                    rec.value("dcode").toString() + "," + rec.value("pipe").toString() + "," + rec.value("rksj").toString();
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}

QVector<QString> SqliteDB::getElectricRecordData(QString ts_t)
{
    QMutexLocker locker(&mutex);

    QVector<QString> result;

    database.transaction();
    QSqlQuery sql_query(database);

    QString sql = "select atf.amp as acc_amp,atf.rms as acc_rms,atf.pkpk as acc_pkpk,aff.freq1 as aff_f1, aff.freq2 as aff_f2,aff.freq3 as aff_f3,aff.freq4 as aff_f4,aff.freq5 as aff_f5,aff.freq6 as aff_f6,aff.freq7 as aff_f7,aff.freq8 as aff_f8,aff.freq9 as aff_f9,aff.freq10 as aff_f10,aff.freq11 as aff_f11,stf.amp as stf_amp,stf.rms as stf_rms,stf.pkpk as stf_pkpk,sff.freq1 as sff_f1, sff.freq2 as sff_f2,sff.freq3 as sff_f3,sff.freq4 as sff_f4,sff.freq5 as sff_f5,sff.freq6 as sff_f6,sff.freq7 as sff_f7,sff.freq8 as sff_f8,sff.freq9 as sff_f9,sff.freq10 as sff_f10,sff.freq11 as sff_f11,atf.dcode,atf.pipe,atf.rksj from voltimefeat_record atf inner join volfreqfeat_record aff on atf.rksj=aff.rksj and atf.dcode=aff.dcode and atf.pipe=aff.pipe inner join curtimefeat_record stf on atf.rksj=stf.rksj and atf.dcode=stf.dcode and atf.pipe=stf.pipe inner join curfreqfeat_record sff on atf.rksj=sff.rksj and atf.dcode=sff.dcode and atf.pipe=sff.pipe where atf.rksj='" + ts_t + "'";

    bool success = sql_query.exec(sql);

    if(success)
    {
        while (sql_query.next()) {
            QSqlRecord rec = sql_query.record(); //
            QString data = rec.value("acc_amp").toString() + "," + rec.value("acc_rms").toString() + "," + rec.value("acc_pkpk").toString() + "," + \
                    rec.value("aff_f1").toString() + "," + rec.value("aff_f2").toString() + "," + \
                    rec.value("aff_f3").toString() + "," + rec.value("aff_f4").toString() + "," + rec.value("aff_f5").toString() + "," + \
                    rec.value("aff_f6").toString() + "," + rec.value("aff_f7").toString() + "," + rec.value("aff_f8").toString() + "," + \
                    rec.value("aff_f9").toString() + "," + rec.value("aff_f10").toString() + "," + rec.value("aff_f11").toString() + "," + \
                    rec.value("stf_amp").toString() + "," + rec.value("stf_rms").toString() + "," + rec.value("stf_pkpk").toString() + "," + \
                    rec.value("sff_f1").toString() + "," + rec.value("sff_f2").toString() + "," + \
                    rec.value("sff_f3").toString() + "," + rec.value("sff_f4").toString() + "," + rec.value("sff_f5").toString() + "," + \
                    rec.value("sff_f6").toString() + "," + rec.value("sff_f7").toString() + "," + rec.value("sff_f8").toString() + "," + \
                    rec.value("sff_f9").toString() + "," + rec.value("sff_f10").toString() + "," + rec.value("sff_f11").toString() + "," + \
                    rec.value("dcode").toString() + "," + rec.value("pipe").toString() + "," + rec.value("rksj").toString();
            result.append(data);
        }
    }

    sql_query.clear();
    database.commit();

    return result;
}
