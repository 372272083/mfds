#ifndef CHARGEINFO_H
#define CHARGEINFO_H

#include "queryinfo.h"

class ChargeInfo : public QueryInfo
{
    Q_OBJECT
public:
    explicit ChargeInfo(QueryInfo *parent = 0);
    ChargeInfo(const ChargeInfo&);
    ChargeInfo& operator=(const ChargeInfo &other);

    float u;
    float i;
    float f;
    float factor;
    float p;
    float q;
    float s;
    QString others;
    QString pqs;

    virtual QString toString();

signals:

public slots:
};

#endif // CHARGEINFO_H
