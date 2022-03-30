#include <QtGlobal>
#include <QApplication>
#include <QScreen>

#include "ScreenHelper.h"


ScreenHelper::ScreenHelper()
{

}


int ScreenHelper::getSizeX(void)
{
  int size=(QApplication::screens().at(0)->availableGeometry()).width();
  return size;
}


int ScreenHelper::getSizeY(void)
{
  int size=(QApplication::screens().at(0)->availableGeometry()).height();
  return size;
}
