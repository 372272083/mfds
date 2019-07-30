#ifndef RECORDWAVEWIDGET_H
#define RECORDWAVEWIDGET_H

#include <QDialog>
#include <QDateTime>

class QComboBox;
class QLineEdit;
class QProgressBar;
class QTimer;
class QPushButton;

class RecordWaveWidget: public QDialog
{
    Q_OBJECT
public:
    explicit RecordWaveWidget(QWidget *parent = 0);

private:
    QComboBox *intervalCombo;
    QLineEdit *pnameEdit;

    QProgressBar* pbar;
    QDateTime initTime;
    QTimer *timer;

    QPushButton *pcancelbtn;
    QPushButton *pokbtn;

    int recv_secs;

signals:
    void recorwWaveWithInfo(int interval,QString name);

public slots:
    void ok();
    void cancel();

    void timeUpdate();
};

#endif // RECORDWAVEWIDGET_H
