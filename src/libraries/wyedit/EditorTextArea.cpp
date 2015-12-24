#include <QPainter>
#include <QRect>
#include <QTextCursor>
#include <QtGlobal>
#include <QScrollBar>
#include <QUrl>

#include "../../main.h"
#include "../TraceLogger.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/GlobalParameters.h"

#include "Editor.h"
#include "EditorTextArea.h"


extern GlobalParameters globalParameters;


EditorTextArea::EditorTextArea(QWidget *parent) : QTextEdit(parent)
{
 flagShowFormatting=false;

 flagShowIndentEdge=false;
 posIndentEdge=0;

 // Устанавливается фильтр на событие показа или скрытия области прокрутки
 this->verticalScrollBar()->installEventFilter(this);

 // this->setStyleSheet("QTextEdit::table {border:1px solid maroon; border-collapse:collapse;}");

 // Разрешение принимать жест QTapAndHoldGesture
 grabGesture(Qt::TapAndHoldGesture);
}


EditorTextArea::~EditorTextArea(void)
{

}


bool EditorTextArea::get_showformatting(void)
{
 return flagShowFormatting;
}


void EditorTextArea::set_showformatting(bool i)
{
 flagShowFormatting=i;

 viewport()->update();
}


// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool EditorTextArea::event(QEvent *event)
{
  if (event->type() == QEvent::Gesture)
  {
    // qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
    return gestureEvent(static_cast<QGestureEvent*>(event));
  }

  return QTextEdit::event(event);
}


// Обработчик жестов
// Вызывается из обработчика событий
bool EditorTextArea::gestureEvent(QGestureEvent *event)
{
  // qDebug() << "In gestureEvent()" << event;

  if (QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
    tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

  return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void EditorTextArea::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
  // qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

  if(gesture->state()==Qt::GestureFinished)
    if(globalParameters.getTargetOs()=="android")
      emit tapAndHoldGestureFinished( mapFromGlobal(gesture->position().toPoint()) );
}


bool EditorTextArea::eventFilter(QObject *o, QEvent *e)
{
 // Обработка показа или скрытия области прокрутки редактора
 if(o==this->verticalScrollBar())
  if(e->type()==QEvent::Hide || e->type()==QEvent::Show)
    emit updateIndentlineGeometry();

 return false; // Продолжать оработку событий дальше
}


void EditorTextArea::paintEvent(QPaintEvent *event)
{
 // Отрисовка родительского класса
 QTextEdit::paintEvent(event);

 // Если нужно нарисовать линию какого-нибудь отступа
 if(flagShowIndentEdge==true)
  {
   QPainter p(viewport());
   p.setRenderHint(QPainter::Antialiasing,false);
   p.setRenderHint(QPainter::HighQualityAntialiasing,false);

   QPen pen(Qt::darkGray);
   pen.setStyle(Qt::SolidLine);
   p.setPen(pen);

   p.drawLine(posIndentEdge, 1,
              posIndentEdge, this->height()-1);
  }


 // Если символы форматирования ненужно рисовать
 if(flagShowFormatting==false)
   return;

 // Начало области поиска
 int documentStartFrameSearch=0;

 // Конец области поиска
 QTextCursor cur=textCursor();
 cur.movePosition(QTextCursor::End);
 int documentEndFrameSearch=cur.position();

 int textLenght=documentEndFrameSearch;
 // qDebug() << "Document lenght " << text_lenght;

 int analysePoint=0;
 int startFrameIterationCount=0;

 while(true)
 {
  // Новая проверяемая позиция курсора устанавливается в середину области поиска
  analysePoint=(documentStartFrameSearch+documentEndFrameSearch)/2;
  cur.setPosition(analysePoint);

  // qDebug() << "Start " << document_start_frame_search << " End " << document_end_frame_search << " Analyse position " << analyse_point;

  // Дополнительный курсор смещается на символ влево
  QTextCursor dopCursor=cur;
  bool previosResult=dopCursor.movePosition(QTextCursor::PreviousCharacter);

  // Если смещаться было некуда, значит начало документа
  // и начало видимой области совпадают
  if(previosResult==false)
   break;

  QRect dopCursorRectangle=cursorRect(dopCursor);
  QRect cursorRectangle=cursorRect(cur);

  // Если текущая позиция является границей начала видимой области
  if(dopCursorRectangle.top()<0 && cursorRectangle.top()>=0)
   break;

  // Нужно решить куда переносить границы поиска
  // Если координаты курсора положительные, значит курсор
  // находится "ниже" начала области обзора
  if(cursorRectangle.top()>0)
   documentEndFrameSearch=analysePoint; // Нижняя граница двигается вверх
  else
   documentStartFrameSearch=analysePoint; // Верхняя граница двигается вниз

  // Если границы сомкнулись
  if((documentStartFrameSearch+1)==documentEndFrameSearch ||
     documentStartFrameSearch==documentEndFrameSearch)
   {
    // qDebug() << "Edge eqivalent";
    cur.setPosition(documentStartFrameSearch);
    break;
   }

  startFrameIterationCount++;
 }

 // qDebug() << "Found vision area at position " << cur.position() << " Iterations " << start_frame_iteration_count;

 // Поверх рисуются элементы разметки
 QPainter p(viewport());
 p.setRenderHint(QPainter::Antialiasing,false);
 p.setRenderHint(QPainter::HighQualityAntialiasing,false);

 QPen pen(Qt::darkGray);
 pen.setStyle(Qt::SolidLine);
 p.setPen(pen);

 do {

  // Выясняется текущий обрабатываемый символ
  QChar c=document()->characterAt(cur.position());
  char c_ansi=c.toLatin1(); // Ранее было toAscii(), но этот метод теперь не поддерживается в Qt 5.2

  if(c_ansi==' ') // Пробел
   {
    QRect rect = cursorRect( cur );
    int y=(rect.top()+rect.bottom())/2;

    // Если координаты курсора вышли за размер области обзора
    if(y>viewport()->height())break; // Дальше рисовать ненужно

    // Рисуется точка на месте пробела
    p.drawRect(rect.left()+2,y,1,1);
   }
  else if(c_ansi=='\t') // Табуляция
   {
    QRect rect = cursorRect( cur );
    int y=(rect.top()+rect.bottom())/2;

    // Если координаты курсора вышли за размер области обзора
    if(y>viewport()->height())break; // Дальше рисовать ненужно

    // Рисуется стрелка на месте табуляции
    p.drawLine(rect.left()+2,y,rect.left()+10,y);
    p.drawLine(rect.left()+7,y-3,rect.left()+10,y);
    p.drawLine(rect.left()+10,y,rect.left()+7,y+3);
   }
  else if(c.unicode()==QChar::ParagraphSeparator) // Перевод строки (в виде границы между абзацами)
   {
    QRect rect = cursorRect( cur );
    int y1=rect.top()+3;
    int y2=rect.bottom()-3;
    int h=y2-y1;
    int w=h/2+2;

    // Если координаты курсора вышли за размер области обзора
    if(y1>viewport()->height())break; // Дальше рисовать ненужно

    // Рисуется пи на месте перевода строки
    p.drawLine(rect.left()+(w/8)+3,y1,rect.left()+(w/8)+3,y2);
    p.drawLine(rect.left()+w-(w/8)-1,y1,rect.left()+w-(w/8)-1,y2);

    p.drawLine(rect.left()+1,y1,rect.left()+w,y1);
    p.drawLine(rect.left()+1,y1,rect.left()+1,y1+h/4);
   }
  else if(c.unicode()==QChar::LineSeparator) // Перевод строки (в виде <br/>)
   {
    QRect rect = cursorRect( cur );
    int y1=rect.top();
    int y2=(rect.top()+rect.bottom())/2;
    int h=y2-y1;
    int w=h/2+1;

    // Если координаты курсора вышли за размер области обзора
    if(y1>viewport()->height())break; // Дальше рисовать ненужно

    // Рисуется изогнутая стрелка на месте перевода строки
    p.drawLine(rect.left(),   y2, rect.left()+w, y2);
    p.drawLine(rect.left()+w, y2, rect.left()+w, y1);

    p.drawLine(rect.left(), y2, rect.left()+w/2, y2-h/2);
    p.drawLine(rect.left(), y2, rect.left()+w/2, y2+h/2);    
   }


  // Проверяется, можно ли сдвинуть дальше курсор
  // проверять нужно, так как movePosition иногда зависает при
  // перемещении курсора
  if(cur.position()>=textLenght) break;

  cur.movePosition(QTextCursor::NextCharacter);
 }
 while( !cur.isNull() );
}


void EditorTextArea::resizeEvent(QResizeEvent *event)
{
 // Отрисовка родительского класса
 QTextEdit::resizeEvent(event);

 // Надо обновить геометрию виджета настройки отступов
 emit updateIndentlineGeometry();
}


// Метод возвращает X-координату курсора в "нуливой" позиции слева
// Это значение используется для работы линейки отступов
int EditorTextArea::get_indent_started_left(void)
{
 return this->lineWidth() + (int)this->document()->documentMargin() + 1;
}


// Метод возвращает X-координату курсора в самой крайней правой позиции
// Это значение используется для работы линейки отступов
int EditorTextArea::get_indent_started_right(void)
{
 return this->lineWidth() + this->viewport()->width() + 1 - (int)this->document()->documentMargin();
}


// Установка видимости линии настройки отступа
void EditorTextArea::show_indetedge(bool i)
{
 flagShowIndentEdge=i;
 viewport()->update();
}


// Установка X-координаты линии настройки отступа
void EditorTextArea::set_indentedge_pos(int i)
{
 posIndentEdge=i;
 viewport()->update();
}



// Метод, определяющий какие типы данных можно вставлять в текст
// Нужен для того, чтоб сделать вставку картинок
bool EditorTextArea::canInsertFromMimeData(const QMimeData *source) const
{
  if(source->hasImage())
   return true;
  else
   return QTextEdit::canInsertFromMimeData(source);
}


// Вставка MIME данных
void EditorTextArea::insertFromMimeData(const QMimeData *source)
{
 QTextCursor cursor = this->textCursor();
 QTextDocument *document = this->document();

 // Вставка картинки
 if(source->hasImage())
  {
   QImage image=qvariant_cast<QImage>(source->imageData());

   // Картинка будет хранится в ресурсах во внутреннем формате
   // без потери качества, поэтому затем при записи
   // легко сохраняется в PNG формат. Чтобы избежать путаницы,
   // сразу имя ресурса картинки задается как PNG файл
   QString imageName="image"+QString::number(rand())+".png";

   document->addResource(QTextDocument::ImageResource, QUrl(imageName), image);
   cursor.insertImage(imageName);
   return;
  }

 if(source->hasHtml())
 {
  QString html=qvariant_cast<QString>(source->html());
  cursor.insertHtml(html);
  return;
 }

 if(source->hasText())
 {
  QString text=qvariant_cast<QString>(source->text());
  cursor.insertText(text);
  return;
 }

}


void EditorTextArea::onChangeFontcolor(QColor selectedColor)
{
  TRACELOG

  // Если выделение есть
  if(textCursor().hasSelection())
    setTextColor(selectedColor); // Меняется цвет текста
  else
  {
    // Иначе надо выделить дополнительным курсором слово на
    // котором стоит курсор
    QTextCursor cursor=textCursor();
    cursor.select(QTextCursor::WordUnderCursor);

    QTextCharFormat format;
    format.setForeground(selectedColor);

    cursor.mergeCharFormat(format);
  }
}


void EditorTextArea::onChangeFontFamily(QString fontFamily)
{
  TRACELOG

  setFontFamily(fontFamily);
}


void EditorTextArea::onChangeFontPointSize(int n)
{
  TRACELOG

  setFontPointSize(n);
}

