#ifndef SYNCDATAINFO_H
#define SYNCDATAINFO_H

#include <QDialog>

class QLabel;
class QProgressBar;
class QPushButton;
class QTimer;

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
    QTimer *timer;

public slots:
    void ok();
    void cancel();

    void timeUpdate();
};

#endif // SYNCDATAINFO_H
