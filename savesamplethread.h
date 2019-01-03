#ifndef SAVESAMPLETHREAD_H
#define SAVESAMPLETHREAD_H

#include <QThread>

class SqliteDB;
class SaveSampleThread : public QThread
{
    Q_OBJECT
public:
    SaveSampleThread(SqliteDB *db);
protected:
    void run();

private :
    SqliteDB* mdb;
};

#endif // SAVESAMPLETHREAD_H
