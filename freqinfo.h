#ifndef CHARGEFREQINFO_H
#define CHARGEFREQINFO_H

#include "queryinfo.h"

class FreqInfo : public QueryInfo
{
    Q_OBJECT
public:
    explicit FreqInfo(QueryInfo *parent = 0);
    FreqInfo(const FreqInfo&);
    FreqInfo& operator=(const FreqInfo &c);

    QString sample_freq;
    int stype;

    virtual QString toString();
signals:

public slots:
};

#endif // CHARGEFREQINFO_H
