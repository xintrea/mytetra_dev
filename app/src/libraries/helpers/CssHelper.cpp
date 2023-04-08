#include <QtGlobal>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QDebug>

#include "CssHelper.h"

#include "libraries/GlobalParameters.h"


extern GlobalParameters globalParameters;


CssHelper::CssHelper()
{

}


qreal CssHelper::getCalculateIconSizePx(void)
{
  qreal dpi=QApplication::screens().at(0)->physicalDotsPerInch();

  qreal iconSizeMm=6; // Размер иконки в миллиметрах (рекомендованный)
  qreal iconSizeInch=iconSizeMm/25.4; // Размер иконки в дюймах
  qreal iconSizePx=iconSizeInch*dpi;

  return iconSizePx;
}


// Замена в CSS-стиле все вхождения подстроки META_ICON_SIZE на вычисленный размер иконки в пикселях
QString CssHelper::replaceCssMetaIconSize(QString styleText)
{
  styleText.replace( "META_ICON_SIZE", QString::number( (int) getCalculateIconSizePx() ) );
  styleText.replace( "META_ICON_HALF_SIZE", QString::number( (int)getCalculateIconSizePx()/2 ) );
  styleText.replace( "META_ICON_TWO_THIRDS_SIZE", QString::number( ((int)getCalculateIconSizePx()*2)/3 ) );
  styleText.replace( "META_ICON_QUARTER_SIZE", QString::number( (int)getCalculateIconSizePx()/4 ) );
  styleText.replace( "META_ICON_FIFTH_SIZE", QString::number( (int)getCalculateIconSizePx()/5 ) );
  styleText.replace( "META_ICON_SIXTH_SIZE", QString::number( (int)getCalculateIconSizePx()/6 ) );

  return styleText;
}


void CssHelper::setCssStyle()
{
  QString csspath = globalParameters.getWorkDirectory()+"/stylesheet.css";

  QFile css(csspath);

  bool openResult=css.open(QIODevice::ReadOnly | QIODevice::Text);

  // Если файла не существует
  if(!openResult)
  {
    qDebug() << "Stylesheet not found in " << csspath << ". Create new css file.";
    globalParameters.createStyleSheetFile( globalParameters.getWorkDirectory() );
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

