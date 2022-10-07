#include "common_headers.h"
#include "types_of_messages.h"
namespace CookBook
{
    namespace network
    {
        class connection : public std::enable_shared_from_this<connection>
        {
        public:
            connection(asio::io_context &context_, asio::ip::tcp::socket socket_, const response_message_1 &res1_,
                       const std::map<uint64_t, CookBook::network::ReceiptItem> &data_)
                : m_context(context_), m_socket(std::move(socket_)), m_data(data_)
            {
                res1_buffer = res1_.data();
            }

            virtual ~connection()
            {
            }

        public:
            void connectToClient(uint64_t clientID)
            {
                id = clientID;
                if (m_socket.is_open())
                {
                    readHeader();
                }
            }
            void disconnect()
            {
                if (isConnected())
                    asio::post(m_context, [this]()
                               { m_socket.close(); });
            }
            bool isConnected() const
            {
                return m_socket.is_open();
            }

        public:
            bool send(const basic_message *msg);
            uint64_t getID() const
            {
                return id;
            }

        protected:
            void write();
            void readHeader()
            {
                asio::async_read(m_socket, asio::buffer(reinterpret_cast<char *>(&tmp_type), sizeof(uint64_t)),
                                 [this](std::error_code ec, std::size_t length)
                                 {
                                     if (!ec)
                                     {
                                         if (tmp_type == static_cast<uint64_t>(CookBook::network::MessageTypes::REQUEST1))
                                         {
#ifdef SERVER_DEBUG
                                             std::cout << "Message of the type 1 received\n";
#endif
                                             writeReceiptIDs();
                                         }
                                         else if (tmp_type == static_cast<uint64_t>(CookBook::network::MessageTypes::REQUEST2))
                                         {
#ifdef SERVER_DEBUG
                                             std::cout << "Message of the type 2 received\n";
#endif
                                             readBody2();
                                         }
#ifdef SERVER_DEBUG
                                         std::cout << tmp_type << '\n';
#endif
                                     }
                                     else
                                     {
                                     }
                                 });
            }
            void readBody2()
            {
                asio::async_read(m_socket, asio::buffer(reinterpret_cast<char *>(&tmp_id), sizeof(uint64_t)),
                                 [this](std::error_code ec, std::size_t length)
                                 {
                                     if (handleReceiptID())
                                         writeReceipt();
                                     else
                                         disconnect();
                                 });
            }

            bool handleReceiptID()
            {
                auto i = m_data.find(tmp_id);
                bool res{false};
                if (i != m_data.end())
                {
                    res2.write(i->second.TextFileName, i->second.ImageFileName);
                    res2_buffer = res2.data();
#ifdef SERVER_DEBUG
                    std::cout << res2 << '\n';
#endif
                    res = true;
                }
                return res;
            }
            void writeReceiptIDs()
            {
                asio::async_write(m_socket, asio::buffer(res1_buffer.data(), res1_buffer.size()),
                                  [this](std::error_code ec, std::size_t length)
                                  {
                                      if (!ec)
                                          disconnect();
                                      else
                                      {
#ifdef SERVER_DEBUG
                                          std::cout << ec.message();
#endif
                                          disconnect();
                                      }
                                  });
            }
            void writeReceipt()
            {
                asio::async_write(m_socket, asio::buffer(res2_buffer.data(),res2_buffer.size()),
                                  [this](std::error_code ec, std::size_t length)
                                  {
                                      if (!ec)
                                          disconnect();
                                      else
                                      {
#ifdef SERVER_DEBUG
                                          std::cout << ec.message();
#endif
                                          disconnect();
                                      }
                                  });
            }

        protected:
            asio::ip::tcp::socket m_socket;
            asio::io_context &m_context;

            uint64_t tmp_type;
            uint64_t tmp_id;

            response_message_2 res2;
            std::vector<char> res1_buffer;
            std::vector<char> res2_buffer;

            const std::map<uint64_t, CookBook::network::ReceiptItem> &m_data;
            uint64_t id{0};
        };
    }
}