#ifndef DEVICEPIPE_H
#define DEVICEPIPE_H

#include <QDialog>
class SqliteDB;
class QSqlTableModel;
class QTableView;
class QComboBox;

class DevicePipe : public QDialog
{
    Q_OBJECT
public:
    explicit DevicePipe(SqliteDB* db,QWidget *parent = 0);
    ~DevicePipe();

private:
    SqliteDB* m_db;
    QTableView* view;

    QSqlTableModel *motormodel;
    QSqlTableModel *devicemodel;
    QSqlTableModel *devicepipemodel;

    QComboBox *devicetypeSelect;
    QComboBox *motorSelect;

    bool devicetypechanging;

signals:

public slots:
    void motorSelectChanged(const QString &text);
    void deviceSelectChanged(const QString &text);

    void editpipe();
};

#endif // DEVICEPIPE_H
