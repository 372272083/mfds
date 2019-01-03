#ifndef DEVICETYPE_H
#define DEVICETYPE_H

#include <QDialog>
class SqliteDB;
class QSqlTableModel;
class QTableView;

class DeviceType : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceType(SqliteDB *db,QWidget *parent = 0);
    ~DeviceType();
private:
    QSqlTableModel *devicetypemodel;
    QTableView* view;
    SqliteDB *m_db;
signals:

public slots:
    void adddevicetype();
    void editdevicetype();
    void deletedevicetype();
};

#endif // DEVICETYPE_H
