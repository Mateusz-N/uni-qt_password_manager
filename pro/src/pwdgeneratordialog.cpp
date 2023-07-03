#include "../headers/pwdgeneratordialog.h"
#include "ui_pwdgeneratordialog.h"

#include <qclipboard.h>
#include "../headers/utils.h"

PwdGeneratorDialog::PwdGeneratorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PwdGeneratorDialog)
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/resources/icons/bipass-logo.svg"));
    generateRandomPwd();
    updatePwd();

    connect(ui->btn_copyToClipboard, SIGNAL(clicked()), this, SLOT(copyPwdToClipboard()));
    connect(ui->btn_regeneratePwd, SIGNAL(clicked()), this, SLOT(generateRandomPwd()));
    connect(ui->btn_togglePwdVisibility, SIGNAL(clicked()), this, SLOT(togglePwdVisibility()));
}

PwdGeneratorDialog::~PwdGeneratorDialog()
{
    delete ui;
}

void PwdGeneratorDialog::copyPwdToClipboard() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_newPwd, QClipboard::Clipboard);
}

void PwdGeneratorDialog::generateRandomPwd() {
    m_newPwd = Utils::generateRandomPwd();
    updatePwd();
}

void PwdGeneratorDialog::togglePwdVisibility() {
    m_passwordIsHidden = !m_passwordIsHidden;
    updatePwd();
}

void PwdGeneratorDialog::updatePwd() {
    ui->label_newPwd->setText(m_passwordIsHidden ? QString(m_newPwd.length(), QChar(0x25cf)) : m_newPwd);
    ui->btn_togglePwdVisibility->setIcon(QIcon(m_passwordIsHidden ? ":/resources/icons/eye-solid.svg" : ":/resources/icons/eye-slash-solid.svg"));
    ui->btn_togglePwdVisibility->setToolTip(m_passwordIsHidden ? tr("Pokaż hasło") : tr("Ukryj hasło"));
}
