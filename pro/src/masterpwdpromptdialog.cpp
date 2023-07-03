#include "../headers/masterpwdpromptdialog.h"
#include "ui_masterpwdpromptdialog.h"

enum class Mode{
    SetNew,
    Authenticate
};

MasterPwdPromptDialog::MasterPwdPromptDialog(QWidget *parent, Mode mode) :
    QDialog(parent),
    ui(new Ui::MasterPwdPromptDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Anuluj");
    this->setWindowIcon(QIcon(":/resources/icons/bipass-logo.svg"));

    m_mode = mode;
    if(m_mode == Mode::SetNew) {
        ui->label_masterPwdLabel->setText("Nowe hasło główne");
        this->setWindowTitle("Ustaw hasło główne");
    }

    connect(ui->btn_togglePwdVisibility, SIGNAL(clicked()), this, SLOT(togglePwdVisibility()));
    connect(ui->lineEdit_masterPwd, SIGNAL(textChanged(QString)), this, SLOT(setMasterPwd(QString)));
    connect(ui->lineEdit_masterPwd, SIGNAL(textChanged(QString)), this, SLOT(disableBtnsIfPwdEmpty(QString)));
}

MasterPwdPromptDialog::~MasterPwdPromptDialog()
{
    delete ui;
}

void MasterPwdPromptDialog::togglePwdVisibility() {
    m_passwordIsHidden = !m_passwordIsHidden;
    updatePwd();
}

void MasterPwdPromptDialog::updatePwd() {
    ui->lineEdit_masterPwd->setEchoMode(m_passwordIsHidden ? QLineEdit::Password : QLineEdit::Normal);
    ui->btn_togglePwdVisibility->setIcon(QIcon(m_passwordIsHidden ? ":/resources/icons/eye-solid.svg" : ":/resources/icons/eye-slash-solid.svg"));
    ui->btn_togglePwdVisibility->setToolTip(m_passwordIsHidden ? tr("Pokaż hasło") : tr("Ukryj hasło"));
}

void MasterPwdPromptDialog::setMasterPwd(QString pwd) {
    m_newPwd = pwd;
}

void MasterPwdPromptDialog::disableBtnsIfPwdEmpty(QString pwd) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(pwd.length() > 0);
}

quint64 MasterPwdPromptDialog::getPwdAsEncryptionKey() {
    return ui->lineEdit_masterPwd->text().toULongLong(nullptr, 64);
}
