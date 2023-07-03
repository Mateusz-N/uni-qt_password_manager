#ifndef UTILS_H
#define UTILS_H

#include <qstring.h>

class Utils
{
private:
    Utils();
public:
    static QString generateRandomPwd();
    static QString decryptPwd(QString encryptedPwd, quint64 key);
    static QMap<QString, QString> encryptPwd(QString pwd, quint64 key);
};

#endif // UTILS_H
