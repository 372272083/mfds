#include "mdisubwidget.h"

MDISubWidget::MDISubWidget(QFrame *parent) : QFrame(parent)
{

}

int MDISubWidget::getAUXId()
{
    return auxid;
}

void MDISubWidget::setAUXId(int id)
{
    this->auxid = id;
}

int MDISubWidget::getUniqueId()
{
    return id;
}

void MDISubWidget::setUniqueId(int id)
{
    this->id = id;
}

QString MDISubWidget::title()
{
    return "";
}

void MDISubWidget::setMcode(QString code)
{
    this->m_mcode = code;
}

QString MDISubWidget::getMcode()
{
    return m_mcode;
}
