#ifndef SAVESAMPLETHREAD_H
#define SAVESAMPLETHREAD_H

#include <QThread>

class SqliteDB;
class SaveSampleThread : public QThread
{
    Q_OBJECT
public:
    SaveSampleThread(SqliteDB *db);
    void setState(bool state);

protected:
    void run();

private :
    SqliteDB* mdb;
    bool state;
};

#endif // SAVESAMPLETHREAD_H
