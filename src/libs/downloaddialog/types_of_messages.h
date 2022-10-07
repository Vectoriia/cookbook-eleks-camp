#ifndef MESSAGES
#define MESSAGES

#include <utility>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include "common_headers.h"

namespace CookBook
{
    namespace network
    {
        enum class MessageTypes : uint64_t
        {
            REQUEST1 = 1,
            RESPONSE1,
            REQUEST2,
            RESPONSE2,
        };

        struct basic_message_header
        {
            basic_message_header(uint64_t type_) : type(type_)
            {
            }
            uint64_t type;
        };

        struct request_message_header_1 : basic_message_header
        {
            request_message_header_1() : basic_message_header(static_cast<uint64_t>(MessageTypes::REQUEST1))
            {
            }
        };

        struct request_message_header_2 : basic_message_header
        {
            request_message_header_2() : basic_message_header(static_cast<uint64_t>(MessageTypes::REQUEST2))
            {
            }
            uint64_t receipts_id;
        };

        struct response_message_header_1 : basic_message_header
        {
            response_message_header_1() : basic_message_header(static_cast<uint64_t>(MessageTypes::RESPONSE1))
            {
            }
            uint64_t dataSize;
        };

        struct response_message_header_2 : basic_message_header
        {
            response_message_header_2() : basic_message_header(static_cast<uint64_t>(MessageTypes::RESPONSE2))
            {
            }
            uint64_t textfile_size;
            uint64_t imagefile_size;
        };

        struct basic_message
        {
            virtual uint64_t size() const = 0;
            virtual ~basic_message();
        };
        //---------------------------------
        class request_message_1 : public basic_message
        {
        public:
            uint64_t size() const override
            {
                return sizeof(header);
            }

        private:
            request_message_header_1 header;
        };
        //---------------------------------

        class request_message_2 : public basic_message
        {
        public:
            uint64_t size() const override
            {
                return sizeof(header) + sizeof(receipts_id);
            }
            request_message_2(uint64_t id) : receipts_id(id)
            {
            }

        private:
            request_message_header_2 header;
            uint64_t receipts_id;
        };
        //---------------------------------

        class response_message_1 //:public basic_message
        {
        public:
            response_message_1()
            {
            }

            void write(const std::string &filename)
            {
                std::ifstream file(filename);
                if (file.is_open())
                {
                    file.seekg(0, file.end);
                    uint64_t length = file.tellg();
                    file.seekg(0, file.beg);

                    header.dataSize = length;
                    try
                    {
                        buffer.reserve(length);
                        std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(buffer));
                        buffer.shrink_to_fit();
                    }
                    catch (const std::bad_alloc &e)
                    {
                        //TODO write something to log
                        file.close();
                        throw e;
                    }
                    file.close();
                }
                else
                {
                    header.dataSize = 0;
                }
            }

            uint64_t size() const
            {
                return sizeof(header.dataSize) + sizeof(header.type) + sizeof(char) * buffer.size();
            }

            //For debugging purpose
            friend std::ostream &operator<<(std::ostream &os, const response_message_1 &msg)
            {
                os << "Type: " << msg.header.type << " ;"
                   << "Size: " << msg.header.dataSize << " .\n";
                return os;
            }

            friend response_message_1 &operator<<(response_message_1 &msg, char *buffer)
            {
                //Expect buffer to represent response_message_1, otherwise undifined behaviour
                uint64_t type;
                uint64_t dataSize;
                std::vector<char> tmp_arr;
                std::memcpy(&type, buffer, sizeof(type));
                std::memcpy(&dataSize, buffer + sizeof(type), sizeof(dataSize));
                try
                {
                    tmp_arr.reserve(dataSize);
                    copy(buffer + sizeof(type) + sizeof(dataSize), buffer + sizeof(type) + sizeof(dataSize) + dataSize, std::back_inserter(tmp_arr));
                }
                catch (const std::bad_alloc &e)
                {
                    //TODO: write something to log
                    throw e;
                }
                msg.header.type = type;
                msg.header.dataSize = dataSize;
                msg.buffer = std::move(tmp_arr);

                return msg;
            }

            // friend response_message_1& operator >>(response_message_1 & msg, char * buffer)
            // {
            //     std::memcpy(buffer, &msg.header.type, sizeof(msg.header.type));
            //     std::memcpy(buffer + sizeof(msg.header.type),&msg.header.dataSize,sizeof(msg.header.dataSize));
            //     std::memcpy(buffer + sizeof(msg.header.dataSize) + sizeof(msg.header.type), msg.buffer.data(), msg.header.dataSize);
            // }

            friend response_message_1 &operator>>(response_message_1 &msg, std::vector<char> &obuffer)
            {
                char *type = reinterpret_cast<char *>(&msg.header.type);
                char *dataSize = reinterpret_cast<char *>(&msg.header.dataSize);

                try
                {
                    std::copy(type, type + sizeof(msg.header.type), std::back_inserter(obuffer));
                    std::copy(dataSize, dataSize + sizeof(msg.header.dataSize), std::back_inserter(obuffer));
                    std::copy(msg.buffer.begin(), msg.buffer.end(), std::back_inserter(obuffer));
                }
                catch (const std::bad_alloc &e)
                {
                    throw e;
                }
                return msg;
            }

            std::vector<char> data() const
            {
                // char type[sizeof(uint64_t)];
                // char dataSize[sizeof(uint64_t)];
                // std::memcpy(type, &header.type, sizeof(uint64_t));
                // std::memcpy(dataSize, &header.dataSize, sizeof(uint64_t));

                const char *type = reinterpret_cast<const char *>(&header.type);
                const char *dataSize = reinterpret_cast<const char *>(&header.dataSize);
                std::vector<char> info;
                info.reserve(header.dataSize + sizeof(header.dataSize) + sizeof(header.type));

                std::copy(type, type + sizeof(header.type), std::back_inserter(info));
                std::copy(dataSize, dataSize + sizeof(header.dataSize), std::back_inserter(info));
                std::copy(buffer.begin(), buffer.end(), std::back_inserter(info));

                return info;
            }

            bool readFromSocket(asio::ip::tcp::socket &socket)
            {
                bool res{false};
                do
                {
                    asio::error_code ec;
                    uint64_t type;
                    uint64_t size;
                    char *typeBuf = reinterpret_cast<char *>(&type);
                    char *sizeBuf = reinterpret_cast<char *>(&size);

                    asio::read(socket, asio::buffer(typeBuf, sizeof(uint64_t)), ec);
                    asio::read(socket, asio::buffer(sizeBuf, sizeof(uint64_t)), ec);

                    header.type = type;
                    header.dataSize = size;

                    this->buffer.reserve(size);
                    char *buf = new char[size];
                    asio::read(socket, asio::buffer(buf, size), ec);
                    std::copy(buf, buf + size, std::back_inserter(this->buffer));
                    if (!ec)
                        res = true;
                    delete[] buf;
                } while (false);
                return res;
            }

            std::map<uint64_t, std::string> make_dict() const
            {
                std::map<uint64_t, std::string> res;
                std::stringstream ss{buffer.data()};
                while (ss)
                {
                    uint64_t key;
                    std::string str;
                    ss >> key;
                    ss.get();
                    getline(ss, str, '\n');
                    if (str != std::string{})
                        res[key] = str;
                }
                return res;
            }

        public:
            struct Item
            {
                uint64_t receipt_id;
                std::string receiptName;
            };

        private:
            std::vector<char> buffer;
            response_message_header_1 header;
        };
        //---------------------------------

        class response_message_2
        {
        public:
            response_message_2()
            {
            }
            void write(const std::string &textfile, const std::string &imagefile)
            {

                //TODO: Do managing resources properly
                std::ifstream txF(textfile);
                //handle TextFile
                if (txF.is_open())
                {
                    //Get file size
                    txF.seekg(0, txF.end);
                    uint64_t length = txF.tellg();
                    txF.seekg(0, txF.beg);
                    header.textfile_size = length;

                    //Copy file to buffer
                    char *arr = new char[length];
                    txF.read(arr, length);

                    buffer.reserve(header.textfile_size);
                    std::copy(arr, arr + length, std::back_inserter(buffer));
                    delete[] arr;
                    txF.close();
                }
                else
                {
                    header.textfile_size = 0;
                }

                std::ifstream imF(imagefile, std::ifstream::binary);
                if (imF.is_open())
                {
                    //Get file size
                    imF.seekg(0, imF.end);
                    uint64_t length = imF.tellg();
                    imF.seekg(0, imF.beg);
                    header.imagefile_size = length;

                    //Copy file to buffer
                    char *arr = new char[length];
                    imF.read(arr, length);
                    buffer.reserve(header.textfile_size + header.imagefile_size);
                    std::copy(arr, arr + length, std::back_inserter(buffer));
                    delete[] arr;
                    imF.close();
                }
                else
                {
                    header.imagefile_size = 0;
                }
                buffer.shrink_to_fit();
            }

            uint64_t size() const
            {
                return sizeof(header.type) + sizeof(header.textfile_size) + sizeof(header.imagefile_size) + sizeof(char) * buffer.size();
            }
            //For debuging purpose
            friend std::ostream &operator<<(std::ostream &os, const response_message_2 msg)
            {
                os << "ID: " << int(msg.header.type) << "Text file size: " << msg.header.textfile_size
                   << "Image file size: " << msg.header.imagefile_size << "\n";
                return os;
            }

            friend response_message_2 &operator>>(response_message_2 &msg, std::vector<char> &buffer)
            {
                //Expect buffer to be the size not less than the size of the message, otherwise undifuned behaviour
                char *type = reinterpret_cast<char *>(&msg.header.type);
                char *textSize = reinterpret_cast<char *>(&msg.header.textfile_size);
                char *imageSize = reinterpret_cast<char *>(&msg.header.imagefile_size);

                try
                {
                    std::copy(type, type + sizeof(msg.header.type), std::back_inserter(buffer));
                    std::copy(textSize, textSize + sizeof(msg.header.textfile_size), std::back_inserter(buffer));
                    std::copy(imageSize, imageSize + sizeof(msg.header.imagefile_size), std::back_inserter(buffer));
                    std::copy(msg.buffer.begin(), msg.buffer.end(), std::back_inserter(buffer));
                }
                catch (const std::bad_alloc &e)
                {
                    throw e;
                }

                return msg;
            }

            friend response_message_2 &operator<<(response_message_2 &mes, char *buffer)
            {
                //Expext buffer to represent response_message_2, otherwise undifined behaviour
                uint64_t type;
                uint64_t textFileSize;
                uint64_t imageFileSize;

                std::memcpy(&type, buffer, sizeof(type));
                std::memcpy(&textFileSize, buffer + sizeof(type), sizeof(textFileSize));
                std::memcpy(&imageFileSize, buffer + sizeof(type) + sizeof(textFileSize), sizeof(imageFileSize));

                std::vector<char> arr{};
                arr.reserve(textFileSize + imageFileSize);
                try
                {
                    std::copy(buffer + sizeof(type) + sizeof(textFileSize) + sizeof(imageFileSize), buffer + sizeof(type) + sizeof(textFileSize) + sizeof(imageFileSize) + textFileSize + imageFileSize, std::back_inserter(arr));
                }
                catch (const std::bad_alloc &e)
                {
                    //TODO: write something to log
                    throw e;
                }

                mes.header.type = type;
                mes.header.textfile_size = textFileSize;
                mes.header.imagefile_size = imageFileSize;
                mes.buffer = std::move(arr);
                return mes;
            }

            std::string TextReceiptToString() const
            {
                std::string str;
                str.reserve(header.textfile_size);
                std::copy(buffer.begin(), buffer.begin() + header.textfile_size, std::back_inserter(str));
                return str;
            }
            bool readTextReceipt(const std::string &testFileName) const
            {
                bool state{false};
                std::ofstream of(testFileName);
                if (of.is_open())
                {
                    of.write(buffer.data(), header.textfile_size);
                    if (of.good())
                        state = true;
                    of.close();
                }
                return state;
            }

            bool readImageReceipt(const std::string &imageFileNama) const
            {
                bool state{false};
                std::ofstream of(imageFileNama, std::ofstream::binary);
                if (of.is_open())
                {
                    of.write(buffer.data() + header.textfile_size, header.imagefile_size);
                    if (of.good())
                        state = true;
                    of.close();
                }
                return state;
            }

            std::vector<char> data() const
            {
                const char *type = reinterpret_cast<const char *>(&header.type);
                const char *textSize = reinterpret_cast<const char *>(&header.textfile_size);
                const char *imageSize = reinterpret_cast<const char *>(&header.imagefile_size);

                std::vector<char> info;
                std::copy(type, type + sizeof(header.type), std::back_inserter(info));
                std::copy(textSize, textSize + sizeof(header.textfile_size), std::back_inserter(info));
                std::copy(imageSize, imageSize + sizeof(header.imagefile_size), std::back_inserter(info));
                std::copy(buffer.begin(), buffer.end(), std::back_inserter(info));

                return info;
            }

            bool readFromSocket(asio::ip::tcp::socket &socket)
            {
                bool res;
                do
                {
                    asio::error_code ec;
                    uint64_t type = 0;
                    uint64_t TextSize = 0;
                    uint64_t ImageSize = 0;

                    char *typeBuf = reinterpret_cast<char *>(&type);
                    char *TextSizeBuf = reinterpret_cast<char *>(&TextSize);
                    char *ImageSizeBuf = reinterpret_cast<char *>(&ImageSize);

                    asio::read(socket, asio::buffer(typeBuf, sizeof(uint64_t)), ec);
                    asio::read(socket, asio::buffer(TextSizeBuf, sizeof(uint64_t)), ec);
                    asio::read(socket, asio::buffer(ImageSizeBuf, sizeof(uint64_t)), ec);

                    header.type = type;
                    header.textfile_size = TextSize;
                    header.imagefile_size = ImageSize;
                    uint64_t size = TextSize + ImageSize;
                    this->buffer.reserve(size);

                    char *buf = new char[size];
                    asio::read(socket, asio::buffer(buf, size), ec);
                    std::copy(buf, buf + size, std::back_inserter(this->buffer));
                    if (!ec)
                        res = true;
                    delete[] buf;
                } while (false);

                return res;
            }

        private:
            response_message_header_2 header;
            std::vector<char> buffer;
        };
        //---------------------------------

        struct ReceiptItem
        {
            std::string TextFileName;
            std::string ImageFileName;
        };
    }
}
#endif