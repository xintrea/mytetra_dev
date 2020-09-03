#include <QtGlobal>
#include <QAbstractItemView>
#include <QScroller>
#include <QScrollBar>

#include "GestureHelper.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


GestureHelper::GestureHelper()
{

}


void GestureHelper::setKineticScrollArea(QAbstractItemView *object)
{
  #if QT_VERSION < 0x050000

   Q_UNUSED(object);
   return;

  #else

  if(object==nullptr)
    return;

  if(globalParameters.getTargetOs()=="android")
  {
    // Настройка жестов прокрутки
    QScroller *scroller = QScroller::scroller(object);

    // For desktop - QScroller::LeftMouseButtonGesture, for Android - QScroller::TouchGesture in doc
    // TouchGesture по факту на Андроиде не работает, а LeftMouseButtonGesture - почему-то работает
    scroller->grabGesture(object, QScroller::LeftMouseButtonGesture);

    // Поведение прокрутки на краях списка (сейчас не пружинит)
    QScrollerProperties properties = scroller->scrollerProperties();
    QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
    properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
    properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
    scroller->setScrollerProperties(properties); // QScrollerProperties::OvershootAlwaysOff

    // Горизонтальный скроллинг скрывается
    object->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");

    // QScrollBar::add-line:horizontal { border: none; background: none; } QScrollBar::sub-line:horizontal { border: none; background: none; }
    // QScrollBar {width:3px;}
    // QScrollBar::up-arrow, QScrollBar::down-arrow {width: 0px; height: 0px;}
    // QScrollBar::add-line:vertical { height: 0px; } QScrollBar::sub-line:vertical { height: 0px; }
    // QScrollBar::add-line:vertical { border: none; background: none; height: 0px; } QScrollBar::sub-line:vertical { border: none; background: none; height: 0px; }
    // background: transparent; background-color:transparent;
    // "QScrollBar::up-arrow, QScrollBar::down-arrow {width: 0px; height: 0px;}"
    object->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width:3px; border: none; background: transparent; margin: 0;}"
                                               "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {width: 0px; height: 0px; border: none;  background: transparent; image: url(:/resource/pic/transparent_dot.png); }"
                                               "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { image: url(:/resource/pic/transparent_dot.png); }");



    /*
    object->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {border: 2px solid black; background: grey; height: 15px;}"
                                                 "QScrollBar::add-line:horizontal {border none; background: none;}"
                                                 "QScrollBar::sub-line:horizontal {border none; background: none;}"
                                                 );
    */

    object->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  }

  #endif
}

