#ifndef MOTORINFO_H
#define MOTORINFO_H

#include <QString>

#include "treenodeinfo.h"
#include "constants.h"

class MotorInfo : public TreeNodeInfo
{
    Q_OBJECT
public:
    MotorInfo();

    int tree_id;
    QString name;
    QString code;
    QString work_mode;
    float power;
    float voltage;
    float current;
    float factor;
    QString insulate;
    float rotate;
    QString beartype;
    QString motortype;

    QString btype;
    float rin;
    float rout;
    float contact_angle;

    float pitch;
    float roller_d;
    int roller_n;
};

#endif // MOTORINFO_H
