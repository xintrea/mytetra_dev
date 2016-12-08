#include "main.h"
#include "CryptService.h"
#include "RC5Simple.h"
#include "Password.h"

#include <QFile>


CryptService::CryptService()
{

}


CryptService::~CryptService()
{

}


void CryptService::convertByteArrayToVector(const QByteArray &qba, vector<unsigned char> &vec)
{
  unsigned int size=qba.size();
  vec.resize(size, 0);
  memcpy(&vec[0], qba.constData(), size*sizeof(unsigned char));
}


void CryptService::convertVectorToByteArray(const vector<unsigned char> &vec, QByteArray &qba)
{
  unsigned int size=vec.size();
  qba.clear();
  qba.append( (const char *)&vec[0], size*sizeof(unsigned char) );

  /*
  printf("\n");
  qDebug() << "Vector: ";
  for(unsigned int i=0; i<size; i++)
    printf("%02hhx", vec[i]);
  printf("\n");

  printf("\n");
  qDebug() << "QByteArray: ";
  for(unsigned int i=0; i<size; i++)
    printf("%02hhx", qba.at(i));
  printf("\n");
  */
}


// Шифрация строки
// Результирующая зашифрованная строка - это набор шифрованных байт,
// представленный в кодировке Base64
QString CryptService::encryptString(QByteArray key, QString line)
{
 if(line=="") return QString();

 // qDebug() << "Encrypt source" << line;

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 vector<unsigned char> vectorLineIn;
 convertByteArrayToVector(line.toUtf8(), vectorLineIn);

 vector<unsigned char> vectorLineOut;

 // Шифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 rc5.RC5_Encrypt(vectorLineIn, vectorLineOut);


 QByteArray result;
 convertVectorToByteArray(vectorLineOut, result);

 QString resultLine( result.toBase64().data() );

 // qDebug() << "Encrypt result" << resultLine;

 return resultLine;
}


// Расшифровка строки
// Принимаемая зашифрованная строка - это набор шифрованных байт,
// представленный в кодировке Base64
QString CryptService::decryptString(QByteArray key, QString line)
{
 if(line=="") return QString();

 /*
 qDebug() << "Decrypt source" << line;
 qDebug() << "Decrypt source HEX" << QByteArray::fromBase64( line.toAscii() ).toHex();
 qDebug() << "Decrypt key" << key.toHex();
 */

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 /*
 printf("Decrypt vector key ");
 for(int i=0; i<vectorKey.size(); i++)
  printf("%.2X", vectorKey[i]);
 printf("\n");
 */

 vector<unsigned char> vectorLineIn;

 // Заменен вызов line.toAscii на line.toLatin1 чтобы шла компиляция в Qt 5.2.
 // Эта замена допустима, так как в Base64 используются только символы латиницы и ограниченный набор прочих символов
 convertByteArrayToVector(QByteArray::fromBase64( line.toLatin1() ), vectorLineIn);

 /*
 printf("Decrypt vector source HEX ");
 for(int i=0; i<vectorLineIn.size(); i++)
  printf("%.2X", vectorLineIn[i]);
 printf("\n");
 */

 vector<unsigned char> vectorLineOut;

 // Дешифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 rc5.RC5_Decrypt(vectorLineIn, vectorLineOut);


 QByteArray result;
 convertVectorToByteArray(vectorLineOut, result);

 QString resultLine=QString::fromUtf8( result.data() );

 // qDebug() << "Decrypt result" << resultLine;

 return resultLine;
}


QByteArray CryptService::encryptByteArray(QByteArray key, QByteArray data)
{
  if(data.size()==0)
    return QByteArray();

  vector<unsigned char> vectorKey;
  convertByteArrayToVector(key, vectorKey);

  vector<unsigned char> vectorDataIn;
  convertByteArrayToVector(data, vectorDataIn);

  vector<unsigned char> vectorDataOut;

  // Шифрация
  RC5Simple rc5;
  rc5.RC5_SetKey(vectorKey);
  rc5.RC5_Encrypt(vectorDataIn, vectorDataOut);

  QByteArray result;
  convertVectorToByteArray(vectorDataOut, result);

  return result;
}


QByteArray CryptService::decryptByteArray(QByteArray key, QByteArray data)
{
  if(data.size()==0)
    return QByteArray();

  vector<unsigned char> vectorKey;
  convertByteArrayToVector(key, vectorKey);

  vector<unsigned char> vectorDataIn;
  convertByteArrayToVector(data, vectorDataIn);

  vector<unsigned char> vectorDataOut;

  // Дешифрация
  RC5Simple rc5;
  rc5.RC5_SetKey(vectorKey);
  rc5.RC5_Decrypt(vectorDataIn, vectorDataOut);

  QByteArray result;
  convertVectorToByteArray(vectorDataOut, result);

  return result;
}


QByteArray CryptService::encryptStringToByteArray(QByteArray key, QString line)
{
 if(line=="")
   return QByteArray();

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 vector<unsigned char> vectorLineIn;
 convertByteArrayToVector(line.toUtf8(), vectorLineIn);

 vector<unsigned char> vectorLineOut;

 // Шифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 rc5.RC5_Encrypt(vectorLineIn, vectorLineOut);

 QByteArray result;
 convertVectorToByteArray(vectorLineOut, result);

 return result;
}


QString CryptService::decryptStringFromByteArray(QByteArray key, QByteArray data)
{
 if(data.length()==0)
   return QString();

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 vector<unsigned char> vectorDataIn;
 convertByteArrayToVector(data, vectorDataIn);

 vector<unsigned char> vectorDataOut;

 // Дешифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 rc5.RC5_Decrypt(vectorDataIn, vectorDataOut);


 QByteArray result;
 convertVectorToByteArray(vectorDataOut, result);

 QString resultLine=QString::fromUtf8( result.data() );

 return resultLine;
}


// Шифрование файла на диске, вместо незашифрованного файла появляется зашифрованный
void CryptService::encryptFile(QByteArray key, QString fileName)
{
 encDecFileSmart(key, fileName, 0);
}


// Расшифровка файла на диске, вместо зашифрованного файла появляется нешифрованный
void CryptService::decryptFile(QByteArray key, QString fileName)
{
 encDecFileSmart(key, fileName, 1);
}


void CryptService::encDecFileSmart(QByteArray key, QString fileName, int mode)
{
 QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

 QFile file(fileName);

 if(!file.open(QIODevice::ReadOnly))
  criticalError("encDecFileSmart() : Cant open binary file "+fileName+" for reading.");

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 vector<unsigned char> vectorDataIn;
 convertByteArrayToVector(file.readAll(), vectorDataIn);

 vector<unsigned char> vectorDataOut;

 file.close();


 // Шифрация / дешифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 if(mode==0)
  rc5.RC5_Encrypt(vectorDataIn, vectorDataOut);
 else
  rc5.RC5_Decrypt(vectorDataIn, vectorDataOut);


 QByteArray result;
 convertVectorToByteArray(vectorDataOut, result);

 if(!file.open(QIODevice::WriteOnly))
  criticalError("encryptFile() : Cant open binary file "+fileName+" for write.");
 file.write(result);

 QApplication::restoreOverrideCursor();
}
