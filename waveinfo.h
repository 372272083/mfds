#ifndef WAVEINFO_H
#define WAVEINFO_H

#include "queryinfo.h"

class WaveInfo : public QueryInfo
{
    Q_OBJECT
public:
    explicit WaveInfo(QueryInfo *parent = 0);
    WaveInfo(const WaveInfo& c);
    WaveInfo& operator=(const WaveInfo &other);

    QString sample_data;
    int stype;

    int is_Continuity;

    virtual QString toString();

signals:

public slots:
};

#endif // WAVEINFO_H
