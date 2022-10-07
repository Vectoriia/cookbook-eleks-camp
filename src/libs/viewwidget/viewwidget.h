#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ViewWidget; }
QT_END_NAMESPACE

namespace CookBook
{
class ViewWidget:public QWidget
{
    Q_OBJECT
public:
    ViewWidget(QWidget *parent);
    virtual ~ViewWidget();

public:
    bool getSaveStatus();
    void setSavedStatus(bool value);
    bool getEditableStatus();
    void setEditableStatus(bool value);

    void setTabIndex(unsigned value);
private:
    bool m_isSaved;
    bool m_isEditable;
protected:
    unsigned m_tab_index;
//unsigned m_tab_index;
    Ui::ViewWidget *ui;

};
}

#endif //VIEWWIDGET_H
