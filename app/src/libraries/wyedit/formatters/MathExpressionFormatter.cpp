#include <QTextBlock>
#include <QTextFragment>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QTextDocumentFragment>
#include <QMessageBox>
#include <QImage>
#include <QTemporaryFile>

#include "MathExpressionFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorCursorPositionDetector.h"
#include "../EditorMultiLineInputDialog.h"

#include "main.h"
#include "libraries/DiskHelper.h"

#include "../../thirdParty/mimetex/mimetexExport.h" // Библиотека mimetex для генерации картинок формул


MathExpressionFormatter::MathExpressionFormatter()
{

}


// Содержимое формулы, которая выделена (если выделена единственная картинка формулы, требуется доработка)
QString MathExpressionFormatter::mathExpressionOnSelect(void)
{
    // Блок, в пределах которого находится курсор
    QTextBlock currentBlock = textArea->textCursor().block();
    QTextBlock::iterator it;
    QTextFragment fragment;

    // Если есть выделение
    if(textArea->textCursor().hasSelection())
    {
        // Перебиратся фрагметы блока
        for(it = currentBlock.begin(); !(it.atEnd()); ++it)
        {
            fragment = it.fragment();

            // Если фрагмент содержит изображение
            if(fragment.isValid())
                if(fragment.charFormat().isImageFormat ())
                {
                    int fragmentStart=fragment.position();
                    int fragmentEnd=fragmentStart+fragment.length();
                    int selectionStart=textArea->textCursor().selectionStart();
                    int selectionEnd=textArea->textCursor().selectionEnd();

                    // Если начало и конец фрагмента совпадает с координатами выделения
                    // Проверяется и случай, когда блок выделен в обратную сторону
                    if( (fragmentStart==selectionStart && fragmentEnd==selectionEnd) ||
                            (fragmentStart==selectionEnd && fragmentEnd==selectionStart) )
                    {
                        QTextImageFormat imageFormat=fragment.charFormat().toImageFormat();

                        return QString();
                    }
                }
        }
    }

    return QString();
}


// Сождержимое формулы (картинки формулы) на которой находится курсор
QString MathExpressionFormatter::mathExpressionOnCursor(void)
{
    // Проверка срабатывает только если нет выделения
    if(textArea->textCursor().hasSelection()==false)
    {
        QTextImageFormat imageFormat = textArea->textCursor().charFormat().toImageFormat();

        if(imageFormat.isValid())
            return QString();
    }

    return QString();
}


// Обработка клавиши добавления/редактирования математического выражения
void MathExpressionFormatter::onMathExpressionClicked(void)
{

    // Если выделена картинка, добавить проверку на то что эта картинка является формулой
    if(editor->cursorPositionDetector->isImageSelect() ||
            editor->cursorPositionDetector->isCursorOnImage())
    {
        qDebug() << "Math expression selected";

        editMathExpression();
    }
    else
    {
        // Иначе математическое выражение не выделено, и срабатывает режим добавления
        addMathExpression();
    }

}


// Вызов окна настройки свойств изображения
void MathExpressionFormatter::onContextMenuEditMathExpression()
{
    // Если выделено математическое выражение, добавить проверку на то что картинка является формулой
    if(editor->cursorPositionDetector->isImageSelect() ||
            editor->cursorPositionDetector->isCursorOnImage())
    {
        qDebug() << "Math expression selected";

        editMathExpression();
    }
}


void MathExpressionFormatter::addMathExpression(void)
{
    QString mathExpressionText=getMathExpressionFromUser();

    insertMathExpressionToTextArea(mathExpressionText);
}


void MathExpressionFormatter::editMathExpression(void)
{


}


// Запрос математического выражения от пользователя
QString MathExpressionFormatter::getMathExpressionFromUser(QString iMathExpressionText)
{
    EditorMultiLineInputDialog dialog;

    dialog.setText(iMathExpressionText);
    dialog.setWindowTitle(tr("Edit TeX math expression"));

    // Если не нажато OK
    if(dialog.exec()!=QDialog::Accepted)
        return QString();

    // Если ничего небыло изменено в TeX коде
    if(!dialog.isModified())
        return QString();

    return dialog.getText();
}


void MathExpressionFormatter::insertMathExpressionToTextArea(QString iMathExpressionText)
{
    QString tempFileName=QDir::tempPath()+"/"+getUniqueId()+".gif";

    qDebug() << "Temporary file name: " << tempFileName;
    qDebug() << "Temporary file name in C style: " << tempFileName.toUtf8().constData();

    // char *expression=(char*) iMathExpressionText.toUtf8().constData();
    // char *gifFileName=(char*) tempFileName.toUtf8().constData();

    char expression0[]="A+B=C";
    char gifFileName0[]="/tmp/1532019893791fmu4qjo.gif";

    char *expression=expression0;
    char *gifFileName=gifFileName0;

    CreateGifFromEq ( expression, gifFileName );

    // Временная картинка загружается из файла
    QImage image = QImageReader(gifFileName).read();

    // И сразу удаляется в корзину
    DiskHelper::removeFileToTrash( gifFileName );

    // Внутреннее имя картинки
    QString imageName=getUniqueImageName();

    // Картинка добавляется в хранилище документа
    textArea->document()->addResource(QTextDocument::ImageResource, QUrl(imageName), image );

    // Создается описание форматированной картинки
    // QTextImageFormat imageFormat;
    // imageFormat.setName(link.toString());

    // Картинка вставляется в текст
    QTextCursor cursor=textArea->textCursor();
    cursor.insertImage(imageName);

    // Ставится пометка что текст записи был изменен
    textArea->document()->setModified(true);
}

