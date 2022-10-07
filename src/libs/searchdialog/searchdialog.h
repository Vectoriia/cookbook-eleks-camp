#ifndef _SEARCH_DIALOG_
#define _SEARCH_DIALOG_

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <vector>
#include <map>
#include <set>
#include <QListWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QScrollArea>
namespace CookBook
{
    class ProxyReceiptsDB;
}

namespace CookBook
{
    class SearchDialog : public QDialog
    {
        Q_OBJECT
    public:
        //holds id and name of the found entries
        //using id_and_name = std::pair<std::string, std::string>;
        SearchDialog(QWidget *parent = nullptr, ProxyReceiptsDB *db = nullptr);

    public:
        std::set<std::string> searchForText(const std::string &str);
        std::set<std::string> searchForCuisine(const std::string &str);
        std::set<std::string> searchForDishType(const std::string t);
        std::set<std::string> searchForIngridentsNOnlyIncluded(const std::vector<std::string> &ingr_a);
        std::set<std::string> searchForIngridentsOnlyIncluded(const std::vector<std::string> &ingr_a);

    public slots:
        bool find();
        void openFile();
    signals:
        void displayView(const QString &id);

    private:
        std::map<std::string, std::string> m_options_a;
        ProxyReceiptsDB *m_db;

        //Output
        QLabel *m_name;
        QLabel *m_cuisine;
        QLabel *m_dishType;
        QLabel *m_ingridients;
        QLabel *m_results;
        QListWidget *m_viewForResults;

        //Inputs
        QLineEdit *m_nameEdit;
        QComboBox *m_cuisineComboB;
        QComboBox *m_dishTypeComboB;
        QScrollArea *m_placeHolder;
        QList<QCheckBox *> m_checkboxList_a;
        QCheckBox * m_only; //Is used to define wethere only selected ingridietns should be in the dish to be found or not

        //layouts
        QVBoxLayout *m_searchLayout;
        QVBoxLayout *m_resultsLayout;
        QFormLayout *m_formLayout;
        QHBoxLayout *m_mainLayout;

        //Buttons
        QPushButton *m_find_b;
        QPushButton *m_open_Fileb;

    protected:
        bool isAllIngridients();
        bool setUpSideBar();

        enum
        {
            INAME = 0x01,
            ICUISINE = 0x2,
            IDISHTYPE = 0x4,
            IINGRIDIENTS = 0x8,
        };
    };
}

#endif