#ifndef __CRYPTSERVICE_H__
#define __CRYPTSERVICE_H__

#include <stdio.h>

#include <QString>
#include <QByteArray>

// Класс с функциями-помощниками, используемыми при шифрации/дешифровки


class CryptService
{

public:

  CryptService();
  virtual ~CryptService();

  static void convertByteArrayToVector(const QByteArray &qba, vector<unsigned char> &vec);
  static void convertVectorToByteArray(const vector<unsigned char> &vec, QByteArray &qba);

  static QString encryptString(QByteArray key, QString line);
  static QString decryptString(QByteArray key, QString line);

  static QByteArray encryptByteArray(QByteArray key, QByteArray data);
  static QByteArray decryptByteArray(QByteArray key, QByteArray data);

  static QByteArray encryptStringToByteArray(QByteArray key, QString line);
  static QString decryptStringFromByteArray(QByteArray key, QByteArray data);

  static void encryptFile(QByteArray key, QString fileName);
  static void decryptFile(QByteArray key, QString fileName);
  static void encDecFileSmart(QByteArray key, QString fileName, int mode);

};

#endif // __CRYPTSERVICE_H__
