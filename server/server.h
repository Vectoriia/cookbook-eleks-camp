#include "common_headers.h"
#include "connection.h"

namespace CookBook
{
    namespace network
    {
        class Server
        {
        public:
            Server(uint64_t portNumber, const std::string &storageInfo, const std::string &response1 = "./data/receipt_id.txt")
                : m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), portNumber))
            {
                if (!initializeReceiptFilesArray(storageInfo))
                {
                    throw std::runtime_error("Receipts are not provided");
                }
                res1.write(response1);
            }

            ~Server()
            {
                stop();
            }

            bool start()
            {
                try
                {
                    waitForClientConnection();
                    m_asioContext.run();
                }
                catch (const std::exception &e)
                {
// TODO:write something to log
#ifdef SERVER_DEBUG
                    std::cout << "SERVER: EXCEPTION: " << e.what() << '\n';
                    return false;
#endif
                }
#ifdef SERVER_DEBUG
                std::cout << "SERVER: STARTED\n";
#endif
                return true;
            }

            void stop()
            {

                m_asioContext.stop();
#ifdef SERVER_DEBUG
                std::cout << "SERVER: STOPPED\n";
#endif
            }

            void waitForClientConnection()
            {
                m_asioAcceptor.async_accept(
                    [this](std::error_code ec, asio::ip::tcp::socket socket)
                    {
                        if (!ec)
                        {
#ifdef SERVER_DEBUG
                            std::cout << "SERVER: New connection successed\n";
#endif
                            std::shared_ptr<connection> newConnection =
                                std::make_shared<connection>(m_asioContext, std::move(socket), res1, m_data);
                            if (connectionAllowed())
                            {
                                m_connection = std::move(newConnection);
                                m_connection->connectToClient(++nIDCounter);
#ifdef SERVER_DEBUG
                                if (m_connection)
                                    std::cout << "Connection ID: " << m_connection->getID() << " approved\n";
#endif
                            }
                        }
                        else
                        {
#ifdef SERVER_DEBUG
                            std::cout << "SERVER: New connection failed\n." << ec.message() << '\n';
#endif
                        }
                        waitForClientConnection();
                    });
            }

            bool initializeReceiptFilesArray(const std::string &filename)
            {
                bool res{false};
                std::ifstream os(filename);
                do
                {
                    if (os.is_open())
                    {
                        std::string str;
                        while (getline(os, str))
                        {
                            std::stringstream ss(str);

                            uint64_t id;
                            std::string textFile, imgFile;
                            if (ss >> id >> textFile >> imgFile)
                            {
                                m_data[id] = ReceiptItem{textFile, imgFile};
                            }
                        }
                        res = true;
                    }

                } while (false);
                return res;
            }

        public:
        protected:
            bool connectionAllowed()
            {
                return true;
            }

        private:
            std::shared_ptr<connection> m_connection;
            asio::io_context m_asioContext;

            uint64_t nIDCounter{10000};
            asio::ip::tcp::acceptor m_asioAcceptor;
            std::map<uint64_t, ReceiptItem> m_data;

            response_message_1 res1;
        };
    }
}