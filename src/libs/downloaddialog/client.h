#ifndef CLIENT
#define CLIENT

#include "common_headers.h"
#include "types_of_messages.h"
namespace CookBook
{
    namespace network
    {
        class Client
        {
        public:
            Client()
            {
            }
            ~Client()
            {
                Disconnect();
                delete m_socket;
            }
            bool sendRequest1()
            {
                bool res;
                do
                {
                    asio::error_code ec;
                    uint64_t type = static_cast<uint64_t>(CookBook::network::MessageTypes::REQUEST1);
                    const char *buffer = reinterpret_cast<const char *>(&type);
                    asio::write(*m_socket, asio::buffer(buffer, sizeof(uint64_t)), ec);

                    if (ec)
                    {
                        break;
                    }
                } while (false);

                return res;
            }
            CookBook::network::response_message_1 getResponse1()
            {
                CookBook::network::response_message_1 res1;
                res1.readFromSocket(*m_socket);
                return res1;
            }
            bool sendRequest2(uint64_t receiptId)
            {
                bool res{false};
                do
                {
                    asio::error_code ec;
                    uint64_t type = static_cast<uint64_t>(CookBook::network::MessageTypes::REQUEST2);
                    
                    char buffer[2*sizeof(uint64_t)];
                    std::memcpy(buffer, &type, sizeof(uint64_t));
                    std::memcpy(buffer + sizeof(uint64_t), &receiptId, sizeof(uint64_t));
                    asio::write(*m_socket, asio::buffer(buffer, 2*sizeof(uint64_t)), ec);
                    if (!ec)
                    {
                        res = true;
                    }

                } while (false);
                return res;
            }

            CookBook::network::response_message_2 getResponse2()
            {
                CookBook::network::response_message_2 res2;
                res2.readFromSocket(*m_socket);
                return res2;

            }

            bool Connect(const std::string &host, uint64_t port)
            {
                try
                {
                    asio::ip::tcp::resolver resolver(m_asioContext);
                    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
                    m_socket = new asio::ip::tcp::socket(m_asioContext);
                    asio::connect(*m_socket, endpoints);
                }
                catch (std::exception &e)
                {
                    return false;
                }
                return true;
            }
            void Disconnect()
            {
                if (IsConnected())
                {
                    m_socket->close();
                }
            }
            bool IsConnected()
            {
                return m_socket->is_open();
            }

        private:
            asio::io_context m_asioContext;
            asio::ip::tcp::socket *m_socket;
        };
    }
}

#endif