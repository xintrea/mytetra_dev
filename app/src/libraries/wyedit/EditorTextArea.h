#ifndef _EDITORTEXTAREA_H_
#define _EDITORTEXTAREA_H_

#include <QTextEdit>
#include <QPaintEvent>
#include <QMimeData>
#include <QTapAndHoldGesture>
#include <QEvent>
#include <QGestureEvent>
#include <QTextDocumentFragment>

class EditorTextArea : public QTextEdit
{
 Q_OBJECT

 public:

  EditorTextArea(QWidget *parent=nullptr);
  ~EditorTextArea(void);

  // Константы, используемые для определения формата данных в буфере обмена
  enum MimeDataFormat{MimeDataText=1,
                      MimeDataHtml=2,
                      MimeDataImage=3};

  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);

  virtual bool canInsertFromMimeData(const QMimeData *source) const;
  virtual void insertFromMimeData(const QMimeData *source);
  int detectMimeDataFormat(const QMimeData *source);

  bool get_showformatting(void);
  void set_showformatting(bool i);

  int get_indent_started_left(void);
  int get_indent_started_right(void);

 signals:
  void tapAndHoldGestureFinished(const QPoint &);
  void updateIndentlineGeometry(void);
  void clickedOnReference(QString href);

  void downloadImages(const QString href);

  void clickOnImage();
  void doubleClickOnImage();

 public slots:
  void show_indetedge(bool i);
  void set_indentedge_pos(int i);
  void onChangeFontcolor(const QColor &selectedColor);
  void onChangeBackgroundColor(const QColor &selectedColor);
  void onChangeFontFamily(QString fontFamily);
  void onChangeFontPointSize(int n);

  void onGlobalPressKey(int key);
  void onGlobalReleaseKey(int key);

  void onDownloadImagesSuccessfull(const QString html,
                                   const QMap<QString, QByteArray> referencesAndMemoryFiles,
                                   const QMap<QString, QString> referencesAndInternalNames);

private:

  bool flagShowFormatting; // Рисовать ли символы форматирования

  bool flagShowIndentEdge; // Рисовать ли линию отступа
  int  posIndentEdge; // По какой координате рисовать линию отступа

  bool mouseCursorOverriden;

  QPoint currentMousePosition;


  bool event(QEvent *event);
  bool gestureEvent(QGestureEvent *event);
  void tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture);

  virtual bool eventFilter(QObject *o, QEvent *e);

  // Переопределение событий обработки клавиш
  // Нужны для отслеживания нажатия клавиши Ctrl чтобы при ней запускался переход по ссылке при клике на ссылку
  virtual void keyPressEvent(QKeyEvent *event);
  virtual void keyReleaseEvent(QKeyEvent *event);

  // Переопределение событий обработки мышки
  // Нужны для обработки кликов по ссылкам в тексте записи
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);

  void switchReferenceClickMode(bool flag);
  QString imageAt(const QPoint& point);
  int hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy);
  void setTextCursorFromPoint(const QPointF &point);
};

#endif // _EDITORTEXTAREA_H_
