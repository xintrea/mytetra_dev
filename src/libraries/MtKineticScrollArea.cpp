#include <QScroller>
#include <QScrollerProperties>
#include <QScrollBar>
#include <QVariant>

#include "MtKineticScrollArea.h"
#include "libraries/GlobalParameters.h"


extern GlobalParameters globalParameters;

MtKineticScrollArea::MtKineticScrollArea()
{
 if(globalParameters.getTargetOs()=="android")
 {
   QScroller *scroller = QScroller::scroller(this);

   // For desktop - QScroller::LeftMouseButtonGesture, for Android - QScroller::TouchGesture in doc
   // TouchGesture по факту на Андроиде не работает, а LeftMouseButtonGesture - почему-то работает
   scroller->grabGesture(this, QScroller::LeftMouseButtonGesture);

   QScrollerProperties properties = scroller->scrollerProperties();
   QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
   properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
   properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
   scroller->setScrollerProperties(properties); // QScrollerProperties::OvershootAlwaysOff

   horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
   verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");

   setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
   // setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
 }
}

MtKineticScrollArea::~MtKineticScrollArea()
{

}
