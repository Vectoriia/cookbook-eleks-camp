#ifndef _SIDEBARVIEW_C_
#define _SIDEBARVIEW_C_

#include <QWidget>
namespace CookBook
{
    class SideBarView : public QWidget
    {
        Q_OBJECT
        public:
        explicit SideBarView(QWidget * parent = nullptr);

        private:
        Q_DISABLE_COPY(SideBarView)
    };
}

#endif