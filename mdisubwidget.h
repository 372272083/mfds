#ifndef MDISUBWIDGET_H
#define MDISUBWIDGET_H

#include <QFrame>
class MDISubWidget : public QFrame
{
    Q_OBJECT
public:
    MDISubWidget(QFrame *parent = 0);

    int getUniqueId();
    void setUniqueId(int id);
    int getAUXId();
    void setAUXId(int id);
    virtual QString title();

    void setMcode(QString code);
    QString getMcode();
private:
    int id;
    int auxid;
protected:
    QString m_mcode;
};

#endif // MDISUBWIDGET_H
