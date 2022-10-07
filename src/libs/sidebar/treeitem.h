#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QList>


class TreeItem
{
public:
    //To meet interface requirments
    explicit TreeItem(const QString &id, const QString &name, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);
    bool removeChild(const QString &id);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parentItem();
    
    //To get id
    QString getId() const;


private:
    QString m_id;
    QString m_name;
    QList<TreeItem *> m_childItems;
    TreeItem *m_parentItem;
};

#endif