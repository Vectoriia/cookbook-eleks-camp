#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <database/proxyreceiptsDB.h>
#include <ui/settings.h>
#include "listmodel.h"
#include "client.h"
#include <thread>
#include <future>
#include <QFuture>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Dialog;
}

namespace CookBook
{
    class ProxyReceiptsDB;

}
QT_END_NAMESPACE
namespace CookBook
{
    class DownloadDialog : public QDialog
    {
        Q_OBJECT

    public:
        DownloadDialog(ProxyReceiptsDB *db, QWidget *parent = nullptr);
        ~DownloadDialog();
        void closeEvent(QCloseEvent *e) override;
        static bool parse(const std::string &data, std::string &receiptName,
                          std::string &cuisine, std::string &dishType, std::vector<std::string> &ingridient_a);

        void closeConnection();
        bool isConnctedToNetwork();
        
    private:
        bool setUpConnection();
        void getAvailebleReceiptsList();
        int downloadReceipt(uint32_t receiptID);
        bool checkIfAvailableToDownload();
        void provideUniquenessItem();
    public:
    enum class ERROR_CODES : int
    {
        OK=1,
        HAS_NOT_NETWORK_CONNECTION,
        SERVER_IS_NOT_RESPONDING,
        RECEIPT_IS_CORRUPTED,
    };
    private:
        Ui::Dialog *ui;
        ProxyReceiptsDB *m_db;
        CookBook::ItemModel *model;
        CookBook::network::Client client;


        //! Now it is able to download only one receipt at a time. But it's going to be changed in the future so it would be possible to download more than one receipt at a time

        mutable std::mutex mut_db; // For m_db object to prevent data race
        // std::future<int> downloadRes; 
        // std::thread m_thread;      
    };
}
#endif // DIALOG_H
