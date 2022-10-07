#include "calculatedialog.h"
#include <QTextStream>
#include "ui_calculatedialog.h"
using namespace CookBook;

CalculateDialog::CalculateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalculateDialog)
{
    ui->setupUi(this);

}
CalculateDialog::CalculateDialog (QWidget* parent,int id,std::string filename): QDialog(parent),ui(new Ui::CalculateDialog){
    ui->setupUi(this);
    setWindowTitle("Calculate");

   this->m_current_file=filename;
   this->m_id=id;
   QFile data(QString::fromStdString(this->m_current_file));
   if (data.open(QIODevice::ReadOnly))
   {
      QTextStream in(&data);
      while (!in.atEnd())
      {
         QString line = in.readLine();
         if(this->ParseLine(line,m_id)){
             break;
         }
      }
      data.close();
   }

   //changing qt objects

   ui->tW_properties_calc->setItem(0,1,new QTableWidgetItem(QString::number(this->m_weight)+"g"));
   ui->tW_properties_calc->setItem(1,1,new QTableWidgetItem(QString::number(this->m_calories)));
   ui->tW_properties_calc->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   ui->tW_properties_calc->setEditTriggers(QAbstractItemView::NoEditTriggers);

   QStringList ingredient_splitted;
   for(int i=0;i<(int)this->m_ingredients_a.size();i++){
       ingredient_splitted = QString::fromStdString(m_ingredients_a[i]).split( "-" );
       ui->tW_ingredients_calc->insertRow(i);
       ui->tW_ingredients_calc->setItem(i,0,new QTableWidgetItem(QString::number(ingredient_splitted[0].toDouble())));
       ui->tW_ingredients_calc->setItem(i,1,new QTableWidgetItem(ingredient_splitted[1]));
   }

   ui->tW_ingredients_calc->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   ui->tW_ingredients_calc->setEditTriggers(QAbstractItemView::NoEditTriggers);

   ui->te_current_servings->setText(QString::number(this->m_servings));

}
CalculateDialog::~CalculateDialog()
{
    delete ui;
}

void CalculateDialog::on_pb_exit_clicked()
{
     close();
}
int CalculateDialog::ParseLine(QString line, int id){

    QStringList general_reicpe_list = line.split( "|" );
    //QString neededID = general_reicpe_list.value( 0 );
    //int current_id=neededID.toInt();

    QStringList ingredients_list;


    this->m_servings = general_reicpe_list.value(4).toInt();
    this->m_calories = general_reicpe_list.value(5).toInt();
    this->m_weight = general_reicpe_list.value(6).toInt();
    ingredients_list =  general_reicpe_list.value(7).split("*");
    for(int i= 0;i<ingredients_list.size();i++){
        this->m_ingredients_a.push_back(ingredients_list.value(i).toStdString());
    }

    return 0;
}

void CalculateDialog::on_pb_calc_clicked()
{
    if(ui->sB_needed_servings->value()>0){
    int calc_calories= this->m_calories/this->m_servings* ui->sB_needed_servings->value();
    int calc_weight=this->m_weight/this->m_servings* ui->sB_needed_servings->value();
    ui->tW_properties_calc->setItem(0,1,new QTableWidgetItem(QString::number(calc_weight)+"g"));
    ui->tW_properties_calc->setItem(1,1,new QTableWidgetItem(QString::number(calc_calories)));
    double calc_ingredient_counted=0;
    QStringList ingredient_splitted;

    for(int i =0;i<ui->tW_ingredients_calc->rowCount();i++){
        ingredient_splitted = QString::fromStdString(m_ingredients_a[i]).split( "-" );
        calc_ingredient_counted=ingredient_splitted[0].toDouble()/this->m_servings*ui->sB_needed_servings->value();
        ui->tW_ingredients_calc->setItem(i,0,new QTableWidgetItem(QString::number(calc_ingredient_counted,'d', 2)));
    }

    ui->te_current_servings->setText(QString::number(ui->sB_needed_servings->value()));
    }
}

