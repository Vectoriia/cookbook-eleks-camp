#ifndef CREATERECEIPTVEIW_H
#define CREATERECEIPTVEIW_H

#include <QFile>
#include <QWidget>
#include <QString>
#include <string>
#include<QRandomGenerator>
#include <QFocusEvent>
#include <QRegularExpressionValidator>
 #include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QFont>
#include <receiptview/receiptview.h>
#include <viewwidget/viewwidget.h>

QT_BEGIN_NAMESPACE
namespace Ui { class CreateReceiptVeiw; }
QT_END_NAMESPACE

namespace CookBook
{
class CreateReceiptVeiw : public ViewWidget
{
    Q_OBJECT

public:
    CreateReceiptVeiw( QWidget *parent);
    CreateReceiptVeiw( QWidget *parent,std::string filename_original, std::string file_image);
    ~CreateReceiptVeiw();

    bool AddReceiptToDataFile();

private slots:

    bool on_pb_confirm_receipt_clicked();

    void on_pb_add_row_clicked();

    void on_pb_delete_last_row_clicked();

    void on_pb_add_photo_clicked();

    void on_tW_ingredients_cellClicked(int row, int column);

public slots:
    void confirm_closing_tab(int index);

    bool saving(bool isSavedAs);

signals:
    void tabWasSaved();
    void tabEditableWasSaved(int index);
    void closingEditabletabConfirmed(int index);
    void changeSavedStatusVector(bool value, int index);
private:
    Ui::CreateReceiptVeiw *ui;

    std::string m_name;
    std::string m_cuisine;
    std::string m_dishtype;
    std::string m_receipt_file;
    std::string m_image_file;

    std::vector<std::string> m_ingredients_a;
    std::vector<std::string> m_method_a;

    unsigned int m_id;
    unsigned int m_servings;
    unsigned int m_weight;
    unsigned int m_calories;

    bool m_isEditingTab;
    bool m_is_via_msgbox;

};

}
#endif
