#ifndef _SETTINGS_COOKBOOK_
#define _SETTINGS_COOKBOOK_

namespace CookBook
{
    namespace Settings
    {
        //Mainwindow
        constexpr char icon_path[] = "./data/images/window_icon.png";
        constexpr char created_folder []= "./data/created";
        constexpr char welcome_page[] = "./data/images/welcomepage.png";

        //DownloadDilaog
        constexpr char host[] = "127.0.0.1";
        constexpr uint64_t port = 10000;
        constexpr char directiryToSave[] = "./data";

        //ProxyreceiptsDB
        constexpr char dataBaseFile[] = "./data/entries.txt";

    }
}
#endif
