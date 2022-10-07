#include <proxyreceiptsDB.h>
#include <QFile>
#include <QTextStream>

using namespace CookBook;

ProxyReceiptsDB::ProxyReceiptsDB(const std::string &filename, QObject *parent) : QObject(parent)
{
    setupDB(filename);
}

std::vector<std::string> ProxyReceiptsDB::getIDs() const
{
    std::vector<std::string> res{};
    std::for_each(m_receipts_a.begin(), m_receipts_a.end(), [&](const std::pair<std::string, item> &i)
                  { res.push_back(i.first); });
    return res;
}

std::unique_ptr<std::string> ProxyReceiptsDB::getFilePath(const std::string &id) const
{
    auto iter = m_receipts_a.find(id);

    if (iter == m_receipts_a.end())
        return nullptr;

    return std::make_unique<std::string>((*iter).second.filepath);
}

std::unique_ptr<std::string> ProxyReceiptsDB::getCuisine(const std::string &id) const
{
    auto iter = m_receipts_a.find(id);

    if (iter == m_receipts_a.end())
        return nullptr;

    return std::make_unique<std::string>((*iter).second.cuisine);
}

std::unique_ptr<std::string> ProxyReceiptsDB::getName(const std::string &id) const
{
    auto iter = m_receipts_a.find(id);

    if (iter == m_receipts_a.end())
        return nullptr;

    return std::make_unique<std::string>((*iter).second.receiptName);
}

std::unique_ptr<std::string> ProxyReceiptsDB::getDishType(const std::string &id) const
{
    auto iter = m_receipts_a.find(id);

    if (iter == m_receipts_a.end())
        return nullptr;

    return std::make_unique<std::string>((*iter).second.dishType);
}

std::vector<std::string> ProxyReceiptsDB::getIngridients(const std::string &id) const
{
    auto iter = m_receipts_a.find(id);

    if (iter == m_receipts_a.end())
        return {};

    return iter->second.ingridients_a;
}

std::unique_ptr<ProxyReceiptsDB::item> ProxyReceiptsDB::getItem(const std::string &id) const
{
    auto iter = m_receipts_a.find(id);

    if (iter == m_receipts_a.end())
        return nullptr;

    return std::make_unique<item>((*iter).second);
}

std::vector<std::pair<std::string, std::string>> ProxyReceiptsDB::getNames() const
{
    std::vector<std::pair<std::string, std::string>> res{};

    std::for_each(m_receipts_a.begin(), m_receipts_a.end(), [&](const std::pair<std::string, item> &i)
                  { res.emplace_back(i.first, i.second.receiptName); });
    return res;
}

std::vector<std::pair<std::string, std::vector<std::string>>> ProxyReceiptsDB::getAllIngridients() const
{
    std::vector<std::pair<std::string, std::vector<std::string>>> res{};

    std::for_each(m_receipts_a.begin(), m_receipts_a.end(), [&](const std::pair<std::string, item> &i)
                  { res.emplace_back(i.first, i.second.ingridients_a); });
    return res;
}

std::vector<std::pair<std::string, std::string>> ProxyReceiptsDB::getCuisines() const
{
    std::vector<std::pair<std::string, std::string>> res{};

    std::for_each(m_receipts_a.begin(), m_receipts_a.end(), [&](const std::pair<std::string, item> &i)
                  { res.emplace_back(i.first, i.second.cuisine); });
    return res;
}

std::vector<std::pair<std::string, std::string>> ProxyReceiptsDB::getDishTypes() const
{
    std::vector<std::pair<std::string, std::string>> res{};

    std::for_each(m_receipts_a.begin(), m_receipts_a.end(), [&](const std::pair<std::string, item> &i)
                  { res.emplace_back(i.first, i.second.dishType); });
    return res;
}

std::vector<std::pair<std::string, bool>> ProxyReceiptsDB::getPrefered() const
{
    std::vector<std::pair<std::string, bool>> res{};

    std::for_each(m_receipts_a.begin(), m_receipts_a.end(), [&](const std::pair<std::string, item> &i)
                  { res.emplace_back(i.first, i.second.prefferd); });
    return res;
}

void ProxyReceiptsDB::setupDB(const std::string &filepath)
{
    QFile file(filepath.c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        process_line(line.toStdString());
    }
}

void ProxyReceiptsDB::process_line(const std::string &str)
{
    QString str_c{str.c_str()};
    str_c = str_c.trimmed();
    QStringList arr = str_c.split('$');
    item obj_a;

    //Value is used because index may be out of range,
    //if there is no suatable data a default value of QStringList will suffice
    obj_a.filepath = arr.value(I_TEXTPATH).toStdString();
    obj_a.receiptName = arr.value(I_NAME).toStdString();
    obj_a.cuisine = arr.value(I_CUISINE).toStdString();
    obj_a.dishType = arr.value(I_DISHTYPE).toStdString();
    obj_a.prefferd = arr.value(I_PREFERRED).toInt();
    QStringList ingidients_a = arr.value(I_INGRIDIENTS).split('|');

    for (const auto &i : ingidients_a)
        obj_a.ingridients_a.push_back(i.toStdString());

    obj_a.imagepath = arr.value(I_IMAGEPATH).toStdString();

    // TODO: Make identifier of the input to be a SHA-1 hash code.
    m_receipts_a[arr[I_NAME].toStdString()] = obj_a;
}

void ProxyReceiptsDB::changePreference(const std::string &id, bool flag)
{
    do
    {
        auto iter = m_receipts_a.find(id);

        if (iter == m_receipts_a.end())
            break;

        m_receipts_a[id].prefferd = flag;
        emit prefChanged(id, flag);
        //emit dataBaseChanged();
        //emit changePreference(id, flag);
    } while (false);
}

std::unique_ptr<bool> ProxyReceiptsDB::isPreferred(const std::string &id) const
{
    auto iter = m_receipts_a.find(id);

    if (iter == m_receipts_a.end())
        return nullptr;

    return std::make_unique<bool>(iter->second.prefferd);
}

std::unique_ptr<std::string> ProxyReceiptsDB::getImagePath(const std::string &id) const
{
    auto iter = m_receipts_a.find(id);

    if (iter == m_receipts_a.end())
        return nullptr;

    return std::make_unique<std::string>(iter->second.imagepath);
}

bool ProxyReceiptsDB::addEntry(std::string const &id, DataItem const &item)
{
    bool res{false};
    auto i = m_receipts_a.find(id);
    do
    {
        if (i != m_receipts_a.end())
            break;

        m_receipts_a[id] = item;
    } while (false);

    emit dataBaseChanged();
    return res;
}

void ProxyReceiptsDB::save() const
{
    QFile file(Settings::dataBaseFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        for (const auto &entry : m_receipts_a)
        {
            out << QString::fromStdString(entry.second.filepath) << '$'
                << QString::fromStdString(entry.second.receiptName) << '$'
                << QString::fromStdString(entry.second.cuisine) << '$'
                << QString::fromStdString(entry.second.dishType) << '$';
            for (int i{0}; i < entry.second.ingridients_a.size(); ++i)
            {
                out << QString::fromStdString(entry.second.ingridients_a[i]);
                if (i != entry.second.ingridients_a.size() - 1)
                    out << '|';
            }
            out << '$';
            out << QString::number(entry.second.prefferd) << '$'
                << QString::fromStdString(entry.second.imagepath) << '\n';
        }
        file.close();
    }
}