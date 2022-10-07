#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
#include <chrono>
#include <QCloseEvent>
#include <QDir>
#include <QFutureWatcher>
#include <database/proxyreceiptsDB.h>
#include <QtConcurrent>
#include <QProgressDialog>
using namespace CookBook;

DownloadDialog::DownloadDialog(ProxyReceiptsDB *db, QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), model(new CookBook::ItemModel), m_db(db)
{
    // TODO: prevent from closing when download
    ui->setupUi(this);
    ui->listView->setModel(model);
    QItemSelectionModel *selectionModel = ui->listView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, [this, selectionModel]()
            {
                const auto selectedRows = selectionModel->selectedRows();
                for (const QModelIndex &index : selectedRows)
                {
                    if (index.isValid())
                    {
                        ui->download_b->setEnabled(true);
                    }
                    else{
                        ui->download_b->setEnabled(false);
                    }
                }
                if(selectedRows.empty())
                    ui->download_b->setEnabled(false); });
    connect(ui->download_b, &QPushButton::released, this, [this, selectionModel]()
            {
                auto i = selectionModel->selectedIndexes();
                
                if(i.size()==0)
                    return;
                ItemModel::ListItem *list = static_cast<ItemModel::ListItem *>(i[0].internalPointer());
                uint32_t tmp_id = list->id;
                selectionModel->clearSelection();
                
                QProgressDialog dialog(this);
                dialog.setLabelText("Please wait for receipt to download...");
                QFutureWatcher<int> futureWatcher;
                QObject::connect(&futureWatcher, &QFutureWatcher<int>::finished, &dialog, &QProgressDialog::reset);
                QObject::connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<int>::cancel);
                QObject::connect(&futureWatcher, &QFutureWatcher<int>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
                QObject::connect(&futureWatcher, &QFutureWatcher<int>::progressValueChanged, &dialog, &QProgressDialog::setValue);
                QFuture<int> fut = QtConcurrent::task([this, tmp_id]
                                                      { return downloadReceipt(tmp_id); })
                                       .spawn();
                futureWatcher.setFuture(fut);
                dialog.exec();
                if (futureWatcher.isCanceled())
                    fut.cancel();
                else
                {
                    futureWatcher.waitForFinished();
                    int ec = fut.result();

                    provideUniquenessItem();
                    switch (ec)
                    {
                    case static_cast<int>(ERROR_CODES::OK):
                        QMessageBox::information(this, "CookBook", "The receipt has been successfuly downloaded.");
                        m_db->save();
                        break;
                    case static_cast<int>(ERROR_CODES::HAS_NOT_NETWORK_CONNECTION):
                        QMessageBox::warning(this, "CookBook", "Has no connection to the network.");
                        break;
                    case static_cast<int>(ERROR_CODES::SERVER_IS_NOT_RESPONDING):
                        QMessageBox::warning(this, "CookBook", "The server is not responding. Please try again or check your network connection.");
                        break;
                    //Temprorary solution
                    case static_cast<int>(ERROR_CODES::RECEIPT_IS_CORRUPTED):
                        QMessageBox::warning(this, "CookBook", "The receipt has been corrupted, please try again or check your network connection.");
                        break;
                    }
                } });
    connect(ui->close_b, &QPushButton::released, this, &DownloadDialog::close);
    if (setUpConnection())
    {
        getAvailebleReceiptsList();
    }
    else
    {
    }
}

DownloadDialog::~DownloadDialog()
{
    closeConnection();
    delete ui;
}

bool DownloadDialog::setUpConnection()
{
    return client.Connect(Settings::host, Settings::port);
}

void DownloadDialog::getAvailebleReceiptsList()
{
    assert(("Client is not connected", client.IsConnected() == true));
    client.sendRequest1();
    auto res1 = client.getResponse1();
    auto dict = res1.make_dict();
    for (auto const &item : dict)
    {
        model->addItem(item.first, QString::fromStdString(item.second));
    }
    provideUniquenessItem();
    emit model->layoutChanged();
}

void DownloadDialog::closeConnection()
{
    client.Disconnect();
}

void DownloadDialog::closeEvent(QCloseEvent *e)
{
    e->accept();
}

int DownloadDialog::downloadReceipt(uint32_t receiptID)
{
    // TODO::Run in thread
    ERROR_CODES res{ERROR_CODES::HAS_NOT_NETWORK_CONNECTION};
    do
    {
        if (client.Connect(Settings::host, Settings::port))
        {
            if (client.sendRequest2(receiptID))
            {
                auto res2 = client.getResponse2();

                QDir dir(Settings::directiryToSave);

                std::string textReceipt{dir.path().toStdString() + "/" + "receipt" + std::to_string(receiptID) + ".txt"};
                std::string image{dir.path().toStdString() + "/" + "receipt" + std::to_string(receiptID) + ".png"};

                ProxyReceiptsDB::DataItem tmp_item;
                tmp_item.filepath = textReceipt;
                tmp_item.imagepath = image;
                tmp_item.prefferd = 0;

                std::string data{res2.TextReceiptToString()};

                if (DownloadDialog::parse(data, tmp_item.receiptName, tmp_item.cuisine,
                                          tmp_item.dishType, tmp_item.ingridients_a))
                {
                    res2.readTextReceipt(textReceipt);
                    res2.readImageReceipt(image);
                    {
                        std::lock_guard<std::mutex> g(mut_db);
                        m_db->addEntry(tmp_item.receiptName, tmp_item);
                    }
                    res = ERROR_CODES::OK;
                    break;
                }
                else
                {
                    res = ERROR_CODES::RECEIPT_IS_CORRUPTED;
                    break;
                }
            }
            else
            {
            }
        }
        else
        {
            break;
        }
    } while (false);
    return static_cast<int>(res);
}

bool DownloadDialog::parse(const std::string &data, std::string &receiptName,
                           std::string &cuisine, std::string &dishType,
                           std::vector<std::string> &ingredients_a)
{
    bool res{false};
    do
    {
        QString str = QString::fromStdString(data);
        QTextStream in(&str);
        QString line = in.readLine();

        if (line.length() == 0)
            break;

        QStringList general_reicpe_list = line.split("|");

        if (general_reicpe_list.size() != 9)
            break;

        QStringList ingredients_list;
        QStringList ingredient_splitted;
        receiptName = general_reicpe_list.value(0).toStdString();

        if (receiptName.length() == 0)
            break;

        cuisine = general_reicpe_list.value(2).toStdString();

        if (cuisine.length() == 0)
            break;

        dishType = general_reicpe_list.value(3).toStdString();

        if (dishType.length() == 0)
            break;

        ingredients_list = general_reicpe_list.value(7).split("*");
        for (int i = 0; i < ingredients_list.size(); i++)
        {
            ingredient_splitted = ingredients_list.value(i).split("-");
            ingredients_a.push_back(ingredient_splitted[1].toStdString());
        }
        res = true;
    } while (false);
    return res;
}

void DownloadDialog::provideUniquenessItem()
{

    if (m_db != nullptr)
    {
        QVector<uint32_t> toDelete;
        auto arr = m_db->getNames();
        int modelItemsNumber = model->rowCount();
        for (auto const &item : arr)
        {
            for (int i{0}; i < modelItemsNumber; ++i)
            {
                const ItemModel::ListItem *l = static_cast<const ItemModel::ListItem *>(model->index(i, 0).internalPointer());
                if (item.second == (l->receiptName).toStdString())
                    toDelete.push_back(l->id);
            }
        }
        for (auto const &id : toDelete)
            model->removeItem(id);
    }

    emit model->layoutChanged();
}
