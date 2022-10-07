#ifndef _PROXYRECEIPTS_DB_
#define _PROXYRECEIPTS_DB_

#include <vector>
#include <map>
#include <utility>
#include <string>
#include <QObject>
#include <memory>
#include <ui/settings.h>
namespace CookBook
{
    class ProxyReceiptsDB : public QObject
    {
        Q_OBJECT
    public:
        struct DataItem
        {
            std::vector<std::string> ingridients_a;
            std::string filepath;
            std::string imagepath;
            std::string receiptName;
            std::string cuisine;
            std::string dishType;
            bool prefferd;
        };
        //using item = std::tuple<std::string, std::string, /*CookBook::Cuisine*/std::string,
        //std::string, std::vector<std::string>, bool > ;
        using item = DataItem;
        explicit ProxyReceiptsDB(const std::string &filename, QObject *parent = nullptr);

        Q_DISABLE_COPY(ProxyReceiptsDB)

        // getters

        std::vector<std::string> getIDs() const;

        std::unique_ptr<std::string> getFilePath(const std::string &id) const;

        std::unique_ptr<std::string> getCuisine(const std::string &id) const;

        std::unique_ptr<std::string> getName(const std::string &id) const;

        std::unique_ptr<std::string> getDishType(const std::string &id) const;

        std::vector<std::string> getIngridients(const std::string &id) const;

        std::unique_ptr<bool> isPreferred(const std::string &id) const;

        std::unique_ptr<std::string> getImagePath(const std::string &id) const;

        std::unique_ptr<item> getItem(const std::string &id) const;

        std::vector<std::pair<std::string, std::string>> getNames() const;

        std::vector<std::pair<std::string, std::vector<std::string>>> getAllIngridients() const;

        std::vector<std::pair<std::string, std::string>> getCuisines() const;

        std::vector<std::pair<std::string, std::string>> getDishTypes() const;

        std::vector<std::pair<std::string, bool>> getPrefered() const;

        bool addEntry(const std::string &id , const DataItem &item);

        void save() const;

    protected:
        enum
        {
            I_TEXTPATH,
            I_NAME,
            I_CUISINE,
            I_DISHTYPE,
            I_INGRIDIENTS,
            I_PREFERRED,
            I_IMAGEPATH,
        };
        void setupDB(const std::string &filepath);
        void process_line(const std::string &str);

    public slots:
        void changePreference(const std::string &id, bool flag);

    signals:
        // * First parametr is an ID second identify the state: false - states for not being in pref list, true otherwise
        void prefChanged(const std::string &, bool);
        void dataBaseChanged();

    private:
        // * An element of the array should consitst of:
        // *1.ID(SHA-1 from the name), 2. Path to the file, 3.Name of the receipt,
        // *4.Cuisine, 5. Dish type, 6. Array of ingridients, 7.Flag to identify is in preference list
        std::map<std::string, item> m_receipts_a;
    };
}
#endif