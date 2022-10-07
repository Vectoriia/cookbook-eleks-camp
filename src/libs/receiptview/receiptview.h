#ifndef RECEIPTVIEW_H
#define RECEIPTVIEW_H

#include <string>
#include <vector>
#include <QLabel>
#include <QFile>
#include <QMessageBox>
#include<QWidget>
#include <calculatedialog/calculatedialog.h>
#include <viewwidget/viewwidget.h>

QT_BEGIN_NAMESPACE
namespace Ui { class ReceiptView; }
QT_END_NAMESPACE

namespace CookBook
{
class ReceiptView : public ViewWidget
{
    Q_OBJECT

public:
    ReceiptView(QWidget *parent,unsigned int id,std::string receipt_filename, std::string image_filename);
    ~ReceiptView();

    void setUpReceiptView();
    void setReceiptId(const std::string & receiptID);
    void setPref(bool pref);

    signals:
    void closeTab();
    void editReceipt(std::string filename_original,std::string file_image);
    void showCalculateDialog(long long int);
    void addToPreferance(const std::string &receiptID);
    void deleteFromPreferance(const std::string &receiptID);
    

protected:
    static int ParseLine(QString line, ReceiptView& obj);
public:
    static void Parse(const std::string &filename, std::string &receiptName,
                     std::string &cuisine, std::string &dishType,
                     std::vector<std::string> & ingridients_a);
    static void Parse(const std::string &filename, std::string &receiptName,
                     std::string &cuisine, std::string &dishType,
                     std::vector<std::string> & ingredients_a, std::vector<std::string> & method_a,
                      unsigned &servings, unsigned &weight,unsigned &calories);
private slots:
    void on_pb_calculate_dialog_clicked();

    void on_cb_isFavorite_stateChanged(int arg1);

    void showDialog();

    void on_pb_edit_clicked();

private:
    Ui::ReceiptView *ui;

    CalculateDialog* calcDialog{nullptr};

    std::string m_name;
    std::string m_cuisine;
    std::string m_dishtype;
    std::string m_receipt_file;
    std::string m_image_file;

    std::vector<std::string> m_ingredients_a;
    std::vector<std::string> m_method_a;

    std::string receiptID;
    unsigned int m_id;
    unsigned int m_servings;
    unsigned int m_weight;
    unsigned int m_calories;

    bool m_isFavorite;

};

}

#endif // RECEIPTVIEW_H
