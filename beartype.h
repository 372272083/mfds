#ifndef BEARTYPE_H
#define BEARTYPE_H

#include <QDialog>
class SqliteDB;
class QSqlTableModel;
class QTableView;

class BearType : public QDialog
{
    Q_OBJECT
public:
    explicit BearType(SqliteDB *db,QWidget *parent = 0);
    ~BearType();
private:
    QSqlTableModel *beartypemodel;
    QTableView* view;
    SqliteDB *m_db;
signals:

public slots:
    void addbeartype();
    void editbeartype();
    void deletebeartype();
};

#endif // BEARTYPE_H
