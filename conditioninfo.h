#ifndef CONDITIONINFO_H
#define CONDITIONINFO_H

#include "queryinfo.h"

class ConditionInfo : public QueryInfo
{
    Q_OBJECT
public:
    explicit ConditionInfo(QueryInfo *parent = 0);
    ConditionInfo(const ConditionInfo& c);
    ConditionInfo& operator=(const ConditionInfo &c);

    float rotate;
    float sample;
    float interval;

    virtual QString toString();

signals:

public slots:
};

#endif // CONDITIONINFO_H
