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
#include "libraries/helpers/UniqueIdHelper.h"

#include "Editor.h"
#include "EditorTextArea.h"


extern GlobalParameters globalParameters;


EditorTextArea::EditorTextArea(QWidget *parent) : QTextEdit(parent)
{
 flagShowFormatting=false;

 flagShowIndentEdge=false;
 posIndentEdge=0;

 // Включается генерация событий мышки при ее перемещении, а не только при кликах
 // Эти события нужны, чтобы менять форму курсора при наведении на ссылку при нажатой клавише Ctrl
 mouseCursorOverriden=false;
 setMouseTracking(true);

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

 return QTextEdit::eventFilter(o, e); // Продолжать обработку событий дальше
}


// Cлот отлавливает нажатия клавиш внутри виджета когда он активен (в фокусе)
void EditorTextArea::keyPressEvent(QKeyEvent *event)
{
  // Если нажата клавиша Ctrl
  if( event->key() == Qt::Key_Control )
    switchReferenceClickMode(true);

  QTextEdit::keyPressEvent(event);
}


// Cлот отлавливает отжатия клавиш внутри виджета когда он активен (в фокусе)
void EditorTextArea::keyReleaseEvent(QKeyEvent *event)
{
  // Если отжата клавиша Ctrl
  if( event->key() == Qt::Key_Control )
    switchReferenceClickMode(false);

  QTextEdit::keyReleaseEvent(event);
}


// Слот принимает глобальные нажатия клавиш. Это позволяет отслеживать нажатия даже если виджет неактивени (не в фокусе)
void EditorTextArea::onGlobalPressKey(int key)
{
  // Если нажата клавиша Ctrl
  if( key == Qt::Key_Control )
    switchReferenceClickMode(true);
}


// Слот принимает глобальные отжатия клавиш. Это позволяет отслеживать отжатия даже если виджет неактивени (не в фокусе)
void EditorTextArea::onGlobalReleaseKey(int key)
{
  // Если отжата клавиша Ctrl
  if( key == Qt::Key_Control )
    switchReferenceClickMode(false);
}


void EditorTextArea::switchReferenceClickMode(bool flag)
{
  if(flag)
  {
    // Сразу нужно проверить, не наведен ли курсор на ссылку, и если наведен, то поменять его вид
    QString href=anchorAt(currentMousePosition);
    if( !(href.isEmpty()) )
    {
      qApp->setOverrideCursor(QCursor(Qt::PointingHandCursor));
      mouseCursorOverriden = true;

      globalParameters.getStatusBar()->showMessage(href);
      qDebug() << "Cursor href in key event: " << href;
    }
    else
    {
      // Сразу нужно проверить, не наведен ли курсор на изображение, и если наведен, то поменять его вид
      QString imageName = imageAt(currentMousePosition);

      if(!imageName.isEmpty())
      {
        qApp->setOverrideCursor(QCursor(Qt::PointingHandCursor)); // Меняется форма курсора на указатель клика по ссылке
        mouseCursorOverriden = true;
        globalParameters.getStatusBar()->showMessage(imageName); // Имя изображения отображается в строке статуса
        qDebug() << "Cursor image in mouse event: " << imageName;
      }
    }
  }
  else
  {
    // Вид курсора сбрасывается на основной. Нужно для того, чтобы курсор поменялся,
    // если мышка в момент отжатия клавиши была наведена на ссылку или изображение и курсор был с указательным пальцем
    qApp->restoreOverrideCursor();

    mouseCursorOverriden = false;

    globalParameters.getStatusBar()->showMessage("");
  }

  qDebug() << "switchReferenceClickMode: " << flag;
}


void EditorTextArea::mouseMoveEvent(QMouseEvent *event)
{
  currentMousePosition=event->pos();

  // Если движение мышкой происходит при нажатой клавише Ctrl
  if( QApplication::keyboardModifiers() & Qt::ControlModifier )
  {
    QString href=anchorAt(currentMousePosition);

    // Если курсор мыши находится над ссылкой
    if(!href.isEmpty())
    {
      if(!mouseCursorOverriden)
      {
        qApp->setOverrideCursor(QCursor(Qt::PointingHandCursor)); // Меняется форма курсора на указатель клика по ссылке
        mouseCursorOverriden = true;
        globalParameters.getStatusBar()->showMessage(href); // Ссылка отображается в строке статуса
        qDebug() << "Cursor href in mouse event: " << href;
      }
    }
    else
    {
      QString imageName = imageAt(currentMousePosition);

      if(!imageName.isEmpty())
      {
        if(!mouseCursorOverriden)
        {
            qApp->setOverrideCursor(QCursor(Qt::PointingHandCursor)); // Меняется форма курсора на указатель клика по ссылке
            mouseCursorOverriden = true;
            globalParameters.getStatusBar()->showMessage(imageName); // Имя изображения отображается в строке статуса
            qDebug() << "Cursor image in mouse event: " << imageName;
        }
      }
      else
      {
        if(mouseCursorOverriden)
        {
            qApp->restoreOverrideCursor(); // Воостанавливается обычный курсор
            mouseCursorOverriden = false;
            globalParameters.getStatusBar()->showMessage("");
        }
      }
    }
  }
  else
    qApp->restoreOverrideCursor(); // Иначе клавиша Ctrl не нажата и курсор не может быть курсором ссылки

  QTextEdit::mouseMoveEvent(event);
}


void EditorTextArea::mousePressEvent(QMouseEvent *event)
{
    // Если клик происходит вместе с клавишей-модификатором Ctrl
    if( event->type()==QEvent::MouseButtonPress && (QApplication::keyboardModifiers() & Qt::ControlModifier) ) {
        QString href = this->anchorAt(event->pos());
        if(!href.isEmpty())
        {
            emit clickedOnReference(href);
        }
        else
        {
            QString imageName = imageAt(currentMousePosition);

            if(!imageName.isEmpty())
            {
                // Устанавливаем курсор на изображение, по которому кликнули
                setTextCursorFromPoint(currentMousePosition);

                // Открываем изображение
                emit clickOnImage();
            }
        }
    } else {
        qApp->restoreOverrideCursor();
    }

    QTextEdit::mousePressEvent(event);
}


void EditorTextArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    // Если происходит двойной клик по картинке
    if(this->currentCharFormat().isImageFormat()) {
        emit doubleClickOnImage();
    }

    QTextEdit::mouseDoubleClickEvent(event);
}


void EditorTextArea::paintEvent(QPaintEvent *event)
{
 // Отрисовка родительского класса
 QTextEdit::paintEvent(event);

 // В заблокированном виджете ничего дополнительного не рисуется
 if(isEnabled()==false)
   return;


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


 // Если символы форматирования не нужно рисовать
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
    if(y>viewport()->height())break; // Дальше рисовать не нужно

    // Рисуется точка на месте пробела
    p.drawRect(rect.left()+2,y,1,1);
   }
  else if(c_ansi=='\t') // Табуляция
   {
    QRect rect = cursorRect( cur );
    int y=(rect.top()+rect.bottom())/2;

    // Если координаты курсора вышли за размер области обзора
    if(y>viewport()->height())break; // Дальше рисовать не нужно

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
    if(y1>viewport()->height())break; // Дальше рисовать не нужно

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
    if(y1>viewport()->height())break; // Дальше рисовать не нужно

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
   return QTextEdit::canInsertFromMimeData(source); // Это не рекурсия, это вызов метода базового класса
}


// Переопределенная функция, срабатывает при вставке (paste) текста/объектов из буфера обмена
void EditorTextArea::insertFromMimeData(const QMimeData *source)
{
  int mimeDataFormat=detectMimeDataFormat( source );

  QTextCursor cursor = this->textCursor();
  QTextDocument *document = this->document();

  // Вставка обычного текста
  if(mimeDataFormat==MimeDataText)
  {
    QString text=source->text();
    cursor.insertText(text);
    return;
  }

  // Вставка HTML-кода
  if(mimeDataFormat==MimeDataHtml)
  {
    QString html=source->html();

    html=QString( html.toUtf8() );

    QByteArray byteArray=html.toUtf8();
    if(byteArray.contains( static_cast<char>(0) ))
    {
        qDebug() << "Problem in HTML code. It has null symbol.";
        qDebug() << "Null symbol pos: " << byteArray.indexOf( static_cast<char>(0) ) << " Text len: " << byteArray.size();
    }

    // Вызов процесса загрузки картинок
    // В конце процесса скачивания будет вызван слот EditorTextArea::onDownloadImagesSuccessfull()
    // в котором будут добавляться к документу скачанные картики и будет вставляться текст
    emit downloadImages(html);

    return;
  }

  // Вставка единичной картинки
  if(mimeDataFormat==MimeDataImage)
  {
    QImage image=qvariant_cast<QImage>(source->imageData());

    // Картинка будет хранится в ресурсах во внутреннем формате
    // без потери качества, поэтому затем при записи
    // легко сохраняется в PNG формат. Чтобы избежать путаницы,
    // сразу имя ресурса картинки задается как PNG файл
    QString imageName=getUniqueImageName();

    document->addResource(QTextDocument::ImageResource, QUrl(imageName), image);
    cursor.insertImage(imageName);
    return;
  }
}


int EditorTextArea::detectMimeDataFormat(const QMimeData *source)
{
  qDebug() << "DetectMimeDataFormat";

  // Однозначно текст
  if(source->hasText() && !source->hasHtml() && !source->hasImage())
    return MimeDataText;

  // Однозначно HTML
  if(!source->hasText() && source->hasHtml() && !source->hasImage())
    return MimeDataHtml;

  // Однозначно картинка
  if(!source->hasText() && !source->hasHtml() && source->hasImage())
    return MimeDataImage;

  // Есть текст, есть HTML, нет картинки = HTML
  if(source->hasText() && source->hasHtml() && !source->hasImage())
    return MimeDataHtml;

  // Есть текст, есть HTML и есть картинка = HTML (таблица из Calc, Excel)
  if(source->hasText() && source->hasHtml() && source->hasImage())
    return MimeDataHtml;

  // Нет текста, есть HTML и есть картинка = картинка
  if(!source->hasText() && source->hasHtml() && source->hasImage())
    return MimeDataImage;

  // В оставшихся случаях
  return MimeDataHtml;
}


// Обработка момента после загрузки всех картинок при вставке HTML-кода
void EditorTextArea::onDownloadImagesSuccessfull(const QString html,
                                                 const QMap<QString, QByteArray> referencesAndMemoryFiles,
                                                 const QMap<QString, QString> referencesAndInternalNames)
{
  // К документу добавляются скачанные картинки в виде ресурсов
  foreach (QString imageReference, referencesAndMemoryFiles.keys())
  {
    QImage image;
    bool result=image.loadFromData( referencesAndMemoryFiles.value(imageReference) );

    // Если данные с картинкой правильные, и с ней может работать Qt
    if(result)
    {
      // Картинка добавляется в ресурсы документа со внутренним именем
      this->document()->addResource(QTextDocument::ImageResource,
                                    QUrl( referencesAndInternalNames.value(imageReference) ),
                                    QVariant(image));
    }
  }

  // Вставляется HTML-код
  qDebug() << "Insert source HTML text: " << html;

  // Код вставляется не напрямую, а пропускается через временный документ,
  // чтобы вставить более понятный для QTextEdit HTML-код
  QTextDocument htmlFilterDoc;
  htmlFilterDoc.setHtml(html);

  qDebug() << "Insert filtered HTML text: " << htmlFilterDoc.toHtml();

  this->textCursor().insertHtml( htmlFilterDoc.toHtml() );
}


void EditorTextArea::onChangeFontcolor(const QColor &selectedColor)
{
  // TRACELOG

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


// Изменение цвета фона текста
void EditorTextArea::onChangeBackgroundColor(const QColor &selectedColor)
{
  // TRACELOG

  // Если выделение есть
  if(textCursor().hasSelection())
    setTextBackgroundColor(selectedColor); // Меняется цвет фона
  else
  {
    // Иначе надо выделить дополнительным курсором слово на
    // котором стоит курсор
    QTextCursor cursor=textCursor();
    cursor.select(QTextCursor::WordUnderCursor);

    QTextCharFormat format;
    format.setBackground(selectedColor);

    cursor.mergeCharFormat(format);
  }
}


void EditorTextArea::onChangeFontFamily(QString fontFamily)
{
  // TRACELOG

  qDebug() << "Apply font family " << fontFamily;

  // Ранее для установки шрифта хватало одной команды setFontFamily(fontFamily);
  // Теперь так не работает, новый код сделан на основе Qt примера Text Edit

  QTextCharFormat format;
  format.setFontFamily(fontFamily);

  qDebug() << "Font from font name: " << format.font().toString();

  // Если нет выделения, дополнительным курсором выделяется слово, на котором стоит курсор
  QTextCursor cursor = this->textCursor();
  if (!cursor.hasSelection())
  {
      cursor.select(QTextCursor::WordUnderCursor);
  }

  cursor.mergeCharFormat(format);
  this->mergeCurrentCharFormat(format);
}


void EditorTextArea::onChangeFontPointSize(int n)
{
  // TRACELOG

  setFontPointSize(n);
}

QString EditorTextArea::imageAt(const QPoint& point)
{
  int scrollY = verticalScrollBar()->value();
  int scrollX = horizontalScrollBar()->value();
  QPoint mousePositionWithScroll = QPoint(point.x()+scrollX, point.y()+scrollY);
  return document()->documentLayout()->imageAt(mousePositionWithScroll);
}

int EditorTextArea::hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy)
{
  int scrollY = verticalScrollBar()->value();
  int scrollX = horizontalScrollBar()->value();
  QPoint mousePositionWithScroll = QPoint(point.x()+scrollX, point.y()+scrollY);
  return document()->documentLayout()->hitTest(mousePositionWithScroll, accuracy);
}

void EditorTextArea::setTextCursorFromPoint(const QPointF &point)
{
  int pos = hitTest(point, Qt::ExactHit);
  if (pos < 0)
    return;
  QTextCursor cursor(textCursor());
  if (!cursor.atEnd())
    pos+=1;
  cursor.setPosition(pos);
  setTextCursor(cursor);
}

