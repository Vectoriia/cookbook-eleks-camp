#include <searchdialog.h>
#include <database/proxyreceiptsDB.h>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>

using namespace CookBook;

SearchDialog::SearchDialog(QWidget *parent, ProxyReceiptsDB *db) : QDialog(parent), m_db{db}
{
    setUpSideBar();
    this->setWindowTitle("Find");
    connect(m_find_b, &QPushButton::released, this, [this]
            {  
                bool res = find();
                if(!res)
                    QMessageBox::information(this, "CookBook", "Oops. Nothing has been found for the options you provided."); });
    connect(m_open_Fileb, &QPushButton::released, this, &SearchDialog::openFile);

    if (m_db == nullptr)
    { // TODO: write something to log
        qDebug() << "Some External Problem\n. The database was not initialized\n";
        exit(1);
    };
    //*Provide data for combobox that display available cuisines
    std::vector<std::pair<std::string, std::string>> cusines_a = m_db->getCuisines();
    std::set<std::string> cuisines_entry{};

    for (const auto &item : cusines_a)
    {
        cuisines_entry.insert(item.second);
    }
    m_cuisineComboB->addItem(tr("All"));
    for (const auto &cuisine : cuisines_entry)
    {
        m_cuisineComboB->addItem(QString{cuisine.c_str()});
    }

    //*Provide data for combobox that display availeble dish types.
    std::vector<std::pair<std::string, std::string>> dishesTypes_a = m_db->getDishTypes();
    std::set<std::string> dishType_entry{};
    for (const auto &item : dishesTypes_a)
    {
        dishType_entry.insert(item.second);
    }
    m_dishTypeComboB->addItem(tr("All"));
    for (const auto &dishtype : dishType_entry)
    {
        m_dishTypeComboB->addItem(QString{dishtype.c_str()});
    }

    //*Provide data for the checkboxes that will be displayed
    std::vector<std::pair<std::string, std::vector<std::string>>> ingridients = m_db->getAllIngridients();
    // It is provide uniqueness.
    std::set<std::string> ingridients_entry{};
    for (const auto &item : ingridients)
    {
        for (const auto &ingridient : item.second)
        {
            if (ingridient != std::string{})
                ingridients_entry.insert(ingridient);
        }
    }

    auto a_checkBox = new QCheckBox("All");
    // a_checkBox->setCheckState(Qt::CheckState::Checked);
    m_checkboxList_a.push_back(a_checkBox);

    for (const auto &ingridient : ingridients_entry)
    {
        m_checkboxList_a.push_back(new QCheckBox(QString{ingridient.c_str()}));
    }
    connect(m_checkboxList_a.front(), &QCheckBox::stateChanged, this, [this](int flag)
            {
                if (flag == Qt::Checked)
                {
                    std::for_each(std::next(m_checkboxList_a.begin(), 1), m_checkboxList_a.end(), [&](QCheckBox *cBox)
                                  { cBox->setCheckState(Qt::Checked); });
                } });

    QVBoxLayout *layout = new QVBoxLayout{};
    for (QCheckBox *checkBox : m_checkboxList_a)
    {
        layout->addWidget(checkBox);
    }
    auto _w = new QWidget{};
    _w->setLayout(layout);
    m_placeHolder->setWidget(_w);

    QItemSelectionModel *selectionModel = m_viewForResults->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, [this, selectionModel]()
            {
                const auto selectedRows = selectionModel->selectedRows();
                for (const QModelIndex &index : selectedRows)
                {
                    if (index.isValid())
                    {
                        m_open_Fileb->setEnabled(true);
                        return;
                    }
                    else
                    {
                        m_open_Fileb->setEnabled(false);
                    }
                }
                if(selectedRows.empty())
                    m_open_Fileb->setEnabled(false); });
}

std::set<std::string> SearchDialog::searchForText(const std::string &str)
{
    if (m_db == nullptr)
        return {};

    std::set<std::string> ids_a{};
    // TODO: Implement search of substr but not from the start of the line
    std::vector<std::pair<std::string, std::string>> arr = m_db->getNames();
    for (const auto &val : arr)
    {
        std::string name = val.second;
        bool flag{true};
        for (int i{0}; i < str.size(); ++i)
        {
            if (str[i] != name[i])
            {
                flag = false;
                break;
            }
        }
        if (flag == true)
            ids_a.insert(val.first);
    }
    return ids_a;
}

std::set<std::string> SearchDialog::searchForCuisine(const std::string &str)
{
    if (m_db == nullptr)
        return {};

    std::set<std::string> ids_a{};
    std::vector<std::pair<std::string, std::string>> arr = m_db->getCuisines();
    for (const auto &val : arr)
    {
        if (val.second == str)
            ids_a.insert(val.first);
    }
    return ids_a;
}

std::set<std::string> SearchDialog::searchForDishType(std::string t)
{
    if (m_db == nullptr)
        return {};

    std::set<std::string> ids_a{};
    std::vector<std::pair<std::string, std::string>> arr = m_db->getDishTypes();
    for (const auto &val : arr)
    {
        if (val.second == t)
            ids_a.insert(val.first);
    }
    return ids_a;
}

std::set<std::string> SearchDialog::searchForIngridentsNOnlyIncluded(const std::vector<std::string> &ingr_a)
{
    if (m_db == nullptr)
        return {};

    std::set<std::string> ids_a{};
    std::vector<std::pair<std::string, std::vector<std::string>>> arr =
        m_db->getAllIngridients();
    for (const auto &val : arr)
    {
        bool flag{true};
        for (const auto &item : ingr_a)
        {
            if (std::find(val.second.begin(), val.second.end(), item) == val.second.end())
            {
                flag = false;
            }
        }
        if (flag == true)
            ids_a.insert(val.first);
    }
    return ids_a;
}

std::set<std::string> SearchDialog::searchForIngridentsOnlyIncluded(const std::vector<std::string> &ingr_a)
{
    if (m_db == nullptr)
        return {};

    std::size_t n = ingr_a.size();
    std::set<std::string> ids_a{};
    std::vector<std::pair<std::string, std::vector<std::string>>> arr =
        m_db->getAllIngridients();
    for (const auto &val : arr)
    {
        bool flag{true};
        for (const auto &item : ingr_a)
        {
            if (std::find(val.second.begin(), val.second.end(), item) == val.second.end())
            {
                flag = false;
            }
        }
        if (flag == true && val.second.size() == n)
            ids_a.insert(val.first);
    }
    return ids_a;
}

bool SearchDialog::setUpSideBar()
{
    m_name = new QLabel(tr("&Name:"));
    m_cuisine = new QLabel(tr("Cuisine:"));
    m_dishType = new QLabel(tr("Dish type:"));
    m_ingridients = new QLabel(tr("Ingridients:"));
    m_results = new QLabel(tr("Results:"));
    m_viewForResults = new QListWidget{};
    m_only = new QCheckBox{tr("Only")};

    m_nameEdit = new QLineEdit{};
    m_cuisineComboB = new QComboBox{};
    m_dishTypeComboB = new QComboBox{};

    m_placeHolder = new QScrollArea{};
    m_placeHolder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_searchLayout = new QVBoxLayout{};
    m_resultsLayout = new QVBoxLayout{};
    m_formLayout = new QFormLayout{};
    m_mainLayout = new QHBoxLayout;

    m_find_b = new QPushButton(tr("Find"));
    m_open_Fileb = new QPushButton(tr("Open"));
    m_open_Fileb->setEnabled(false);

    m_name->setBuddy(m_nameEdit);

    m_formLayout->addRow(m_name, m_nameEdit);
    m_formLayout->addRow(m_cuisine, m_cuisineComboB);
    m_formLayout->addRow(m_dishType, m_dishTypeComboB);

    m_searchLayout->addLayout(m_formLayout);
    auto ing_layout = new QFormLayout{};
    ing_layout->addRow(m_ingridients, m_only);
    // m_searchLayout->addWidget(m_ingridients);
    m_searchLayout->addLayout(ing_layout);
    m_searchLayout->addWidget(m_placeHolder);

    m_searchLayout->addWidget(m_find_b);

    m_resultsLayout->addWidget(m_results);
    m_resultsLayout->addWidget(m_viewForResults);
    m_resultsLayout->addWidget(m_open_Fileb);

    m_mainLayout->addLayout(m_searchLayout);
    m_mainLayout->addLayout(m_resultsLayout);
    setLayout(m_mainLayout);
    // TODO Do the check if everything is properly initialized
    return true;
}

bool SearchDialog::find()
{
    // * Explanation. We need here to find intersection of 4 sets.
    // * In order to know if a set is considered to be universum we have a bitmap
    // * if in position is 1 then it is not a universum subset
    // * Then we intersect those 4 subsets using the bitmap.

    std::set<std::string> receiptsID_names{};
    std::set<std::string> receiptsID_cuisines{};
    std::set<std::string> receiptsID_dishes{};
    std::set<std::string> receiptsID_ingridients{};

    std::set<std::string> receiptsID_combined{};

    int state = 0x0;
    if (!(m_nameEdit->text().isEmpty()))
        state |= INAME;
    if (!(m_cuisineComboB->currentText() == QString("All")))
        state |= ICUISINE;
    if (!(m_dishTypeComboB->currentText() == QString("All")))
        state |= IDISHTYPE;
    if (!(isAllIngridients()))
        state |= IINGRIDIENTS;

    if (state & INAME)
        receiptsID_names = searchForText(m_nameEdit->text().toStdString());
    if (state & ICUISINE)
        receiptsID_cuisines = searchForCuisine(m_cuisineComboB->currentText().toStdString());
    if (state & IDISHTYPE)
        receiptsID_dishes = searchForDishType(m_dishTypeComboB->currentText().toStdString());
    if (state & IINGRIDIENTS)
    {
        std::vector<std::string> checkedIngridients{};
        for (QCheckBox *checkBox : m_checkboxList_a)
        {
            if (checkBox->isChecked())
                checkedIngridients.push_back(checkBox->text().toStdString());
        }
        if (m_only->isChecked())
            receiptsID_ingridients = searchForIngridentsOnlyIncluded(checkedIngridients);
        else
            receiptsID_ingridients = searchForIngridentsNOnlyIncluded(checkedIngridients);
    }

    // TODO: Implement a function performing intersection of two subsets.
    // TODO: To reduce repetition of the code
    //* Intersection of Names subset and Cuisines
    std::set<std::string> f{};
    bool is_fUniversum{false};
    if ((state & INAME) && (state & ICUISINE))
    {
        for (auto const &id : receiptsID_names)
        {
            auto res = receiptsID_cuisines.find(id);
            if (res != receiptsID_cuisines.end())
                f.insert(id);
        }
    }
    else if (state & INAME)
    {
        f = std::move(receiptsID_names);
    }
    else if (state & ICUISINE)
    {
        f = std::move(receiptsID_cuisines);
    }
    else
    {
        is_fUniversum = true;
    }
    //* Intersection of Dishes subset and Ingridiets
    std::set<std::string> s{};
    bool is_sUniversum{false};
    if ((state & IDISHTYPE) && (state & IINGRIDIENTS))
    {
        for (auto const &id : receiptsID_dishes)
        {
            auto res = receiptsID_ingridients.find(id);
            if (res != receiptsID_ingridients.end())
                s.insert(id);
        }
    }
    else if (state & IDISHTYPE)
    {
        s = std::move(receiptsID_dishes);
    }
    else if (state & IINGRIDIENTS)
    {
        s = std::move(receiptsID_ingridients);
    }
    else
    {
        is_sUniversum = true;
    }
    // * Inersection of two previous subsets
    if (!is_fUniversum && !is_sUniversum)
    {
        for (auto const &id : f)
        {
            auto res = s.find(id);
            if (res != s.end())
                receiptsID_combined.insert(id);
        }
    }
    else if (!is_fUniversum)
    {
        receiptsID_combined = std::move(f);
    }
    else if (!is_sUniversum)
    {
        receiptsID_combined = std::move(s);
    }
    else
    {
        auto tmp_a = m_db->getIDs();
        for (const auto &id : tmp_a)
        {
            receiptsID_combined.insert(id);
        }
    }
    m_options_a.clear();
    for (const auto &id : receiptsID_combined)
        m_options_a[id] = *(m_db->getName(id));

    // TODO: Implement a class that inherits from QListWIdgetItem so it will store not only a text but also an identifier in order to be able to directly call signal "displayView"
    m_viewForResults->clear();
    for (const auto &item : m_options_a)
        m_viewForResults->addItem(item.second.c_str());

    QItemSelectionModel *selectionModel = m_viewForResults->selectionModel();
    selectionModel->clear();

    if (receiptsID_combined.empty())
        return false;
    else
        return true;
}

void SearchDialog::openFile()
{
    std::string name = m_viewForResults->currentItem()->text().toStdString();
    std::string id;
    for (const auto item : m_options_a)
    {
        if (item.second == name)
        {
            id = item.first;
            break;
        }
    }
    if (id.empty())
        qDebug() << "Something unexpected happened\n";
    emit displayView(QString{id.c_str()});
}

bool SearchDialog::isAllIngridients()
{
    return m_checkboxList_a[0]->isChecked();
}