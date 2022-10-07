# CookBook

CookBook is a simple receipt viewer which you can use to watch receipts and create your own with ease. For now it has only build for Linux kernel. But in the feature it would be availbable for Windows as well.

# Installation
You can install AppImage of the application for x86-64 right away. If you want to build it on your own see build section.


# Build

First, get a copy of the repository, you can do it with command
```
git clone https://github.com/Bogdan741/cookbook.git
```

or just download a copy of archive and then extract the content to a folder.

Next step is to build a project. But you need to set CMAKE_PREFIX_PATH so it would provide path to your Qt library.
```
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
make
make install DESTDIR=app
```


# Notes
The server folder contains server code which is expect to be running to be able to download the receipts.
