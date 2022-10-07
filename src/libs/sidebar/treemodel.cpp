#include <treemodel.h>
#include <treeitem.h>

#include <QStringList>
using namespace CookBook;

TreeModel::TreeModel(const QMap<QString, QString> &mainSection, const QMap<QString, QString> &prefSection, QObject *parent)
    : QAbstractItemModel(parent)
{
    // rootItem = new TreeItem("", "");

    // mp_cuisine = new TreeItem("", tr("Dishes:"), rootItem);
    // rootItem->appendChild(mp_cuisine);
    // setupModelData(mainSection, mp_cuisine);

    // mp_preference = new TreeItem("", tr("Preferences:"), rootItem);
    // rootItem->appendChild(mp_preference);
    // setupModelData(prefSection, mp_preference);
    setData(mainSection, prefSection);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem *>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem *>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem *>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModel::setupModelData(const QMap<QString, QString> &data, TreeItem *parent)
{
    for (auto iter = data.begin(); iter != data.end(); ++iter)
    {
        parent->appendChild(new TreeItem{iter.key(), iter.value(), parent});
    }
}

void TreeModel::appendToPreference(const QString &id, const QString &name)
{
    
    mp_preference->appendChild(new TreeItem(id, name, mp_preference));
}

void TreeModel::removeFromPreference(const QString &id)
{
    mp_preference->removeChild(id);
}

void TreeModel::setData(const QMap<QString, QString> &mainSection, const QMap<QString, QString> &prefSection)
{
    //TODO:Implementation should be added
    if(rootItem != nullptr)
        delete rootItem;
    rootItem = new TreeItem("", "");

    mp_cuisine = new TreeItem("", tr("Dishes:"), rootItem);
    rootItem->appendChild(mp_cuisine);
    setupModelData(mainSection, mp_cuisine);

    mp_preference = new TreeItem("", tr("Preferences:"), rootItem);
    rootItem->appendChild(mp_preference);
    setupModelData(prefSection, mp_preference);

    emit layoutChanged();
}