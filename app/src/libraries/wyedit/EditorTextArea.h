#ifndef _EDITORTEXTAREA_H_
#define _EDITORTEXTAREA_H_

#include <QTextEdit>


class QPaintEvent;
class QMimeData;
class QTapAndHoldGesture;
class QEvent;
class QGestureEvent;


class EditorTextArea : public QTextEdit
{
    Q_OBJECT

public:

    EditorTextArea(QWidget *parent=nullptr);
    ~EditorTextArea(void);

    //! Константы, используемые для определения формата данных в буфере обмена
    enum MimeDataFormat{ MimeDataText=1,
                         MimeDataHtml=2,
                         MimeDataImage=3 };

    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

    virtual bool canInsertFromMimeData(const QMimeData *source) const;
    virtual void insertFromMimeData(const QMimeData *source);
    MimeDataFormat detectMimeDataFormat(const QMimeData *source);

    //! Получение флага, указывающего рисовать ли символы форматирования
    bool getShowFormatting(void);

    //! Установка флага, указывающего рисовать ли символы форматирования
    void setShowFormatting(bool i);

    int getIndentStartedLeft(void);
    int getIndentStartedRight(void);

    void softRedraw();

signals:
    void tapAndHoldGestureFinished(const QPoint &);
    void updateIndentlineGeometry(void);
    void clickedOnReference(QString href);

    void downloadImages(const QString href);

    void doubleClickOnImage();

public slots:
    void showIndentEdge(bool i);
    void setIndentEdgePos(int i);

    void onGlobalPressKey(int key);
    void onGlobalReleaseKey(int key);

    void onDownloadImagesSuccessfull(const QString html,
                                     const QMap<QString, QByteArray> referencesAndMemoryFiles,
                                     const QMap<QString, QString> referencesAndInternalNames);

private:

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


    bool m_flagShowFormatting; //< Рисовать ли символы форматирования

    bool m_flagShowIndentEdge; //< Рисовать ли линию отступа
    int  m_posIndentEdge; //< По какой координате рисовать линию отступа

    bool m_mouseCursorOverriden;

    QPoint m_currentMousePosition;
};

#endif // _EDITORTEXTAREA_H_
