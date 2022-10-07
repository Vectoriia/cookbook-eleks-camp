#ifndef _SIDEBAR_M_
#define _SIDEBAR_M_

#include <QTreeView>

namespace CookBook
{
    class ProxyReceiptsDB;
    class TreeModel;
}
namespace CookBook{
    class SideBar : public QWidget
    {
        Q_OBJECT
    public:
        explicit SideBar(ProxyReceiptsDB* db, QWidget * parent = nullptr);
        ~SideBar();
    
    public slots:
        void addToPreference(const QString &id, const QString &name);
        void removeFromPreference(const QString &id);
    
    signals:
        void displayView(const QString &id);

    protected:
        void setUpModel();

    private:
        QTreeView * m_tree;
        TreeModel * m_model;
        ProxyReceiptsDB * m_db;

    };
}
#endif
