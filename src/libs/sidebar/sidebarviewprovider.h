#ifndef _SIDEBARPROVIDER_M_
#define _SIDEBARPROVIDER_M_


namespace CookBook
{
    class SideBarView;

    class SideBarViewProvider
    {
        
    public:
        explicit SideBarViewProvider();
        virtual SideBarView* veiw() const = 0;
        virtual ~SideBarViewProvider();
    private:

    };
}

#endif