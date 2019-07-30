#ifndef FAULTINFO_H
#define FAULTINFO_H

#include <QObject>
#include <QString>
#include <QVector>

#include "mfdslib/IO_Param.h"

class FaultInfo : public QObject
{
    Q_OBJECT
public:
    FaultInfo(QObject *parent = 0);
    FaultInfo(const FaultInfo&);

    QString dcode;
    QString channel;
    QVector<int> fault_codes;

    SDiaCriterion fault_detail_thd;
    SDiaCriterion fault_detail_cur;
    SDiaCriterion fault_detail_cof;
    SDiaCriterion fault_detail_trigger;

    QString sample_time;

    QString toString();

    FaultInfo& operator =(const FaultInfo&);
};

#endif // FAULTINFO_H
