#ifndef LISTMODEL_H
#define LISTMODEL_H


#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <memory>
#include <vector>
// #include "dialog.h"
namespace CookBook
{

    class ItemModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
        explicit ItemModel();

    public:
        QVariant data(const QModelIndex &index, int role) const override;
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        void addItem(uint32_t id, const QString &name);
        void removeItem(uint32_t id);
        
    public:
        struct ListItem
        {
            ListItem(uint32_t id_, const QString &name) : id(id_), receiptName(name)
            {
            }
            uint32_t id;
            QString receiptName;
        };
    private:
        // friend class DownloadDialog;
        std::vector<ListItem *> m_list_a;
    };
}
#endif // LISTMODEL_H