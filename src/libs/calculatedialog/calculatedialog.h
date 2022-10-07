#ifndef CALCULATEDIALOG_H
#define CALCULATEDIALOG_H
#include <string>
#include <QDialog>
#include <QFile>
QT_BEGIN_NAMESPACE
namespace CookBook
{
    class ProxyReceiptsDB;
}
namespace Ui { class CalculateDialog; }
QT_END_NAMESPACE
namespace CookBook
{
class CalculateDialog : public QDialog
{
    Q_OBJECT

public:
    CalculateDialog(QWidget *parent = nullptr);
    CalculateDialog( QWidget *parent,int id,std::string filename);

    ~CalculateDialog();

    int ParseLine(QString line, int id);
private slots:
    void on_pb_exit_clicked();

    void on_pb_calc_clicked();

private:
    Ui::CalculateDialog *ui;

    std::string m_current_file;
    std::vector<std::string> m_ingredients_a;
    int m_id;
    int m_servings;
    int m_weight;
    int m_calories;


};

}
#endif // CALCULATEDIALOG_H
