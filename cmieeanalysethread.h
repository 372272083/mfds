#ifndef CMIEEANALYSETHREAD_H
#define CMIEEANALYSETHREAD_H

#include <QThread>

class CMIEEWaveInfo;

class CMIEEAnalyseThread : public QThread
{
    Q_OBJECT
public:
    CMIEEAnalyseThread();
    ~CMIEEAnalyseThread();

    void setData(CMIEEWaveInfo* wi);
    bool isAlive();

protected:
    void run();

private:
    CMIEEWaveInfo* wi;
    bool isLive;
};

#endif // CMIEEANALYSETHREAD_H
