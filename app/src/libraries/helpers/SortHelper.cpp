#include "SortHelper.h"


// Функция для сортировки массива из QStringList исходя из длин списков
bool compareQStringListLen(const QStringList &list1, const QStringList &list2)
{
  return list1.size() < list2.size();
}


