#ifndef PATHTRACKINGCHANNELSELECT_H
#define PATHTRACKINGCHANNELSELECT_H

#include <QDialog>
#include <constants.h>

class QComboBox;

class PathTrackingChannelSelect : public QDialog
{
    Q_OBJECT
public:
    explicit PathTrackingChannelSelect(QDialog *parent = 0);
    void setmode(int mode);
private:
    QComboBox* xAxisCB;
    QComboBox* yAxisCB;
    int mode;
signals:
    void channelSelected(int,int);
private slots:
    void ok();
    void cancel();
};

#endif // PATHTRACKINGCHANNELSELECT_H
