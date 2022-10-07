#include "receiptview.h"
#include "ui_receiptview.h"
#include <QScopedPointer>
#include <QTextStream>
using namespace CookBook;

ReceiptView::ReceiptView(QWidget *parent, unsigned int id, std::string receipt_filename, std::string image_filename) : ViewWidget(parent), ui(new Ui::ReceiptView)
{
    ui->setupUi(this);
    this->m_id = id;

    this->m_image_file = image_filename;
    if(image_filename=="disable"){
        ui->l_photo->setVisible(false);
        ui->cb_isFavorite->setVisible(false);
        ui->pb_calculate_dialog->setEnabled(false);
    }
    this->m_receipt_file = receipt_filename;

    QFile data(QString::fromStdString(this->m_receipt_file));
    if (data.open(QIODevice::ReadOnly))
    {
        QTextStream in(&data);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            ParseLine(line,*this);
        }
        data.close();
    }
    else
    {
        QMessageBox::warning(nullptr, "Warning!", "Files are damaged.\nTry to reinstall application.");
    }

    //changing qt objects
    setUpReceiptView();

    calcDialog = new CalculateDialog(nullptr, this->m_id, this->m_receipt_file);

    connect(calcDialog, &CalculateDialog::accepted, this, &ReceiptView::show);
    connect(calcDialog, &CalculateDialog::rejected, this, &ReceiptView::show);
}

ReceiptView::~ReceiptView()
{
    if (calcDialog)
        delete calcDialog;
    if (ui)
        delete ui;
}

void ReceiptView::setUpReceiptView()
{

    ui->l_name_of_receipt->setText(QString::fromStdString(this->m_name));
    ui->l_cuisine->setText(QString::fromStdString(this->m_cuisine));
    ui->l_dishtype->setText(QString::fromStdString(this->m_dishtype));

    if (m_isFavorite)
    {
        ui->cb_isFavorite->setChecked(1);
    }

    QString temp;
    for (int i = 0; i < (int)this->m_method_a.size(); i++)
    {
        temp.append(QString::fromStdString(this->m_method_a[i]) + "\n");
    }

    ui->pte_method->setPlainText(temp);
    ui->pte_method->setReadOnly(true);
    ui->tW_properties->setItem(0, 1, new QTableWidgetItem(QString::number(this->m_servings)));
    ui->tW_properties->setItem(1, 1, new QTableWidgetItem(QString::number(this->m_weight) + "g"));
    ui->tW_properties->setItem(2, 1, new QTableWidgetItem(QString::number(this->m_calories)));
    ui->tW_properties->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tW_properties->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList ingredient_splitted;
    for (int i = 0; i < (int)this->m_ingredients_a.size(); i++)
    {
        ingredient_splitted = QString::fromStdString(m_ingredients_a[i]).split("-");
        ui->tW_ingredients->insertRow(i);
        ui->tW_ingredients->setItem(i, 0, new QTableWidgetItem(QString::number(ingredient_splitted[0].toDouble())));
        ui->tW_ingredients->setItem(i, 1, new QTableWidgetItem(ingredient_splitted[1]));
    }

    ui->tW_ingredients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tW_ingredients->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //ui->l_photo->setMaximumSize(QSize(320,320));
    if (m_image_file != std::string{})
    {
        QScopedPointer<QPixmap> img{new QPixmap(QString::fromStdString(m_image_file))};
        img->scaled(QSize(ui->l_photo->height(), ui->l_photo->width()), Qt::KeepAspectRatio, Qt::FastTransformation);
        ui->l_photo->setPixmap(*img);
        ui->l_photo->setScaledContents(true);
    }
}
//name|favorite|cuisine|dishtype|servings|calories|weight|in*gredi*ents|met*ho*d
int ReceiptView::ParseLine(QString line, ReceiptView &obj)
{

    QStringList general_reicpe_list = line.split("|");
    QStringList ingredients_list;
    QStringList method_list;

    obj.m_name = general_reicpe_list.value(0).toStdString();
    obj.m_isFavorite = general_reicpe_list.value(1).toInt();
    obj.m_cuisine = general_reicpe_list.value(2).toStdString();
    obj.m_dishtype = general_reicpe_list.value(3).toStdString();

    obj.m_servings = general_reicpe_list.value(4).toUInt();
    obj.m_calories = general_reicpe_list.value(5).toUInt();
    obj.m_weight = general_reicpe_list.value(6).toUInt();

    ingredients_list = general_reicpe_list.value(7).split("*");
    for (int i = 0; i < ingredients_list.size(); i++)
    {
        obj.m_ingredients_a.push_back(ingredients_list.value(i).toStdString());
    }

    method_list = general_reicpe_list.value(8).split("*");
    for (int i = 0; i < method_list.size(); i++)
    {
        obj.m_method_a.push_back(method_list.value(i).toStdString());
    }


    return 0;
}

void ReceiptView::on_pb_calculate_dialog_clicked()
{
    calcDialog->show();
}

void ReceiptView::on_cb_isFavorite_stateChanged(int arg1)
{
    if (arg1)
    {
        emit addToPreferance(this->receiptID);
    }
    else
    {
        emit deleteFromPreferance(this->receiptID);
    }
}


void ReceiptView::showDialog()
{
    calcDialog->show();
    //this->hide();
}

void ReceiptView::on_pb_edit_clicked()
{
    emit editReceipt(this->m_receipt_file, this->m_image_file);
}

void ReceiptView::Parse(const std::string &filename, std::string &receiptName,
                        std::string &cuisine, std::string &dishType,
                        std::vector<std::string> &ingredients_a)
{

    QFile data(QString::fromStdString(filename));
    if (data.open(QIODevice::ReadOnly))
    {
        QTextStream in(&data);

        QString line = in.readLine();

        if (line.length() == 0)
        {
            QMessageBox::warning(nullptr, "Warning!", "File is empty.\nTry to reinstall application.");
            return;
        }

        QStringList general_reicpe_list = line.split("|");

        if (general_reicpe_list.size() != 9)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        QStringList ingredients_list;
        QStringList ingredient_splitted;
        receiptName = general_reicpe_list.value(0).toStdString();

        if (receiptName.length() == 0)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        cuisine = general_reicpe_list.value(2).toStdString();

        if (cuisine.length() == 0)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        dishType = general_reicpe_list.value(3).toStdString();

        if (dishType.length() == 0)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        ingredients_list = general_reicpe_list.value(7).split("*");
        for (int i = 0; i < ingredients_list.size(); i++)
        {
            ingredient_splitted = ingredients_list.value(i).split("-");
            ingredients_a.push_back(ingredient_splitted[1].toStdString());
        }
        data.close();
    }
    else
    {
        QMessageBox::warning(nullptr, "Warning!", "Files are damaged.\nTry to reinstall application.");
        return;
    }
}

void ReceiptView::Parse(const std::string &filename, std::string &receiptName,
                        std::string &cuisine, std::string &dishType,
                        std::vector<std::string> &ingredients_a, std::vector<std::string> &method_a,
                        unsigned &servings, unsigned &weight, unsigned &calories)
{

    QFile data(QString::fromStdString(filename));
    if (data.open(QIODevice::ReadOnly))
    {
        QTextStream in(&data);

        QString line = in.readLine();

        if (line.length() == 0)
        {
            QMessageBox::warning(nullptr, "Warning!", "File is empty.\nTry to reinstall application.");
            return;
        }

        QStringList general_reicpe_list = line.split("|");

        if (general_reicpe_list.size() != 9)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        QStringList ingredients_list;
        QStringList method_list;

        receiptName = general_reicpe_list.value(0).toStdString();

        if (receiptName.length() == 0)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        cuisine = general_reicpe_list.value(2).toStdString();

        if (cuisine.length() == 0)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        dishType = general_reicpe_list.value(3).toStdString();

        if (dishType.length() == 0)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        bool check;
        servings = general_reicpe_list.value(4).toUInt(&check, 10);
        if (!check)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        calories = general_reicpe_list.value(5).toUInt(&check, 10);
        if (!check)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        weight = general_reicpe_list.value(6).toUInt(&check, 10);
        if (!check)
        {
            QMessageBox::warning(nullptr, "Warning!", "File has mistakes.\nTry to reinstall application.");
            return;
        }

        ingredients_list = general_reicpe_list.value(7).split("*");
        for (int i = 0; i < ingredients_list.size(); i++)
        {
            ingredients_a.push_back(ingredients_list.value(i).toStdString());
        }
        method_list = general_reicpe_list.value(8).split("*");
        for (int i = 0; i < method_list.size(); i++)
        {
            method_a.push_back(method_list.value(i).toStdString());
        }
        data.close();
    }
    else
    {
        QMessageBox::warning(nullptr, "Warning!", "Files are damaged.\nTry to reinstall application.");
        return;
    }
}

void ReceiptView::setReceiptId(const std::string &id)
{
    this->receiptID = id;
}

void ReceiptView::setPref(bool pref)
{
    if(pref)
        ui->cb_isFavorite->setCheckState(Qt::CheckState::Checked);
    else
        ui->cb_isFavorite->setCheckState(Qt::CheckState::Unchecked);
}
