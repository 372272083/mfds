#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QDialog>
class SqliteDB;
class QSqlTableModel;

class WaveForm : public QDialog
{
    Q_OBJECT
public:
    explicit WaveForm(SqliteDB *db,QWidget *parent = 0);
    ~WaveForm();
private:
    QSqlTableModel *beartypemodel;
    SqliteDB *m_db;
signals:

public slots:

};

#endif // WAVEFORM_H
