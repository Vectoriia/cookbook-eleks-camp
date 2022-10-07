#include <viewwidget.h>
using namespace CookBook;

ViewWidget::ViewWidget(QWidget *parent) : QWidget(parent)
{
    m_isEditable = false;
    m_isSaved = true;
}
ViewWidget::~ViewWidget()
{
}
bool ViewWidget::getSaveStatus()
{
    return this->m_isSaved;
}
void ViewWidget::setSavedStatus(bool value)
{
    this->m_isSaved = value;
}
bool ViewWidget::getEditableStatus()
{
    return this->m_isEditable;
}
void ViewWidget::setEditableStatus(bool value)
{
    this->m_isEditable = value;
}
