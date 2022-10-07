#include "createreceiptview.h"
#include "ui_createreceiptveiw.h"
#include <QDialogButtonBox>
using namespace CookBook;

CreateReceiptVeiw::CreateReceiptVeiw( QWidget *parent): ViewWidget(parent), ui(new Ui::CreateReceiptVeiw){
    ui->setupUi(this);

    this->m_isEditingTab=false;
    this->m_is_via_msgbox=false;
    this->m_receipt_file="";

    //setting validator
    ui->lE_name->setValidator(new QRegularExpressionValidator( QRegularExpression("[A-Za-z(),. ]+")));
    ui->lE_cuisine->setValidator(new QRegularExpressionValidator( QRegularExpression("[A-Za-z ]+")));
    ui->lE_dishtype->setValidator(new QRegularExpressionValidator( QRegularExpression("[A-Za-z ]+")));
    ui->lE_calories->setValidator(new QRegularExpressionValidator( QRegularExpression("[0-9]+")));
    ui->lE_weight->setValidator(new QRegularExpressionValidator( QRegularExpression("[0-9]+")));
    ui->lE_servings->setValidator(new QRegularExpressionValidator( QRegularExpression("[0-9]+")));

    //hint fow user
    ui->lE_name->setText("Beef stew");
    ui->lE_cuisine->setText("French");
    ui->lE_dishtype->setText("Stew");
    ui->lE_servings->setText("4");
    ui->lE_weight->setText("875");
    ui->lE_calories->setText("560");

    ui->l_hint->setVisible(false);

}
//constructor for editing tab
CreateReceiptVeiw::CreateReceiptVeiw( QWidget *parent,std::string filename_original, std::string file_image): ViewWidget(parent), ui(new Ui::CreateReceiptVeiw){
    ui->setupUi(this);

    this->m_isEditingTab=true;
    this->m_is_via_msgbox=false;
    this->m_receipt_file="";

    //setting validator
    ui->lE_name->setValidator(new QRegularExpressionValidator( QRegularExpression("[A-Za-z(),.]+")));
    ui->lE_cuisine->setValidator(new QRegularExpressionValidator( QRegularExpression("[A-Za-z]+")));
    ui->lE_dishtype->setValidator(new QRegularExpressionValidator( QRegularExpression("[A-Za-z]+")));
    ui->lE_calories->setValidator(new QRegularExpressionValidator( QRegularExpression("[0-9]+")));
    ui->lE_weight->setValidator(new QRegularExpressionValidator( QRegularExpression("[0-9]+")));
    ui->lE_servings->setValidator(new QRegularExpressionValidator( QRegularExpression("[0-9]+")));

    //info from file
    ReceiptView::Parse(filename_original, m_name,m_cuisine, m_dishtype,m_ingredients_a, m_method_a,m_servings,m_weight, m_calories);

    //qt settings
    ui->lE_name->setText(QString::fromStdString(m_name));
    ui->lE_cuisine->setText(QString::fromStdString(m_cuisine));
    ui->lE_dishtype->setText(QString::fromStdString(m_dishtype));
    ui->lE_servings->setText(QString::number(m_servings));
    ui->lE_weight->setText(QString::number(m_weight));
    ui->lE_calories->setText(QString::number(m_calories));

    QStringList ingredient_splitted;

    for(int i=0;i<(int)this->m_ingredients_a.size();i++){
        ingredient_splitted = QString::fromStdString(m_ingredients_a[i]).split( "-" );
        ui->tW_ingredients->insertRow(i);

        auto item1=new QTableWidgetItem ;
        item1->setText(QString::number(ingredient_splitted[0].toDouble()));
        ui->tW_ingredients->setItem(i,0,item1);

        auto item2=new QTableWidgetItem ;
        item2->setText(ingredient_splitted[1]);
        ui->tW_ingredients->setItem(i,1,item2);
    }

    ui->tW_ingredients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tW_ingredients->setEditTriggers(QAbstractItemView::SelectedClicked);

    QString temp;
    for(int i=0;i<(int)this->m_method_a.size();i++){
        temp.append(QString::fromStdString(this->m_method_a[i])+"\n");
    }
    ui->tE_method->setText(temp);

    //user cannot change photo
    ui->pb_add_photo->setText("Adding photo not available");
    ui->pb_add_photo->setEnabled(false);
    this->m_image_file=file_image;

    ui->label_title->setText("Edit the receipt");

}
CreateReceiptVeiw::~CreateReceiptVeiw()
{
    delete ui;
}

//102|name|favorite|cuisine|dishtype|servings|calories|weight|in*gredi*ents|met*ho*d

bool CreateReceiptVeiw::on_pb_confirm_receipt_clicked()
{
    bool isSavedSuccesfully;
     isSavedSuccesfully= saving(true);

    if(!isSavedSuccesfully){
        return false;
    }else{
        this->setSavedStatus(true);
        if(!m_is_via_msgbox){
            //emit changeSavedStatusVector(true,m_tab_index);
            emit tabWasSaved();
        }
        else {
            emit tabEditableWasSaved(m_tab_index);
        }
    }
}

//name|favorite|cuisine|dishtype|servings|calories|weight|in*gredi*ents|met*ho*d
bool CreateReceiptVeiw::AddReceiptToDataFile(){

    QString receiptLine;

    //receiptLine+=QString::fromStdString(id)+"|";
    receiptLine+=QString::fromStdString(this->m_name)+"|";
    receiptLine+=QString::number(0)+"|";
    receiptLine+=QString::fromStdString(this->m_cuisine)+"|";
    receiptLine+=QString::fromStdString(this->m_dishtype)+"|";
    receiptLine+=QString::number(this->m_servings)+"|";
    receiptLine+=QString::number(this->m_calories)+"|";
    receiptLine+=QString::number(this->m_weight)+"|";
    // receiptLine+=QString::fromStdString(this->m_image_file)+"|";
    for(int i=0;i<m_ingredients_a.size();i++){
        receiptLine+=QString::fromStdString(this->m_ingredients_a[i]);
        if(i!=m_ingredients_a.size()-1)
            receiptLine+="*";
    }
    receiptLine+="|";
    for(int i=0;i<m_method_a.size();i++){
        receiptLine+=QString::fromStdString(this->m_method_a[i]);
        if(i!=m_method_a.size()-1)
            receiptLine+="*";
    }

    QFile file(QString::fromStdString(m_receipt_file));
    if (file.open(QIODevice::ReadWrite)){
        QTextStream out(&file);
        this->setSavedStatus(true);
        out <<receiptLine;
    }else{
        QMessageBox::warning(nullptr, "Attention!","Something wrong with your files.");
         return false;
    }

    // QImage *img = new QImage(QString::fromStdString(m_image_file));
    // img->save(QString::fromStdString("/home/vectoria/cookbook/cookbook/data/created/images/receipt")+QString::number(this->m_id)+".png",  "png", 100);
    // delete img;
    return true;

}

void CreateReceiptVeiw::on_pb_add_row_clicked()
{
    ui->tW_ingredients->insertRow(ui->tW_ingredients->rowCount());

    auto item1 =new QTableWidgetItem;
    QLineEdit *edit_first_column = new QLineEdit(ui->tW_ingredients);
    edit_first_column->setValidator(new QDoubleValidator(edit_first_column));

    ui->tW_ingredients->setCellWidget(ui->tW_ingredients->rowCount()-1, 0, edit_first_column);
    ui->tW_ingredients->setItem(ui->tW_ingredients->rowCount()-1, 0,item1 );

    auto item2 =new QTableWidgetItem;
    QLineEdit *edit_second_column = new QLineEdit(ui->tW_ingredients);
    edit_second_column->setValidator(new QRegularExpressionValidator( QRegularExpression("[A-Za-z(),.]+")));

    ui->tW_ingredients->setCellWidget(ui->tW_ingredients->rowCount()-1, 1, edit_second_column);
    ui->tW_ingredients->setItem(ui->tW_ingredients->rowCount()-1, 1,item2);

}

void CreateReceiptVeiw::on_pb_delete_last_row_clicked()
{
    if(ui->tW_ingredients->rowCount()>0){
            ui->tW_ingredients->removeRow(ui->tW_ingredients->rowCount()-1);
    }
}

void CreateReceiptVeiw::on_pb_add_photo_clicked()
{
    QString selfilter = tr("JPEG (*.jpg *.jpeg)");
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"),&selfilter);
    if (!fileName.isEmpty())
        this->m_image_file=fileName.toStdString();
    else
        QMessageBox::information(nullptr,"Adding denied","You haven't choose suitable file.");
}

void CreateReceiptVeiw::on_tW_ingredients_cellClicked(int row, int column){
    if(m_isEditingTab)
    {

        if(column==0)
        {
            QLineEdit *edit_first_column = new QLineEdit(ui->tW_ingredients);
            edit_first_column->setValidator(new QDoubleValidator(edit_first_column));

            ui->tW_ingredients->setCellWidget(row, 0, edit_first_column);
        }else if(column==1)
        {
            QLineEdit *edit_second_column = new QLineEdit(ui->tW_ingredients);
            edit_second_column->setValidator(new QRegularExpressionValidator( QRegularExpression("[A-Za-z(),.]+")));

            ui->tW_ingredients->setCellWidget(row, 1, edit_second_column);
        }
    }
}

void CreateReceiptVeiw::confirm_closing_tab(int index){


        QMessageBox msgBox( QMessageBox::Question,"Cookbook","You're trying to close unsaved page. It can cause data loss.\n Do you want to change?");
        QAbstractButton *dontSaveBtn =msgBox.addButton("Don't save",QMessageBox::RejectRole);
       QAbstractButton *cancelBtn= msgBox.addButton("Cancel",QMessageBox::AcceptRole);
        QAbstractButton *saveBtn = msgBox.addButton("Save",QMessageBox::DestructiveRole);
        msgBox.exec();
        if (msgBox.clickedButton() == saveBtn) {
            m_is_via_msgbox=true;
            if(on_pb_confirm_receipt_clicked()){
                this->setSavedStatus(true);
            }
        }else if(msgBox.clickedButton() == dontSaveBtn){
            emit closingEditabletabConfirmed(index);
            this->setSavedStatus(true);
        }else if(msgBox.clickedButton() == cancelBtn){
            this->setSavedStatus(false);
        }

}
bool CreateReceiptVeiw::saving(bool isSavedAs){


    //checking if necessary fields are filled
    if(ui->lE_name->text().length()==0){
        QMessageBox::warning(nullptr,"Adding denied","Error:\nYou cannot save receipt without name.");
        return false;
    }
    if(ui->lE_servings->text().length()>0){
        if(ui->lE_servings->text()=="0"){
            QMessageBox::warning(nullptr,"Adding denied","Error:\nValue of servings must be at least one.");
            return false;
        }
    }
    if(ui->lE_servings->text().length()==0){
        QMessageBox::warning(nullptr,"Adding denied","Error:\nYou cannot save receipt without servings.");
        return false;
    }
    if(ui->tW_ingredients->rowCount()==0){
        QMessageBox::warning(nullptr,"Adding denied","Error:\nYou cannot save receipt without ingredients.");
        return false;
    }
  if(m_isEditingTab){
        for(int i=0;i<ui->tW_ingredients->rowCount();i++){
            if(ui->tW_ingredients->item(i,0)->text().length()==0||ui->tW_ingredients->item(i,1)->text().length()==0){
                QMessageBox::warning(nullptr,"Adding denied","Error:\nYou cannot save receipt with empty ingredients.");
                return false;
            }
        }

    }else{
        for(int i=0;i<ui->tW_ingredients->rowCount();i++){
            if(ui->tW_ingredients->cellWidget(i,0)->property("text").toString().length()==0||ui->tW_ingredients->cellWidget(i,1)->property("text").toString().length()==0){
                QMessageBox::warning(nullptr,"Adding denied","Error:\nYou cannot save receipt with empty ingredients.");
                return false;
            }
        }

   }


    //loading to user's receipt directory
  if(isSavedAs){
      QString saveFile = QFileDialog::getSaveFileName(this, tr("CookBook Save As"), "/home", "Text files (*.txt)");
      if(saveFile.length()==0){
          return false;
      }
      this->m_receipt_file=saveFile.toStdString()+".txt";
    }else if(m_receipt_file.length()==0){
        QString saveFile = QFileDialog::getSaveFileName(this, tr("CookBook Save As"), "/home", "Text files (*.txt)");
        if(saveFile.length()==0){
            return false;
        }
        this->m_receipt_file=saveFile.toStdString()+".txt";
    }


    //save receipt

    this->m_name= ui->lE_name->text().toStdString();
    this->m_cuisine=ui->lE_cuisine->text().toStdString();
    this->m_dishtype=ui->lE_dishtype->text().toStdString();
    this->m_weight= ui->lE_weight->text().toUInt();
    this->m_calories=ui->lE_calories->text().toUInt();

    if(ui->lE_cuisine->text().length()==0)
      this->m_cuisine="Other";
    if(ui->lE_dishtype->text().length()==0)
        this->m_dishtype="Other";
    if(ui->lE_weight->text().length()==0)
         this->m_weight=0;
    if(ui->lE_calories->text().length()==0)
        this->m_calories=0;

    this->m_servings=ui->lE_servings->text().toUInt();
    if(m_isEditingTab){
        this->m_ingredients_a.clear();
         for(int i=0;i<ui->tW_ingredients->rowCount();i++){
            this->m_ingredients_a.push_back(ui->tW_ingredients->item(i,0)->text().toStdString()+"-"+ui->tW_ingredients->item(i,1)->text().toStdString());
         }

    }else{
        for(int i=0;i<ui->tW_ingredients->rowCount();i++)
        {
            this->m_ingredients_a.push_back(ui->tW_ingredients->cellWidget(i,0)->property("text").toString().toStdString()+"-"+ui->tW_ingredients->cellWidget(i,1)->property("text").toString().toStdString());
        }
    }
    QString temp_method = ui->tE_method->toPlainText();

    QStringList tempList = temp_method.split(QRegularExpression("[\n]"),Qt::SkipEmptyParts);
     this->m_method_a.clear();

    for(int i=0;i<tempList.size();i++){

        this->m_method_a.push_back(tempList.value(0).toStdString());
    }

   return AddReceiptToDataFile();


}
