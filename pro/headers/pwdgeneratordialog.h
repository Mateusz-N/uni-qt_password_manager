#ifndef PWDGENERATORDIALOG_H
#define PWDGENERATORDIALOG_H

#include <QDialog>

namespace Ui {
class PwdGeneratorDialog;
}

class PwdGeneratorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PwdGeneratorDialog(QWidget *parent = nullptr);
    ~PwdGeneratorDialog();

private slots:
    void copyPwdToClipboard();
    void generateRandomPwd();
    void togglePwdVisibility();
    void updatePwd();

private:
    Ui::PwdGeneratorDialog *ui;
    QString m_newPwd;
    bool m_passwordIsHidden = true;
};

#endif // PWDGENERATORDIALOG_H
