#include "../headers/mainwindow.h"
#include "ui_mainwindow.h"

#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qevent.h>

#include "../headers/pwdmaskdelegate.h"
#include "../headers/utils.h"
#include "../headers/pwdgeneratordialog.h"
#include "../headers/masterpwdpromptdialog.h"
#include "../headers/plaintextdelegate.h"
#include "../headers/customtreeview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(m_currentFileName + " — BiPass");
    this->setWindowIcon(QIcon(":/resources/icons/bipass-logo.svg"));
    treeView = ui->treeView;

    // Dodaje menu kontekstowe zawierające wszystkie akcje dostępne z poziomu paska narzędzi
    treeView->addActions(ui->menubar->actions());

    // Przygotuj model przechowujący pary serwis-hasło
    pwdModel = new QStandardItemModel(this);
    pwdModel->setColumnCount(2);
    pwdModel->setHeaderData(0, Qt::Horizontal, tr("Serwis"));
    pwdModel->setHeaderData(1, Qt::Horizontal, tr("Hasło"));
    treeView->setModel(pwdModel);

    // Przygotuj delegat maskujący hasła
    pwdMaskDelegate = new PwdMaskDelegate(QChar(0x25cf), true, treeView); // Kropki środkowe
    treeView->setItemDelegateForColumn(0, new PlainTextDelegate());
    treeView->setItemDelegateForColumn(1, pwdMaskDelegate);
    treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    // Połączenia akcji ze slotami
    connect(ui->actionClose, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));                   // "Zakończ"
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));                         // "O programie..."
    connect(ui->actionImportPwd, SIGNAL(triggered()), this, SLOT(importPwd()));                     // "Importuj hasła..."
    connect(ui->actionExportPwd, SIGNAL(triggered()), this, SLOT(exportPwdAs()));                   // "Eksportuj hasła jako..."
    connect(ui->actionSavePwd, SIGNAL(triggered()), this, SLOT(savePwd()));                         // "Zapisz"
    connect(ui->actionClearPwd, SIGNAL(triggered()), this, SLOT(clearPwd()));                       // "Wyczyść listę haseł"
    connect(ui->actionTogglePwdVisibility, SIGNAL(triggered()), this, SLOT(togglePwdVisibility())); // "Pokaż/Ukryj hasła"
    connect(ui->actionGenerateRandomPwd, SIGNAL(triggered()), this, SLOT(generateRandomPwd()));     // "Generuj nowe hasło"
    connect(ui->actionLoginPwdPair_add, SIGNAL(triggered()), this, SLOT(loginPwdPair_add()));       // "Dodaj parę login-hasło"
    connect(ui->actionLoginPwdPair_remove, SIGNAL(triggered()), this, SLOT(loginPwdPair_remove())); // "Usuń parę login-hasło"
    connect(ui->actionLoginPwdPair_edit, SIGNAL(triggered()), this, SLOT(loginPwdPair_edit()));     // "Edytuj parę login-hasło"
    connect(ui->actionMovePwd_rowUp, SIGNAL(triggered()), this, SLOT(movePwd()));                   // "Przenieś hasło... > Wiersz wyżej"
    connect(ui->actionMovePwd_rowDown, SIGNAL(triggered()), this, SLOT(movePwd()));                 // "Przenieś hasło... > Wiersz niżej"
    connect(pwdModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(documentModified()));         // Zasygnalizuj zmianę zawartości

    // Wyłącz lub włącz akcje zależne od zaznaczenia
    connect(
        treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(toggleSelectionDependentActions(QItemSelection,QItemSelection))
    );


    srand(time(0));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* Pokaż okno dialogowe wyświetlające informacje o programie */
void MainWindow::showAbout() {
    QMessageBox aboutDialog(this);
    aboutDialog.setWindowTitle(tr("O programie"));
    aboutDialog.setTextFormat(Qt::RichText);
    aboutDialog.setText(
        tr(
            "<strong>BiPass &copy; Mateusz Nosel, 2023</strong>"
            "<br/>"
            "<br/>Ikony stworzone przez Font Awesome:"
            "<br/><a href='https://fontawesome.com/'>https://fontawesome.com</a>"
            "<br/>&copy; 2023 Fonticons, Inc. Na licencji: CC BY 4.0:"
            "<br/><a href='https://creativecommons.org/licenses/by/4.0/legalcode'>https://creativecommons.org/licenses/by/4.0/legalcode</a>"
        )
    );
    aboutDialog.addButton(QMessageBox::Ok);
    aboutDialog.exec();
}

/* Importuj hasła z pliku za pośrednictwem okna dialogowego */
void MainWindow::importPwd() {
    if(isWindowModified()) {
        // Zapytaj, czy zapisać zmiany przed importem (jeśli istnieją niezapisane zmiany)
        QMessageBox unsavedChangesDialog;
        unsavedChangesDialog.setWindowTitle(tr("Niezapisane zmiany"));
        unsavedChangesDialog.setText(tr("Masz niezapisane zmiany. Czy chcesz zapisać zmiany w pliku ") + m_currentFileName + "?");

        QPushButton *save = unsavedChangesDialog.addButton(tr("Zapisz"), QMessageBox::AcceptRole);
        unsavedChangesDialog.addButton(tr("Odrzuć"), QMessageBox::RejectRole);
        unsavedChangesDialog.setDefaultButton(save);
        unsavedChangesDialog.setIcon(QMessageBox::Warning);
        unsavedChangesDialog.exec();

        if(unsavedChangesDialog.clickedButton() == save) {
            savePwd();
        }
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Wybierz plik z hasłami"), NULL, tr("Pliki CSV") + " (*.csv)");
    if(fileName.isEmpty() || !askForMasterPwd()) return; // Przerwanie importowania, jeśli użytkownik anulował lub zamknął dialog wyboru pliku lub dialog uwierzytelniania

    // Utwórz nowy obiekt pliku na podstawie wybranego pliku
    QFile file(QFileInfo(fileName).absoluteFilePath());

    // Spróbuj otworzyć plik w trybie odczytu
    if(!file.open(QIODevice::ReadOnly)) {

        // Jeśli błąd, wyświetl komunikat i powróć to aplikacji
        QMessageBox::critical(this, tr("Błąd"), tr("Nie udało się wczytać pliku"));
        return;
    }

    // Jeśli wszystko OK, wczytaj hasła z pliku
    clearPwd(false); // Ale najpierw wyczyść listę
    QTextStream in(&file);
    int lineNr = 1;
    while(!in.atEnd()) {

        // Odczytaj pary serwis-hasło w formacie CSV [serwis,hasło]
        QStringList entry = in.readLine().split(",");

        // Sprawdź, czy uzyskano poprawny podział (format jest prawidłowy)
        if(entry.length() != 2) {

            // Jeśli błąd, wyświetl komunikat i powróć do aplikacji
            QMessageBox::critical(this, tr("Błąd"), tr("Nie udało się odczytać pliku. Dane mają nieprawidłowy format!"));
            return;
        }

        // Jeśli OK, dodaj bieżącą parę do modelu (treeView) [pomiń 1 wiersz, czyli nagłówki]
        if(lineNr > 1) {
            QString decryptedPwd = Utils::decryptPwd(entry[1], m_encryptionKey);

            // Utils::decryptPwd zwraca pusty łańcuch, jeśli klucz szyfrujący jest nieprawidłowy
            if(decryptedPwd.isEmpty()) {
                QMessageBox::critical(this, tr("Odmowa dostępu"), tr("Wprowadzone hasło główne jest nieprawidłowe!"));
                return;
            }
            pwdModel->insertRow(0);
            pwdModel->setData(pwdModel->index(0, 0), entry[0]);
            pwdModel->setData(pwdModel->index(0, 1), decryptedPwd);
        }
        lineNr++;
    }
    file.close();
    m_currentFileName = fileName;
    ui->statusbar->showMessage("Otwarty plik: " + m_currentFileName);
}

// Bazowa metoda eksportująca hasła do pliku
// Nie może bezpośrednio być slotem, ponieważ posiada parametr, fileName
void MainWindow::exportPwd(QString fileName) {
    // Jeśli wprowadzono nazwę
    if(fileName != "") {

        // Utwórz nowy obiekt pliku o podanej nazwie
        QFile file(QFileInfo(fileName).absoluteFilePath());

        // Spróbuj otworzyć plik w trybie zapisu
        if(!file.open(QIODevice::WriteOnly)) {

            // Jeśli błąd
            QMessageBox::critical(this, tr("Błąd"), tr("Nie udało się zapisać pliku"));
            return;
        }

        // Jeśli wszystko OK, zapisz hasła w pliku
        QTextStream out(&file);
        int rowCount = pwdModel->rowCount();
        out << "serwis,hasło\n";
        for(int row = 0; row < rowCount; row++) {
            QModelIndex siteIndex = pwdModel->index(row, 0);
            QModelIndex pwdIndex = pwdModel->index(row, 1);
            out << pwdModel->data(siteIndex).toString() << "," << Utils::encryptPwd(pwdModel->data(pwdIndex).toString(), m_encryptionKey).value("pwd") << "\n";
        }
        file.close();
        m_currentFileName = fileName;

        // Zaktualizuj stan pliku (zmiany zapisane)
        setWindowModified(false);
        setWindowTitle(m_currentFileName + " — BiPass");
        ui->statusbar->showMessage("Otwarty plik: " + m_currentFileName);
    }
}

// Eksportuj hasła do pliku za pośrednictwem okna dialogowego
void MainWindow::exportPwdAs() {
    if(!checkIfFirstLaunch()) return;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Zapisz hasła do pliku"), "pwd.csv", tr("Pliki CSV") + " (*.csv)");
    exportPwd(fileName);
}

// Nadpisz bieżący plik
void MainWindow::savePwd() {
    if(!checkIfFirstLaunch()) return;
    // Eksportuj jako... jeśli nic nie zaimportowano lub oryginalny plik został usunięty/przeniesiony
    !m_currentFileName.isNull() && QFile::exists(QFileInfo(m_currentFileName).absoluteFilePath()) ? exportPwd(m_currentFileName) : exportPwdAs();
}

// Wyczyść wpisy (reset modelu)
void MainWindow::clearPwd(bool askForConfirmation) {
    if(askForConfirmation) {
        QMessageBox confirmClearDialog;
        confirmClearDialog.setWindowTitle(tr("Niebezpieczna operacja"));
        confirmClearDialog.setText(tr("Czy na pewno chcesz wyczyścić listę haseł?"));

        QPushButton *no = confirmClearDialog.addButton(tr("Nie"), QMessageBox::RejectRole);
        confirmClearDialog.addButton(tr("Tak"), QMessageBox::AcceptRole);
        confirmClearDialog.setDefaultButton(no);
        confirmClearDialog.setIcon(QMessageBox::Warning);
        confirmClearDialog.exec();

        if(confirmClearDialog.clickedButton() == no) {
            return;
        }
    }
    pwdModel->removeRows(0, pwdModel->rowCount());
}

// Pokaż/Ukryj hasła nakładając maskę
void MainWindow::togglePwdVisibility() {

    // Sprawdź obecnego delegata i odpowiednio przełącz oraz zaktualizuj GUI
    if(pwdMaskDelegate->isPasswordHidden()) {
        pwdMaskDelegate->setPasswordHidden(false);
        ui->actionTogglePwdVisibility->setText(tr("Ukryj hasła"));
        ui->actionTogglePwdVisibility->setToolTip(tr("Ukryj hasła"));
        ui->actionTogglePwdVisibility->setIcon(QIcon(":/resources/icons/eye-slash-solid.svg"));
    }
    else {
        pwdMaskDelegate->setPasswordHidden(true);
        ui->actionTogglePwdVisibility->setText(tr("Pokaż hasła"));
        ui->actionTogglePwdVisibility->setToolTip(tr("Pokaż hasła"));
        ui->actionTogglePwdVisibility->setIcon(QIcon(":/resources/icons/eye-solid.svg"));
    }
    treeView->viewport()->update();
}

// Generuj nowe hasło i wyświetl je w nowym oknie
void MainWindow::generateRandomPwd() {
    PwdGeneratorDialog *pgd = new PwdGeneratorDialog();
    pgd->show();
}

bool MainWindow::checkIfFirstLaunch() {
    QString masterDir = QCoreApplication::applicationDirPath() + "/master.txt";
    if(!QFile::exists(masterDir)) {
        MasterPwdPromptDialog *mppd = new MasterPwdPromptDialog(this, MasterPwdPromptDialog::Mode::SetNew);
        int result = mppd->exec();
        if(result == QDialog::Accepted) {
            m_encryptionKey = mppd->getPwdAsEncryptionKey();
        }
        else {
            return false;
        }

        QFile master(masterDir);
        if(!master.open(QIODevice::WriteOnly)) {
            // Jeśli błąd
            QMessageBox::critical(this, tr("Błąd"), tr("Coś poszło nie tak!"));
            return false;
        }
        QTextStream out(&master);
        out << "Czego tu szukasz? :)";
        master.close();
    }
    return true;
}

bool MainWindow::askForMasterPwd() {
    MasterPwdPromptDialog *mppd = new MasterPwdPromptDialog();
    int result = mppd->exec();
    if(result == QDialog::Accepted) {
        m_encryptionKey = mppd->getPwdAsEncryptionKey();
        return true;
    }
    return false;
}

void MainWindow::loginPwdPair_add() {
    // Wstaw wiersz na początek listy i ustaw zaznaczenie na pierwszą komórkę
    pwdModel->insertRow(0);
    treeView->selectionModel()->select(pwdModel->index(0, 0), QItemSelectionModel::ClearAndSelect);
    toggleSelectionDependentActions(treeView->selectionModel()->selection(), treeView->selectionModel()->selection());
}

void MainWindow::loginPwdPair_remove() {
    QModelIndexList selectedItemIndexList = treeView->selectionModel()->selectedIndexes();
    if(selectedItemIndexList.isEmpty()) {
        return; // Nie usuwaj jeśli nic nie zaznaczono
    }
    // Pobierz indeks zaznaczonego elementu
    int selectedItemRow = selectedItemIndexList.first().row();

    // Usuń cały wiersz (bez sensu byłoby usunięcie pojedynczej komórki)
    pwdModel->removeRow(selectedItemRow);

    // Zaktualizuj akcje zależne od zaznaczenia
    toggleSelectionDependentActions(treeView->selectionModel()->selection(), treeView->selectionModel()->selection());
}

void MainWindow::loginPwdPair_edit() {
    QModelIndexList selectedItemIndexList = treeView->selectionModel()->selectedIndexes();
    // Najpierw sprawdź, czy edytor jest już otwarty (otwórz nowy tylko jeśli nie ma już otwartego)
    if(!treeView->isPersistentEditorOpen(selectedItemIndexList.first())) {
        if(selectedItemIndexList.isEmpty()) {
            return; // Nie edytuj jeśli nic nie zaznaczono
        }
        // Pobierz indeks zaznaczonego elementu
        QModelIndex selectedItemIndex = selectedItemIndexList.first();
        treeView->edit(selectedItemIndex);
    }
}

void MainWindow::movePwd() {
    // Przesunięcie: -1 = wiersz wyżej; (+)1 = wiersz niżej
    int offset;
    if(sender() == ui->actionMovePwd_rowUp) {
        offset = -1;
    }
    else if(sender() == ui->actionMovePwd_rowDown) {
        offset = 1;
    }

    // W razie gdyby slot został wywołany przez niepożądanego nadawcę...
    else {
        return;
    }

    // Pobierz indeks i nr wiersza zaznaczonego elementu, oblicz nową pozycję
    QModelIndexList selectedItemIndexList = treeView->selectionModel()->selectedIndexes();
    if(selectedItemIndexList.isEmpty()) {
        return; // Nie przesuwaj jeśli nic nie zaznaczono
    }
    QModelIndex selectedItemIndex = selectedItemIndexList.first();
    QStandardItem *selectedItem = pwdModel->itemFromIndex(selectedItemIndex);
    int selectedRow = selectedItemIndex.row();
    if((selectedRow == 0 && offset == -1) || (selectedRow == pwdModel->rowCount() - 1 && offset == 1)) {
        return; // Nie przesuwaj jeśli element ma być przeniesiony przed pierwszy lub za ostatni wiersz
    }
    int newRow = selectedRow + offset;

    // Wstaw wiersz nowe miejsce
    QList<QStandardItem*> item = pwdModel->takeRow(selectedRow);
    pwdModel->insertRow(newRow, item);

    // Zaktualizuj zaznaczenie
    QModelIndex newIndex = pwdModel->index(newRow, selectedItem->column());
    treeView->selectionModel()->select(newIndex, QItemSelectionModel::ClearAndSelect);
}

void MainWindow::toggleSelectionDependentActions(QItemSelection selected, QItemSelection) {
    if(!selected.indexes().isEmpty()) {
        ui->actionMovePwd_rowUp->setEnabled(selected.indexes().first().row() != 0);
        ui->actionMovePwd_rowDown->setEnabled(selected.indexes().last().row() != pwdModel->rowCount() - 1);
    }
    else {
        ui->actionMovePwd_rowUp->setEnabled(false);
        ui->actionMovePwd_rowDown->setEnabled(false);
    }
    ui->actionLoginPwdPair_edit->setEnabled(!selected.indexes().isEmpty());
    ui->actionLoginPwdPair_remove->setEnabled(!selected.indexes().isEmpty());
}

void MainWindow::documentModified() {
    setWindowTitle("[*]" + m_currentFileName + " — BiPass");
    setWindowModified(true);
}

void MainWindow::closeEvent(QCloseEvent *e) {
    if(isWindowModified()) {
        QMessageBox confirmCloseDialog;
        confirmCloseDialog.setWindowTitle(tr("Niezapisane zmiany"));
        confirmCloseDialog.setText(tr("Masz niezapisane zmiany. Czy na pewno chcesz zakończyć program?"));

        QPushButton *no = confirmCloseDialog.addButton(tr("Nie"), QMessageBox::RejectRole);
        confirmCloseDialog.addButton(tr("Tak"), QMessageBox::AcceptRole);
        confirmCloseDialog.setDefaultButton(no);
        confirmCloseDialog.setIcon(QMessageBox::Warning);
        confirmCloseDialog.exec();

        if(confirmCloseDialog.clickedButton() == no) {
            e->ignore();
        }
        else {
            e->accept();
        }
    }
    else {
        e->accept();
    }
}
