#ifndef FREQXSELECTDIALOG_H
#define FREQXSELECTDIALOG_H

#include <QDialog>
#include <constants.h>

class QListWidget;
class QListWidgetItem;
class QComboBox;

class FreqXSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FreqXSelectDialog(QDialog *parent = 0);
    void setMode(int);
private:
    int mode;
    QListWidget* listView;
    QComboBox* curTypeCB;
signals:
    void itemSelected(TREENODETYPE,QString);
private slots:
    void onItemDoubleClicked(QListWidgetItem*);
    void ok();
};

#endif // FREQXSELECTDIALOG_H
