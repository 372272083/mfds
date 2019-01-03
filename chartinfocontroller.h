#ifndef CHARTINFOCONTROLLER_H
#define CHARTINFOCONTROLLER_H

#include <QWidget>
#include <QDateTime>
#include <QMap>

class ChartWidget;
class SqliteDB;
class QGridLayout;
class QVBoxLayout;
class QLabel;

class ChartInfoController : public QWidget
{
    Q_OBJECT
public:
    explicit ChartInfoController(SqliteDB *db,QWidget *parent = 0);

    void setCurDevicePipe(QString,QString,QString,QString);
private:
    ChartWidget *chart;
    ChartWidget *chartFreq;

    QVBoxLayout *chartLayout;

    SqliteDB* m_db;
    QGridLayout *rightgrid;

    QString motor;
    QString device;
    QString pipe;
    QString deviceType;
    QString deviceModel;

    QString e_table_name;
    QString freq_table_name;

    QDateTime initTime;

    QVector<QLabel*> allAttributeLables;
    QMap<QString,QLabel*> valueAttributeLabels;

    QString graph_u_title;
    QString graph_i_title;

    QString graph_u_freq_title;
    QString graph_i_freq_title;

    QString graph_add_title;
    QString graph_speed_title;

    QString graph_add_freq_title;

    QString graph_temperature_title;

signals:

public slots:
    void timeUpdate();
};

#endif // CHARTINFOCONTROLLER_H
