#ifndef EDITORMATHEXPRESSIONDIALOG_H
#define EDITORMATHEXPRESSIONDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTimer>
#include <QSplitter>
#include <QTextEdit>
#include <QScrollArea>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QKeyEvent>

#include "formatters/MathExpressionFormatter.h"

/******************************************************************
 *                      Диалог написания Tex формулы              *
 * ****************************************************************/

// =================================================================
// Вспомогательный класс-наследник QTextEdit для отлова undo / redo
// =================================================================
class TexTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TexTextEdit(QTextEdit *parent = nullptr) : QTextEdit(parent) {}
    virtual ~TexTextEdit() {}

protected:
    void keyPressEvent(QKeyEvent *e) {
        if (e->modifiers()==Qt::ControlModifier) {
            if (e->key()==Qt::Key_Z)
                emit isUndo();
            if (e->key()==Qt::Key_Y)
                emit isRedo();
        }
        QTextEdit::keyPressEvent(e);
    }

signals:
    void isUndo(); // Сообщение о том, что произошло событие undo
    void isRedo(); // Сообщение о том, что произошло событие redo
};

// =============================================
// Основной класс диалога написания Tex формулы
// =============================================
class EditorMathExpressionDialog : public QDialog
{
    Q_OBJECT

public:

    explicit EditorMathExpressionDialog(MathExpressionFormatter *mathExpressionFormatter, QWidget *parent = nullptr);
    virtual ~EditorMathExpressionDialog();

    // Вставка текста Tex формулы в редактор формулы
    void setMathExpressionText(QString text);

    // Получение текста Tex формулы из редактора формулы
    QString getMathExpressionText();

    // Был ли изменен текст формулы, показанный в диалоге
    bool isModified();

    void setWordWrapMode(QTextOption::WrapMode mode);

protected slots:

    // Обработка переключения режима масштабирования картинки формулы
    void onSwitchFitToScrollArea();

    // Увеличение масштаба области текста формулы
    void onTextZoomIn();

    // Уменьшение масштаба области текста формулы
    void onTextZoomOut();

    // Включение таймера обновления картинки формулы
    void onStartTimer();

    // Отключение таймера обновления картинки формулы
    void onStopTimer();

    // Обновление картинки формулы по таймеру
    void onTimerAlarm();

    // Установка флага изменения текста формулы при наборе символов
    void onTextChanged();

    // Перерисовка картинки формулы при действии undo
    void onUndo();

    // Перерисовка картинки формулы при действии redo
    void onRedo();

protected:

    // Первая отрисовка картинки формулы
    void showEvent(QShowEvent *event);

    // Масштабирование картинки формулы в зависимости от изменения размеров диалога
    void resizeEvent(QResizeEvent *event);

    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

private:

    // Обновление картинки формулы
    void updateFormulaPicture();

    // Масштабирование картинки формулы
    void pictureZoom();

    // Разделитель областей отображения картинки формулы и написания текста формулы
    QSplitter *mathSplitter;

    // Область работы с картинкой формулы
    QWidget *topWidget; // Виджет, объединяющий все контролы по работе с картинкой формулы (для сплитера)
    QLabel *pictureFormalaLabel;                // Надпись Picture formula
    QHBoxLayout *pictureFormulaControlLayout;   // Область контролов по работе с масштабированием картинки
    QRadioButton *timerRadioButton;             // Обновление картинки формулы по таймеру
    QRadioButton *realTimeRadioButton;          // Обновление картинки формулы в реальном времени
    QCheckBox *fitToScrollAreaCheckBox;         // Масштабирование картинки до размеров ScrollArea
    QVBoxLayout *pictureFormulaLayout;          // Компановщик всех контролов по работе с картинкой формулы

    // Область работы с текстом формулы
    QWidget *bottomWidget;  // Виджет, объединяющий все контролы по работе с текстом формулы (для сплитера)
    QLabel *textFormalaLabel;                   // Надпись Formula text
    QToolButton *textFormulaZoomInPushButton;   // Увеличение масштаба области текста
    QToolButton *textFormulaZoomOutPushButton;  // Уменьшение масштаба области текста
    QHBoxLayout *textFormulaZoomLayout;         // Компановщик контролов работы с масштабом текста формулы
    QVBoxLayout *textFormulaLayout; // Компановщик всех контролов по работе с текстом формулы
    QLabel *imageLabel;             // Метка для отображения картинки формулы
    QScrollArea *imageScrollArea;   // Область прокрутки метки
    TexTextEdit *textArea;          // Текстовая область написания формулы

    bool formulaModifiedTimePeriod; // Признак, было ли изменение формулы во время простоя таймера

    // Кнопки Ok и Cancel
    QDialogButtonBox *dialogButtonBox;

    // Работа с таймером
    QTimer *timer;                  // Таймер для отрисовки картинки формулы
    int mathExpressionUpdateTime;   // Период обновления картинки по таймеру (сек.)

    MathExpressionFormatter *mathExpressionFormatter; // Класс для работы с математическими выражениями
};

#endif // EDITORMATHEXPRESSIONDIALOG_H
