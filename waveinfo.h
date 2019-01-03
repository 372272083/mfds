#ifndef WAVEINFO_H
#define WAVEINFO_H

#include "queryinfo.h"

class WaveInfo : public QueryInfo
{
    Q_OBJECT
public:
    explicit WaveInfo(QObject *parent = 0);

    QString sample_data;
    int stype;

    virtual QString toString();

signals:

public slots:
};

#endif // WAVEINFO_H
