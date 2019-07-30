#ifndef CMIEVANALYSETHREAD_H
#define CMIEVANALYSETHREAD_H

#include <QThread>

class CMIEVWaveInfo;

class CMIEVAnalyseThread : public QThread
{
    Q_OBJECT
public:
    CMIEVAnalyseThread();
    ~CMIEVAnalyseThread();
    void setData(CMIEVWaveInfo* wi);
    bool isAlive();
protected:
    void run();

private:
    CMIEVWaveInfo* wi;
    bool isLive;
};

#endif // CMIEVANALYSETHREAD_H
