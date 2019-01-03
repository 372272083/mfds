#ifndef MOTORINFO_H
#define MOTORINFO_H

#include <QObject>
#include <QString>

class MotorInfo : public QObject
{
    Q_OBJECT
public:
    MotorInfo();

    QString name;
    QString code;
    QString work_mode;
    QString power;
    QString voltage;
    QString factor;
    QString insulate;
    QString rotate;
    QString beartype;
};

#endif // MOTORINFO_H
