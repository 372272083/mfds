#ifndef MOTORTYPE_H
#define MOTORTYPE_H

#include <QDialog>
class SqliteDB;
class QSqlTableModel;
class QTableView;

class MotorType : public QDialog
{
    Q_OBJECT
public:
    explicit MotorType(SqliteDB *db,QWidget *parent = 0);
    ~MotorType();
private:
    QSqlTableModel *motortypemodel;
    QTableView* view;
    SqliteDB *m_db;
signals:

public slots:
    void addmotortype();
    void editmotortype();
    void deletemotortype();
};

#endif // MOTORTYPE_H
