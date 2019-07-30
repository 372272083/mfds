#ifndef TW888INFO_H
#define TW888INFO_H

#include "queryinfo.h"

class TW888Info : public QueryInfo
{
    Q_OBJECT
public:
    explicit TW888Info(QueryInfo *parent = 0);
    TW888Info(const TW888Info&);
    TW888Info& operator=(const TW888Info &other);

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

#endif // TW888INFO_H
