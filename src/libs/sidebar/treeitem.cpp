#include "treeitem.h"

TreeItem::TreeItem(const QString &id, const QString &name, TreeItem *parent)
    : m_id{id}, m_name{name}, m_parentItem(parent)
{
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    bool flag{true};
    for (const auto &i : m_childItems)
    {
        if (item->getId() == i->getId() && item->getId() != "")
            flag = false;
    }
    if(flag)
        m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return 1;
}

QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= 1)
        return QVariant();
    return m_name;
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem *>(this));

    return 0;
}

QString TreeItem::getId() const
{
    return m_id;
}

bool TreeItem::removeChild(const QString &id)
{
    bool flag{false};
    for (std::size_t i{0}; i < m_childItems.size(); ++i)
    {
        if (id == m_childItems[i]->getId())
        {
            m_childItems.removeAt(i);
            flag = true;
            break;
        }
    }
    return flag;
}