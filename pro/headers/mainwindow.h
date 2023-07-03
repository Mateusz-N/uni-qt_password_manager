#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstandarditemmodel.h>
#include <qtreeview.h>

#include "./pwdmaskdelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *pwdModel;
    QString m_currentFileName = tr("Bez tytułu");
    quint64 m_encryptionKey;
    PwdMaskDelegate *pwdMaskDelegate;
    QTreeView *treeView;
    void exportPwd(QString);
    bool userLoggedIn();
    bool checkIfFirstLaunch();
    bool askForMasterPwd();

private slots:
    void showAbout();
    void importPwd();
    void exportPwdAs();
    void savePwd();
    void clearPwd(bool askForConfirmation = true);
    void togglePwdVisibility();
    void generateRandomPwd();
    void loginPwdPair_add();
    void loginPwdPair_remove();
    void loginPwdPair_edit();
    void movePwd();
    void toggleSelectionDependentActions(QItemSelection selected, QItemSelection);
    void documentModified();
    void closeEvent(QCloseEvent *e);
};
#endif // MAINWINDOW_H
