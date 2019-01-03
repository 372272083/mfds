#ifndef CHARGEFREQINFO_H
#define CHARGEFREQINFO_H

#include "queryinfo.h"

class FreqInfo : public QueryInfo
{
    Q_OBJECT
public:
    explicit FreqInfo(QObject *parent = 0);

    QString sample_freq;
    int stype;

    virtual QString toString();
signals:

public slots:
};

#endif // CHARGEFREQINFO_H
