#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSplitter>
#include <QtWidgets>
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <database/proxyreceiptsDB.h>
#include <receiptview/receiptview.h>
#include <createreceiptview/createreceiptview.h>
#include <viewwidget/viewwidget.h>
#include <sidebar/sidebar.h>
#include <searchdialog/searchdialog.h>
#include <calculatedialog/calculatedialog.h>
#include <downloaddialog/dialog.h>
#include <viewwidget/viewwidget.h>
#include <settings.h>
#include <QTimer>

#define DEBUG

using namespace CookBook;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    // setStyleSheet("background-color: #282C34;");
    // TODO: Change
    this->setWindowIcon(QIcon(Settings::icon_path));

    this->menuBar()->setStyleSheet("background-color:" + QString("#8B95C9") + ";");
    this->setWindowTitle("CookBook");

    // Setting splitter
    QSize rec = this->size();
    auto nx = rec.width();
    ui->splitter->setSizes({nx / 9, nx - nx / 9});
    ui->splitter->setCollapsible(0, false);
    ui->splitter->setCollapsible(1, false);

    m_db = new ProxyReceiptsDB(Settings::dataBaseFile);

    // Setting SideBar
    ui->centralwidget->setContentsMargins(0, 0, 0, 0);
    m_sideBar = new SideBar(m_db);
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_sideBar);
    ui->trw->setLayout(layout);

    auto _widT = createWelcomePage();
    this->m_viewWidget_a.push_back(_widT);

    ui->tabWidget->addTab(_widT, tr("Welcome page"));

    connect(m_sideBar, &SideBar::displayView, this, &MainWindow::open_view);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index)
            {
                if (ui->tabWidget->count() != 1 && (this->m_viewWidget_a[index]->getEditableStatus()&&!(this->m_viewWidget_a[index]->getSaveStatus())))
                {
                    emit attemptToCloseEditableTab(index);
                }
                else if (ui->tabWidget->count() != 1)
                {
                    QTabWidget *temp = static_cast<QTabWidget *>(ui->tabWidget->widget(index));
                    this->m_viewWidget_a.erase( m_viewWidget_a.begin() + index);
                    ui->tabWidget->removeTab(index);
                    if (temp)
                        delete temp;
                } });
    timer = new QTimer{};
    timer->setInterval(10 * 60 * 1000);
    timer->start();
    connect(timer, &QTimer::timeout, m_db, &ProxyReceiptsDB::save);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_db;
    delete timer;
}

void MainWindow::on_actionDownload_recipe_triggered()
{
    QScopedPointer<DownloadDialog> down(new DownloadDialog(m_db, this));
    down->exec();
}

void MainWindow::on_actionCreate_new_receipt_triggered()
{
    bool isEditablePage = false;
    for (int i = 0; i < ui->tabWidget->count(); i++)
    {
        if (m_viewWidget_a[i]->getEditableStatus())
        {
            isEditablePage = true;
            QMessageBox::information(this, "Appologize", "You can open only one editable page yet.\nYou can close previous one and try again");
            return;
        }
    }
    auto createReceiptView = new CreateReceiptVeiw{this};

    connect(createReceiptView, &CreateReceiptVeiw::tabWasSaved, this, &MainWindow::close_current_tab);
    connect(this, &MainWindow::attemptToCloseEditableTab, createReceiptView, &CreateReceiptVeiw::confirm_closing_tab);
    connect(createReceiptView, &CreateReceiptVeiw::tabEditableWasSaved, this, &MainWindow::close_editable_tab);
    connect(createReceiptView, &CreateReceiptVeiw::closingEditabletabConfirmed, this, &MainWindow::close_editable_tab);

    connect(this, &MainWindow::attemptToSaveEditableTab, createReceiptView, &CreateReceiptVeiw::saving);
    ui->tabWidget->addTab(createReceiptView, tr("Your receipt"));

    this->m_viewWidget_a.push_back(createReceiptView);
    this->m_viewWidget_a[m_viewWidget_a.size() - 1]->setSavedStatus(false);
    this->m_viewWidget_a[m_viewWidget_a.size() - 1]->setEditableStatus(true);

    ui->tabWidget->setCurrentWidget(createReceiptView);
}

void MainWindow::on_actionFind_triggered()
{
    QScopedPointer<SearchDialog> searchD(new SearchDialog(this, m_db));
    connect(searchD.get(), &SearchDialog::displayView, this, &MainWindow::open_view);
    searchD->exec();
}

void MainWindow::on_actionOpen_triggered()
{
    QString filter = tr("Text (*.txt)");
    QString filedirection = QFileDialog::getOpenFileName(this, tr("Open receipt"), QDir::homePath(), tr("Text (*.txt)"), &filter);

    if (filedirection.isEmpty())
    {
        QMessageBox::information(nullptr, "Opening denied", "You haven't choose suitable file.");
        // QMessageBox::information(this, "Appologize", "Have not added the feature yet...");
    }
    else
    {
        std::string filename = filedirection.toStdString();
        std::string tabname = "Your receipt";

        auto receiptVeiw = new ReceiptView{this, 100, filename, "disable"};

        // favorite is unable

        connect(receiptVeiw, &ReceiptView::closeTab, this, &MainWindow::close_current_tab);
        connect(receiptVeiw, &ReceiptView::editReceipt, this, &MainWindow::create_edit_tab);

        int current_index = ui->tabWidget->currentIndex();
        this->m_viewWidget_a.erase(m_viewWidget_a.begin() + current_index);
        ui->tabWidget->removeTab(current_index);

        ui->tabWidget->insertTab(current_index, receiptVeiw, QString{tabname.c_str()});
        this->m_viewWidget_a.insert(current_index,receiptVeiw);
        ui->tabWidget->setCurrentWidget(receiptVeiw);
    }
}

void MainWindow::on_actionNew_Tab_triggered()
{
    auto _wid = createWelcomePage();

    int index = ui->tabWidget->addTab(_wid, tr("Welcome page"));
    this->m_viewWidget_a.push_back(_wid);
    ui->tabWidget->setCurrentWidget(_wid);
}

void MainWindow::on_actionSave_triggered()
{
    if (this->m_viewWidget_a[ui->tabWidget->currentIndex()]->getEditableStatus())
    {
        if (this->m_viewWidget_a[ui->tabWidget->currentIndex()]->getSaveStatus())
            emit attemptToSaveEditableTab(false);
        else if (!this->m_viewWidget_a[ui->tabWidget->currentIndex()]->getSaveStatus())
            emit attemptToSaveEditableTab(true);
    }
    else
    {
        QMessageBox::information(this, "Appologize", "You can save only edited or new receipt.");
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    if (this->m_viewWidget_a[ui->tabWidget->currentIndex()]->getEditableStatus())
    {

        emit attemptToSaveEditableTab(true);
    }
    else
    {
        QMessageBox::information(this, "Appologize", "You can save only edited or new receipt.");
    }
}

void MainWindow::on_actionClose_Tab_triggered()
{
    int index = ui->tabWidget->currentIndex();
    if (ui->tabWidget->count() != 1 && this->m_viewWidget_a[index]->getEditableStatus() && !(this->m_viewWidget_a[index]->getSaveStatus()))
    {
        emit attemptToCloseEditableTab(index);
    }
    else if (ui->tabWidget->count() != 1)
    {
        QTabWidget *temp = static_cast<QTabWidget *>(ui->tabWidget->widget(index));
        ui->tabWidget->removeTab(index);
        this->m_viewWidget_a.erase(m_viewWidget_a.begin() + index);
        if (temp)
            delete temp;
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::open_view(const QString &id)
{
    std::string filename = *(m_db->getFilePath(id.toStdString()));
    std::string tabname = *(m_db->getName(id.toStdString()));

    std::string image_filename = *(m_db->getImagePath(id.toStdString()));

    auto receiptVeiw = new ReceiptView{this, 100, filename, image_filename};

    receiptVeiw->setReceiptId(id.toStdString());
    receiptVeiw->setPref(*(m_db->isPreferred(id.toStdString())));
    connect(receiptVeiw, &ReceiptView::addToPreferance, this, [this](const std::string &id)
            { this->m_db->changePreference(id, 1); });
    connect(receiptVeiw, &ReceiptView::deleteFromPreferance, this, [this](const std::string &id)
            { this->m_db->changePreference(id, 0); });

    connect(receiptVeiw, &ReceiptView::closeTab, this, &MainWindow::close_current_tab);
    connect(receiptVeiw, &ReceiptView::editReceipt, this, &MainWindow::create_edit_tab);
    // TODO:Check if everything is saved
    // TODO: Actually it would not delete it, so be carefull to delete it

    int current_index = ui->tabWidget->currentIndex();
    this->m_viewWidget_a.erase(m_viewWidget_a.begin() + current_index);
    ui->tabWidget->removeTab(current_index);

    ui->tabWidget->insertTab(current_index, receiptVeiw, QString{tabname.c_str()});
    this->m_viewWidget_a.insert(current_index,receiptVeiw);

    ui->tabWidget->setCurrentWidget(receiptVeiw);
}

void MainWindow::close_current_tab()
{
 int current_index = ui->tabWidget->currentIndex();
    if (ui->tabWidget->count() != 1)
    {
        // ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), "Saved");
        if (this->m_viewWidget_a[current_index]->getEditableStatus() && !(this->m_viewWidget_a[current_index]->getSaveStatus()))
        {
            QTabWidget *temp = static_cast<QTabWidget *>(ui->tabWidget->currentWidget());

            this->m_viewWidget_a.erase(m_viewWidget_a.begin() + current_index);
            ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
            if (temp)
                delete temp;
        }
        else
        {
            QTabWidget *temp = static_cast<QTabWidget *>(ui->tabWidget->currentWidget());

            this->m_viewWidget_a.erase(m_viewWidget_a.begin() + current_index);
            ui->tabWidget->removeTab(current_index);
            if (temp)
                delete temp;
        }
    }
}

void MainWindow::close_editable_tab(int index)
{

    if (ui->tabWidget->count() != 1)
    {
//        if (this->m_viewWidget_a[index]->getEditableStatus() && !(this->m_viewWidget_a[index]->getSaveStatus()))
//        {
//            QTabWidget *temp = static_cast<QTabWidget *>(ui->tabWidget->widget(index));

//            this->m_viewWidget_a.erase(m_viewWidget_a.begin() + index);
//            ui->tabWidget->removeTab(index);
//            if (temp)
//                delete temp;
//        }
//        else
        //{
            QTabWidget *temp = static_cast<QTabWidget *>(ui->tabWidget->widget(index));

            this->m_viewWidget_a.erase(m_viewWidget_a.begin() + index);
            ui->tabWidget->removeTab(index);
            if (temp)
                delete temp;
        //}
    }
}

void MainWindow::create_edit_tab(std::string filename_original, std::string file_image)
{
    bool isEditablePage = false;
    for (int i = 0; i < m_viewWidget_a.size(); i++)
    {
        if (m_viewWidget_a[i]->getEditableStatus())
        {
            isEditablePage = true;
            QMessageBox::information(this, "Appologize", "You can open only one editable page yet.\nYou can close previous one and try again");
            return;
        }
    }
    auto createReceiptView = new CreateReceiptVeiw{this, filename_original, file_image};
    // auto createReceiptView = new CreateReceiptVeiw{this, Paths::created_folder, filename_original, file_image};

    connect(createReceiptView, &CreateReceiptVeiw::tabWasSaved, this, &MainWindow::close_current_tab);
    connect(this, &MainWindow::attemptToCloseEditableTab, createReceiptView, &CreateReceiptVeiw::confirm_closing_tab);

    connect(createReceiptView, &CreateReceiptVeiw::tabEditableWasSaved, this, &MainWindow::close_editable_tab);
    connect(createReceiptView, &CreateReceiptVeiw::closingEditabletabConfirmed, this, &MainWindow::close_editable_tab);

    connect(this, &MainWindow::attemptToSaveEditableTab, createReceiptView, &CreateReceiptVeiw::saving);
    ui->tabWidget->addTab(createReceiptView, tr("Editing receipt"));

    this->m_viewWidget_a.push_back(createReceiptView);
    this->m_viewWidget_a[m_viewWidget_a.size() - 1]->setSavedStatus(false);
    this->m_viewWidget_a[m_viewWidget_a.size() - 1]->setEditableStatus(true);

    ui->tabWidget->setCurrentWidget(createReceiptView);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    bool isSomePageUnsaved = false;
    for (int i = 0; i < ui->tabWidget->count(); i++)
    {
        if (!m_viewWidget_a[i]->getSaveStatus())
        {
            isSomePageUnsaved = true;
            break;
        }
    }
    if (isSomePageUnsaved)
    {

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setInformativeText("You're trying to close application with unsaved page. It can cause data loss.\n Do you want to continue?");
        QAbstractButton *noBtn = msgBox.addButton("No", QMessageBox::RejectRole);
        QAbstractButton *yesBtn = msgBox.addButton("Yes", QMessageBox::AcceptRole);

        int ret = msgBox.exec();
          if (msgBox.clickedButton() == yesBtn) {
              if (m_db)
                  m_db->save();
              //TODO: check if file(tab) is modifed
              event->accept();
          } else {

              event->ignore();
          }

    }
    else
    {
        if (m_db)
            m_db->save();
        // TODO: check if file(tab) is modifed
        event->accept();
    }
}

ViewWidget *MainWindow::createWelcomePage()
{
    auto _widT = new ViewWidget(nullptr);

    auto _layout1 = new QVBoxLayout();
    QLabel *_label = new QLabel(tr("WELCOME PAGE"));
    _layout1->addWidget(_label);
    _label->setAlignment(Qt::AlignCenter);
    _label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // TODO: Change
    QPixmap *img = new QPixmap(Settings::welcome_page);

    img->scaled(QSize(_label->height(), _label->width()), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    _label->setPixmap(*img);
    _label->setScaledContents(true);

    if (img)
        delete img;

    _widT->setLayout(_layout1);
    return _widT;
}
