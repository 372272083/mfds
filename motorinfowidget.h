#ifndef MOTORINFOWIDGET_H
#define MOTORINFOWIDGET_H

#include <QWidget>
class SqliteDB;
class MotorInfo;
class QLabel;
class QComboBox;

class MotorInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MotorInfoWidget(SqliteDB *db, QWidget *parent = 0);
    ~MotorInfoWidget();
private:
    QList<MotorInfo*> motors;

    QComboBox *motorCombo;
    QLabel *nameEdit;
    QLabel *codeEdit;
    QLabel *workMEdit;
    QLabel *powerEdit;
    QLabel *voltageEdit;
    QLabel *factorEdit;
    QLabel *insulateEdit;
    QLabel *rotateEdit;
    QLabel *beartypeEdit;

signals:
    void motorChanged(QString motorCode);
    void deviceTypeChanged(QString deviceType);

public slots:
    void motorChangedCombo(const QString &text);
    void deviceTypeChangedCombo(const QString &text);
};

#endif // MOTORINFOWIDGET_H
