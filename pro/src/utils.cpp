#include "../headers/utils.h"

#include <qstring.h>
#include <qmap.h>

#include "../headers/simplecrypt.h"

Utils::Utils()
{

}

QString Utils::generateRandomPwd() {
    QString newPwd;

    // Losowy znak nie-alfanumeryczny
    // W tablicy ASCII 32 takie znaki są rozbite na 4 zakresy
    int randomNonAlphanumericChar_asciiPos = rand() % 32 + '!';
    if(randomNonAlphanumericChar_asciiPos > '/') {
        randomNonAlphanumericChar_asciiPos += 10;
        if(randomNonAlphanumericChar_asciiPos > '@') {
            randomNonAlphanumericChar_asciiPos += 26;
            if(randomNonAlphanumericChar_asciiPos > '\'') {
                randomNonAlphanumericChar_asciiPos += 26;
            }
        }
    }
    newPwd = newPwd
            + QChar(rand() % 26 + 'a')                      // Losowa mała litera
            + QChar(rand() % 26 + 'A')                      // Losowa wielka litera
            + QChar(rand() % 10 + '0')                      // Losowa cyfra
            + QChar(randomNonAlphanumericChar_asciiPos);    // Losowy znak nie-alfanumeryczny
    // Losowe dopełnienie do 16 znaków
    for(int charNo = 0; charNo < 12; charNo++) {
        newPwd += QChar(rand() % 94 + '!');
    }
    // Dodatkowe wymieszanie znaków
    std::random_shuffle(newPwd.begin(), newPwd.end());
    return newPwd;
}

QString Utils::decryptPwd(QString encryptedPwd, quint64 key) {
    SimpleCrypt crypto(key);
    return crypto.decryptToString(encryptedPwd);
}

QMap<QString, QString> Utils::encryptPwd(QString pwd, quint64 key) {
    SimpleCrypt crypto(key);
    QMap<QString, QString> map;
    map["pwd"] = crypto.encryptToString(pwd);
    map["key"] = QString::number(key);
    return map;
}
