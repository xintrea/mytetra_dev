#include <QMessageBox>
#include <QCryptographicHash>
#include <QtGlobal>
#include <QByteArray>
#include <QString>

#include "main.h"
#include "Password.h"
#include "Pbkdf2Qt.h"
#include "CryptService.h"

#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/enterPassword/EnterPassword.h"
#include "models/dataBaseConfig/DataBaseConfig.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;
extern DataBaseConfig dataBaseConfig;


Password::Password(void)
{ 
}


Password::~Password(void)
{ 
}


// Установка пароля
// Если пароль в системе еще не использовался, пароль преобразуется в ключ 
// и ключ запоминается в память
// Если пароль в системе уже был установлен, проверяется, задан ли правильный пароль
// путем дешифровки проверочного текста
// Только если пароль правилен, устанавливается ключ
bool Password::retrievePassword()
{
  // Если пароль в данной сессии не вводился
  if(globalParameters.getCryptKey().size()==0)
  {

    // Если в хранилище данных вообще не задан пароль
    if(dataBaseConfig.get_crypt_mode()==0)
    {
      // Запрашивается пароль
      EnterPassword enterPwd(ENTER_PASSWORD_MODE_DOUBLE);
      int i=enterPwd.exec();
      if(i==QDialog::Rejected)
        return false; // Была нажата отмена, ничего ненужно делать

      // В этом месте пароль введен правильно и подтвержден пользователем
      QString password=enterPwd.getPassword();


      // Вычисляется и запоминается в память ключ шифрования
      setCryptKeyToMemory(password);

      // Вычисляются и запоминаются в конфиг хранилища данных
      // значения для последующей проверки пароля при вводе
      saveCheckPasswordKey(password);

      return true;
    }
    else
    {
      // Иначе пароль уже задан в системе
      // И нужно либо запросить пароль,
      // Либо воспользоваться промежуточным хешем если пароль был запомнен


      // Проверяется, запомнен ли пароль (точнее, промежуточный хеш)
      if( mytetraConfig.getPasswordSaveFlag() &&
          mytetraConfig.getPasswordMiddleHash().length()>0)
      {
        // Пароль хранится в системе

        // Если хранимый пароль (точнее, промежуточный хеш) правильный
        if(checkMiddleHash()==true)
        {
          setCryptKeyToMemoryFromMiddleHash();

          // И пароль у пользователя запрашивать ненужно
          return true;
        }
        else
          mytetraConfig.setPasswordMiddleHash(""); // хранимый пароль сбрасывается
      }


      // Запрос пароля у пользователя
      return enterExistsPassword();

    } // Закрылось условие что пароль уже есть в конфиге хранилища данных
  }

  // Здесь считается, что в данной сессии пароль вводился
  // а если вводился и установлен, то вводился правильно
  return true;
}


// Замена пароля
bool Password::replacePassword(void)
{
  // Если в хранилище данных вообще не задан пароль
  // Замена пароля невозможна
  if(dataBaseConfig.get_crypt_mode()==0)
    return false;


  // Запрашивается пароль
  EnterPassword enterPwd(ENTER_PASSWORD_MODE_WITH_PREVIOUS);
  int i=enterPwd.exec();
  if(i==QDialog::Rejected)
    return false; // Была нажата отмена, ничего ненужно делать


  // Здесь считается, что введен старый и новый пароль
  // и строки с паролями ненулевые
  QString previousPassword=enterPwd.getPreviousPassword();
  QString password=enterPwd.getPassword();


  // Проверяется, правильно ли введен старый пароль
  if(checkPasswordWithExists(previousPassword)==false)
  {
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Password entering"));
    msgBox.setText(tr("Wrong old password"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();

    // В памяти сбрасывается возможно вводимый ранее правильно пароль
    globalParameters.setCryptKey(QByteArray());

    return false;
  }

  // ------------------------
  // Данные перешифровываются
  // ------------------------

  // Выясняется ссылка на модель дерева данных
  KnowTreeModel *dataModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());

  // Перешифрация
  dataModel->reEncrypt(previousPassword, password);


  // Вычисляется и запоминается в память ключ шифрования
  setCryptKeyToMemory(password);

  // Вычисляются и запоминаются в конфиг хранилища данных
  // значения для последующей проверки пароля при вводе
  saveCheckPasswordKey(password);

  // Сообщение что пароль успешно изменен
  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Change Password"));
  msgBox.setText(tr("Password successfully changed"));
  msgBox.setIcon(QMessageBox::Information);
  msgBox.exec();

  return true;
}


void Password::setCryptKeyToMemory(QString password)
{
  QByteArray middleHash=calculateMiddleHash(password);

  // Ключ в виде MD5
  QByteArray key=QCryptographicHash::hash(middleHash, QCryptographicHash::Md5);
  // qDebug() << "Password::setCryptKeyToMemory() : Set crypt key to:" << key.toHex();

  // Ключ запоминается в память
  globalParameters.setCryptKey(key);
}


void Password::setCryptKeyToMemoryFromMiddleHash(void)
{
  // Из общего конфига считывается промежуточный хеш
  QByteArray middleHash=QByteArray::fromBase64( mytetraConfig.getPasswordMiddleHash().toLatin1() );

  // Ключ в виде MD5
  QByteArray key=QCryptographicHash::hash(middleHash, QCryptographicHash::Md5);
  // qDebug() << "Set crypt key from middle hash to:" << key.toHex();

  // Ключ запоминается в память
  globalParameters.setCryptKey(key);
}


// Проверка правильности хранимого пароля (точнее промежуточного хеша)
bool Password::checkMiddleHash(void)
{
  // Из общего конфига считывается промежуточный хеш
  QByteArray middleHash=QByteArray::fromBase64( mytetraConfig.getPasswordMiddleHash().toLatin1() );
  // qDebug() << "Password::checkMiddleHash() : middleHash :" << middleHash.toHex();

  // Промежуточный хеш преобразуется в ключ (MD5 сумма)
  QByteArray key=QCryptographicHash::hash(middleHash, QCryptographicHash::Md5);
  // qDebug() << "Password::checkMiddleHash() : Crypt key :" << key.toHex();

  // Из конфига базы данных считываются проверочные данные
  QByteArray checkData=QByteArray::fromBase64( dataBaseConfig.get_middle_hash_check_data().toLatin1() );

  // Проверочные данные расшифровыватся с помощью ключа
  QString line=CryptService::decryptStringFromByteArray(key, checkData);


  // Сравниватся проверочные данные
  if(line==SAVED_PASSWORD_CHECKING_LINE)
    return true;
  else
  {
    qDebug() << "Password::checkMiddleHash() : Check data not equivalent";

    return false;
  }
}


QByteArray Password::calculateMiddleHash(QString password)
{
  QByteArray salt("^1*My2$Tetra3%_4[5]");
  QByteArray middleHash;

  // Хеш PBKDF2 от пароля и соли
  Pbkdf2Qt hashAlgorythm;
  middleHash=hashAlgorythm.Pbkdf2(password.toUtf8(),
                                  salt,
                                  CRYPT_CHECK_ROUNDS,
                                  CRYPT_CHECK_HASH_LEN);

  // qDebug() << "Password::calculateMiddleHash() : middleHash " << middleHash.toHex();

  return middleHash;
}


void Password::smartSaveMiddleHash(QString password)
{
  // Если стоит настройка, что надо хранить пароль локально,
  // и пароль (точнее промежуточный хеш) еще не запомнен
  if(mytetraConfig.getPasswordSaveFlag() &&
     mytetraConfig.getPasswordMiddleHash().length()==0)
  {
    QByteArray middleHash=calculateMiddleHash(password);
    // qDebug() << "Password::smartSaveMiddleHash() : middleHash " << middleHash.toHex();

    // В общий конфиг запоминается промежуточный хеш
    mytetraConfig.setPasswordMiddleHash( QString(middleHash.toBase64().data()) );

    // В конфиг базы данных запоминаются проверочные данные
    // Эти данные нужны для последующей проверки промежуточного хеша
    saveMiddleHashCheckData(middleHash);

    // Выводится уведомление что пароль сохранен
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Password"));
    msgBox.setText(tr("The password is saved"));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
  }
  else
  {
    // В общем конфиге сбрасывается промежуточный хеш
    mytetraConfig.setPasswordMiddleHash("");
  }

}


void Password::saveMiddleHashCheckData(QByteArray middleHash)
{
  // Вычисляются проверочные данные для промежуточного хеша
  QByteArray checkData=calculateMiddleHashCheckData(middleHash);
  // qDebug() << "Password::saveMiddleHashCheckData() : checkData " << checkData.toHex();

  // Проверочные данные запоминаются в конфиг базы данных
  dataBaseConfig.set_middle_hash_check_data( QString(checkData.toBase64().data()) );
}


QByteArray Password::calculateMiddleHashCheckData(QByteArray middleHash)
{
  // Вычисляется ключ шифрования на основе промежуточного хеша
  QByteArray key=QCryptographicHash::hash(middleHash, QCryptographicHash::Md5);

  QString stringForCheck=SAVED_PASSWORD_CHECKING_LINE;

  // Шифруется проверочная строка (результат в кодировке Base64)
  QString encrypt=CryptService::encryptString(key, stringForCheck);

  // Base64 преобразуется в набор байт
  QByteArray checkData=QByteArray::fromBase64(encrypt.toLatin1());

  return checkData;
}


// Вычисляются и запоминаются в конфиг хранилища данных
// соль и хеш пароля для последующей проверки пароля при вводе
void Password::saveCheckPasswordKey(QString password)
{
  QByteArray hash;
  QByteArray salt;

  for(int i=0; i<32; i++)
    salt.append(rand()%0xFF);

  Pbkdf2Qt hashAlgorythm;
  hash=hashAlgorythm.Pbkdf2(password.toUtf8(),
                            salt,
                            CRYPT_CHECK_ROUNDS,
                            CRYPT_CHECK_HASH_LEN);

  dataBaseConfig.set_crypt_check_hash( QString(hash.toBase64().data()) );
  dataBaseConfig.set_crypt_check_salt( QString(salt.toBase64().data()) );
  dataBaseConfig.set_crypt_mode(1);

  smartSaveMiddleHash(password);
}


// Запрос пароля в случае, если в системе уже задавался пароль шифрования
// Если пароль введен правильно, возвращается true, иначе false
bool Password::enterExistsPassword(void)
{
  // Запрос пароля и проверка его хеша

  // Запрашивается пароль
  EnterPassword enterPwd(ENTER_PASSWORD_MODE_SINGLE);
  if(mytetraConfig.get_autoClosePasswordEnable())
    enterPwd.setCancelDelay( mytetraConfig.get_autoClosePasswordDelay() );
  int i=enterPwd.exec();
  if(i==QDialog::Rejected)
    return false; // Была нажата отмена, ничего ненужно делать

  // В этом месте пароль введен
  QString password=enterPwd.getPassword();

  // Если пароль введен правильно
  if( checkPasswordWithExists(password) )
  {
    // В память запоминается шифрующий ключ
    setCryptKeyToMemory(password);


    // Дополнительно, если в общих настройках указано,
    // что пароль нужно запоминать, и он (точнее промежуточный хеш )
    // еще не запомнен
    if(mytetraConfig.getPasswordSaveFlag() &&
       mytetraConfig.getPasswordMiddleHash().length()==0)
      smartSaveMiddleHash(password);

    return true;
  }
  else
  {
    // Иначе введен неправильный пароль
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Password entering"));
    msgBox.setText(tr("Wrong password"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();

    return false;
  }
}


void Password::resetPassword(void)
{
  // Если пароль небыл установлен, сбрасывать нечего
  if(dataBaseConfig.get_crypt_mode()==0)
    return;

  // Задается вопрос, не нужно ли сбросить (обнулить) пароль
  QMessageBox messageBox;
  messageBox.setWindowTitle(tr("Reset crypt password"));
  messageBox.setText(tr("Your items tree no more of encrypted data. Would you like to remove the encrypt password?"));
  messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
  QAbstractButton *resetButton=messageBox.addButton(tr("Remove password"), QMessageBox::AcceptRole);
  messageBox.exec();

  // Если пользователь подтвердил
  if(messageBox.clickedButton() == resetButton)
  {
    dataBaseConfig.set_crypt_mode(0);
    dataBaseConfig.set_crypt_check_salt("");
    dataBaseConfig.set_crypt_check_hash("");

    mytetraConfig.setPasswordMiddleHash("");
    dataBaseConfig.set_middle_hash_check_data("");

    // Ключ в памяти удаляется
    globalParameters.setCryptKey(QByteArray());
  }
}


// Проверка переданного пароля на соответсвие запомненному хешу
bool Password::checkPasswordWithExists(QString password)
{
  // Из конфига хранилища даннных считывается соль и хеш пароля
  QByteArray hash=QByteArray::fromBase64( dataBaseConfig.get_crypt_check_hash().toLatin1() );
  QByteArray salt=QByteArray::fromBase64( dataBaseConfig.get_crypt_check_salt().toLatin1() );


  // Вычисляется хеш пароля с использованием считанной соли
  QByteArray passwordHash;
  Pbkdf2Qt hashAlgorythm;
  passwordHash=hashAlgorythm.Pbkdf2(password.toUtf8(),
                                    salt,
                                    CRYPT_CHECK_ROUNDS,
                                    CRYPT_CHECK_HASH_LEN);


  // Если хеши совпадают, значит пароль введен правильный
  if(hash==passwordHash)
    return true;
  else
    return false;
}
