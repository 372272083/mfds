#include "waveform.h"

#include <QSqlTableModel>
#include "sqlitedb.h"
#include <QList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlRecord>

#include <QDebug>

#include <QMessageBox>

WaveForm::WaveForm(SqliteDB *db,QWidget *parent) : QDialog(parent),m_db(db)
{
    this->setWindowTitle(tr("Wave Analyse"));
    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(800,600);
    //this->setMaximumSize(550,400);

    this->setWindowIcon(QIcon(":/images/icon"));
}

WaveForm::~WaveForm()
{

}
