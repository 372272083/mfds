#ifndef SYNCDATAINFO_H
#define SYNCDATAINFO_H

#include <QDialog>

class QLabel;
class QProgressBar;
class QPushButton;

class SyncDataInfo : public QDialog
{
    Q_OBJECT
public:
    explicit SyncDataInfo(QWidget *parent = 0);

private:
    QLabel* info;
    QProgressBar* pecentage;
    QPushButton* sync_btn;
    QPushButton* cancel_sync;

public slots:
    void ok();
    void cancel();

};

#endif // SYNCDATAINFO_H
