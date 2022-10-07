#include "server.h"

int main()
{
    try
    {
        CookBook::network::Server server(10000, "./data/storage_info.txt");
        server.start();
    }
    catch (const std::runtime_error &e)
    {
#ifdef SERVER_DEBUG
        std::cout << e.what();
#endif
    }
    return 0;
}