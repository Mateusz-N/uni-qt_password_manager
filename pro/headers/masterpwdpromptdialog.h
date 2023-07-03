#ifndef MASTERPWDPROMPTDIALOG_H
#define MASTERPWDPROMPTDIALOG_H

#include <QDialog>

namespace Ui {
class MasterPwdPromptDialog;
}

class MasterPwdPromptDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode{Authenticate, SetNew};
    explicit MasterPwdPromptDialog(QWidget *parent = nullptr, Mode = Mode::Authenticate);
    ~MasterPwdPromptDialog();

    quint64 getPwdAsEncryptionKey();

private slots:
    void togglePwdVisibility();
    void updatePwd();
    void setMasterPwd(QString);
    void disableBtnsIfPwdEmpty(QString pwd);

private:
    Ui::MasterPwdPromptDialog *ui;
    QString m_newPwd;
    Mode m_mode;
    bool m_passwordIsHidden = true;
};

#endif // MASTERPWDPROMPTDIALOG_H
