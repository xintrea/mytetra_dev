#include <QtGlobal>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QDebug>

#include "CssHelper.h"

#include "libraries/GlobalParameters.h"
#include "models/appConfig/AppConfig.h"
#include "models/appConfig/AppFiles.h"

extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;
extern AppFiles mytetraFiles;

CssHelper::CssHelper()
{

}


qreal CssHelper::getCalculateIconSizePx(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
  qreal dpiX=qApp->desktop()->physicalDpiX();
  qreal dpiY=qApp->desktop()->physicalDpiY();
  qreal dpi=(dpiX+dpiY)/2;
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
  qreal dpi=QApplication::screens().at(0)->physicalDotsPerInch();
#endif

  qreal iconSizeMm=6; // Размер иконки в миллиметрах (рекомендованный)
  qreal iconSizeInch=iconSizeMm/25.4; // Размер иконки в дюймах
  qreal iconSizePx=iconSizeInch*dpi;

  return iconSizePx;
}


// Замена в CSS-стиле все вхождения подстроки META_ICON_SIZE на вычисленный размер иконки в пикселях
QString CssHelper::replaceCssMetaIconSize(QString styleText)
{
    QMap<QString, qreal> table;
    table["META_ICON_FOUR_SIZE"]       = getCalculateIconSizePx() * 4.0;
    table["META_ICON_TRIPLE_SIZE"]     = getCalculateIconSizePx() * 3.0;
    table["META_ICON_DOUBLE_SIZE"]     = getCalculateIconSizePx() * 2.0;
    table["META_ICON_ONEANDHALF_SIZE"] = getCalculateIconSizePx() * 1.5;

    table["META_ICON_SIZE"] = getCalculateIconSizePx();

    table["META_ICON_SEVFIVEPERC_SIZE"]  = getCalculateIconSizePx() * 0.75;
    table["META_ICON_HALF_SIZE"]         = getCalculateIconSizePx() / 2.0;
    table["META_ICON_THIRD_PART_SIZE"]   = getCalculateIconSizePx() / 3.0;
    table["META_ICON_QUARTER_PART_SIZE"] = getCalculateIconSizePx() / 4.0;

    for (auto name : table.keys())
    {
      qreal value=table.value(name);
      styleText.replace( name, QString::number( (int) value )+"px" );
    }

    return styleText;
}


void CssHelper::setCssStyle()
{
  QString dirName=globalParameters.getWorkDirectory();

  // Если файл стилей есть по старому пути, переносим его в каталог style
  QFile file(dirName+"/stylesheet.css");
  if (file.exists())
  {
    QDir styleDir(dirName);
    styleDir.mkdir("style");
    file.rename(dirName+"/style/stylesheet.css");
  }

  QString csspath = dirName+"/style/stylesheet.css";

  QFile css(csspath);

  bool openResult=css.open(QIODevice::ReadOnly | QIODevice::Text);

  // Если файла не существует
  if(!openResult)
  {
    qDebug() << "Stylesheet not found in " << csspath << ". Create new css file.";

    // Вычисляется имя темы, с которой надо создать файл темы
    QString themeName;
    switch (mytetraConfig.getInterfaceTheme())
    {
        case AppConfig::Light: themeName="light"; break;
        case AppConfig::Dark: themeName="dark"; break;
    }

    mytetraFiles.createThemesFiles( globalParameters.getWorkDirectory(), themeName);
  }

  css.close();

  // Заново открывается файл
  if(css.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "Stylesheet success loaded from" << csspath;
    QString style = QTextStream(&css).readAll();

    style=CssHelper::replaceCssMetaIconSize(style);

    qApp->setStyleSheet(style);
  }
}

