#ifndef FAULTTYPEENGINE_H
#define FAULTTYPEENGINE_H

#include <QObject>
#include <QMap>

class FaultTypeEngine : public QObject
{
    Q_OBJECT
public:
    FaultTypeEngine(QObject *parent=0);
    QMap<int,QString> faults;
};

#endif // FAULTTYPEENGINE_H
