#include <ctime>

#include <QString>

#include "UniqueIdHelper.h"


// Функция всегда возвращает уникальный идентификатор
QString getUniqueId(void)
{
 // Уникальный идентификатор состоит из 10 цифр количества секунд с эпохи UNIX
 // и 10 случайных символов 0-9 a-z

 // Строка из 10 случайных символов
 const char * const symbols="0123456789abcdefghijklmnopqrstuvwxyz";

 QString line = QString::number(time(nullptr), 10);
 line=line.rightJustified(10, '0');

 for(int i=0; i<10; i++)
  line+=symbols[rand()%strlen(symbols)];

 return line;
}


// Получение псевдо-уникального имени картинки
// Внутренний формат для храния картинок в MyTetra - png
// Вызывающий код должен при необходимости проверять уникальность имени картинки в пределах одной записи
QString getUniqueImageName(void)
{
    return "image"+getUniqueId()+".png"; // "image"+QString::number(rand())+".png";
}

