#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
    class QTimer;
}
QT_END_NAMESPACE

namespace CookBook
{
    class SideBar;
    class ProxyReceiptsDB;
    class SearchDialog;
    class CalculateDialog;
    
    class CreateViewProvider;
    class ReceiptViewProvider;
    class UploadDilaog;
    class DownloadDialog;
    class ViewWidget;

}
class CalculateDialog;
namespace CookBook
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        void closeEvent(QCloseEvent *event) override;
        static ViewWidget* createWelcomePage();
    private slots:
        void on_actionDownload_recipe_triggered();

        void on_actionFind_triggered();

        void on_actionOpen_triggered();

        void on_actionNew_Tab_triggered();

        void on_actionSave_triggered();

        void on_actionSave_as_triggered();

        void on_actionClose_Tab_triggered();

        void on_actionExit_triggered();

        void open_view(const QString &id);

        void close_current_tab();

        void close_editable_tab(int index);

        void create_edit_tab(std::string filename_original,std::string file_image);

        void on_actionCreate_new_receipt_triggered();
    signals:
        void attemptToCloseEditableTab(int index);
        void attemptToSaveEditableTab(bool isSaveAs);
    private:
        Ui::MainWindow *ui;
        QVector<int> m_toSave_a;
        SideBar *m_sideBar{nullptr};
        ProxyReceiptsDB *m_db{nullptr};
        QTimer *timer{nullptr};
        QVector<ViewWidget *> m_viewWidget_a;
    };
}
#endif // MAINWINDOW_H
