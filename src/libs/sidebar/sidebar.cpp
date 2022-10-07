#include <QDebug>

#include <sidebar.h>
#include <QVBoxLayout>
#include <treemodel.h>
#include <treeitem.h>
#include <database/proxyreceiptsDB.h>
#include <QHeaderView>
using namespace CookBook;

SideBar::SideBar(ProxyReceiptsDB *db, QWidget *parent)
    : QWidget(parent), m_tree(new QTreeView(this)), m_db(db)
{
    if (m_db == nullptr)
    {
        //TODO:write something to log
        qDebug() << "Cannot download a database\n";
        exit(1);
    }

    setUpModel();

    m_tree->setModel(m_model);
    m_tree->setHeaderHidden(true);

    m_tree->setMinimumSize(150, 0);
    m_tree->setStyleSheet("QTreeView{\n     background-color: #8B95C9;\n}");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_tree);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    connect(m_tree, &QTreeView::doubleClicked, this, [&](const QModelIndex &index)
            {
                auto item = static_cast<TreeItem *>(index.internalPointer());
                if (item->getId() != "")
                    emit displayView(item->getId());
            });

    connect(m_db, &ProxyReceiptsDB::dataBaseChanged, this, [this]()
            {
                std::vector<std::pair<std::string, std::string>> id_and_name_a = m_db->getNames();
                QMap<QString, QString> common_a{};
                for (const auto &item : id_and_name_a)
                {
                    common_a[item.first.c_str()] = (*(m_db->getName(item.first))).c_str();
                }

                std::vector<std::pair<std::string, bool>> id_and_pref_a = m_db->getPrefered();
                QMap<QString, QString> prefferd_a{};
                for (const auto &item : id_and_pref_a)
                {
                    if (item.second == 1)
                    {
                        prefferd_a[item.first.c_str()] = (*(m_db->getName(item.first))).c_str();
                    }
                }

                m_model->setData(common_a, prefferd_a);
            });
    connect(m_db, &ProxyReceiptsDB::prefChanged, this, [this](const std::string & id, bool flag)
    {
        if(flag == 1)
            addToPreference(QString::fromStdString(id), QString::fromStdString(*(m_db->getName(id))));
        else
            removeFromPreference(QString::fromStdString(id));
        
        emit m_model->layoutChanged();
    });
}

SideBar::~SideBar()
{
    delete m_model;
}

void SideBar::addToPreference(const QString &id, const QString &name)
{
    m_model->appendToPreference(id, name);
}

void SideBar::removeFromPreference(const QString &id)
{
    m_model->removeFromPreference(id);
}

void SideBar::setUpModel()
{
    std::vector<std::pair<std::string, std::string>> id_and_name_a = m_db->getNames();
    QMap<QString, QString> common_a{};
    for (const auto &item : id_and_name_a)
    {
        common_a[item.first.c_str()] = (*(m_db->getName(item.first))).c_str();
    }

    std::vector<std::pair<std::string, bool>> id_and_pref_a = m_db->getPrefered();
    QMap<QString, QString> prefferd_a{};
    for (const auto &item : id_and_pref_a)
    {
        if (item.second == 1)
        {
            prefferd_a[item.first.c_str()] = (*(m_db->getName(item.first))).c_str();
        }
    }

    m_model = new TreeModel{common_a, prefferd_a};

    emit m_model->layoutChanged();

}
