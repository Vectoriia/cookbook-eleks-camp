#include "listmodel.h"

using namespace CookBook;

ItemModel::ItemModel()
{
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    int row = index.row();

    return m_list_a[row]->receiptName;
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    // ListItem *child = m_list_a.value(row);
    ListItem *child = m_list_a[row];

    if (child)
        return createIndex(row, column, child);
    else
        return QModelIndex();
}

int ItemModel::rowCount(const QModelIndex &parent) const
{
    return this->m_list_a.size();
}

void ItemModel::addItem(uint32_t id, const QString &name)
{
    ListItem *item = new ListItem(id, name);
    m_list_a.push_back(item);
}

void ItemModel::removeItem(uint32_t tid)
{
    ListItem *i{nullptr};
    m_list_a.erase(
        std::remove_if(m_list_a.begin(), m_list_a.end(),
                       [tid, &i](ListItem *item)
                       {
                           if (item->id == tid)
                           {
                               i = item;
                               return true;
                           }
                           else
                               return false;
                       }));
    if (i != nullptr)
    {
        delete i;
        i = nullptr;
    }
}
