#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>


class TreeItem;

namespace CookBook{
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    //TODO: For each cuisine have several sections(dishTypes) in which receipts resides
    explicit TreeModel(const QMap<QString, QString> & mainSection, const QMap<QString, QString> & prefSection, QObject *parent = nullptr);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void setData(const QMap<QString, QString> & mainSection, const QMap<QString, QString> & prefSection);

public slots:
    void appendToPreference(const QString& id, const QString &name);
    void removeFromPreference(const QString &id);

private:
    void setupModelData(const QMap<QString, QString> &data, TreeItem *parent);
    //!Point to Preference row. Is owned by rootItem, which is relesed after destructor of the object is called.
    TreeItem *mp_preference{nullptr};
    //!Point to Cuisene row. Is owned by rootItem, which is relesed after destructor of the object is called.

    TreeItem *mp_cuisine{nullptr};
    TreeItem *rootItem{nullptr};
};
}

#endif // TREEMODEL_H