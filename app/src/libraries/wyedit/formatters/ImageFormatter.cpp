#include <QTextBlock>
#include <QTextFragment>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QTextDocumentFragment>
#include <QMessageBox>
#include <QImage>

#include "ImageFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorImageProperties.h"
#include "../EditorCursorPositionDetector.h"

#include "main.h"
#include "libraries//Downloader.h"
#include "libraries/helpers/UniqueIdHelper.h"
#include "libraries/helpers/MessageHelper.h"


ImageFormatter::ImageFormatter()
{

}


// Формат картинки, которая выделена (если выделена единственная картинка)
QTextImageFormat ImageFormatter::imageFormatOnSelect(void)
{
    // Если выбрано изображение
    if(editor->cursorPositionDetector->isImageSelect()) {
        return editor->cursorPositionDetector->getImageSelectFormat();
    }

    return QTextImageFormat();
}


// Формат картинки на которой находится курсор
QTextImageFormat ImageFormatter::imageFormatOnCursor(void)
{
  // Проверка срабатывает только если нет выделения
  if(textArea->textCursor().hasSelection()==false)
  {
    QTextImageFormat imageFormat = textArea->textCursor().charFormat().toImageFormat();

    if(imageFormat.isValid())
      return imageFormat;
  }

  return QTextImageFormat();
}


void ImageFormatter::openImage()
{
  // Данные обрабатываемой картинки
  QTextImageFormat imageFormat;

  // Если выбрано изображение
  if(editor->cursorPositionDetector->isImageSelect()) {
    imageFormat=imageFormatOnSelect();
  }
  else if(editor->cursorPositionDetector->isCursorOnImage()) {
    // Если изображение не выбрано, но курсор находится в позиции изображения
    imageFormat=imageFormatOnCursor();
  }

  // Имя картинки в ресурсах документа
  QString imageName=imageFormat.name();

  // Выясняется путь к файлу
  QString fullDir = editor->getWorkDirectory();
  QString fullFileName = fullDir+"/"+imageName;

  qDebug() << "Open image file: "+fullFileName;

  // Открытие файла средствами операционной системы
  QUrl urlFileName("file:"+fullFileName);
  QDesktopServices::openUrl(urlFileName);
}


void ImageFormatter::editImageProperties(void)
{
    // Для картинки с формулой свойства изображения редактироваться не должны
    if(editor->cursorPositionDetector->isMathExpressionSelect() ||
            editor->cursorPositionDetector->isCursorOnMathExpression()) {
        return;
    }

    // Данные обрабатываемой картинки
    QTextImageFormat imageFormat;

    // Если выбрано изображение
    if(editor->cursorPositionDetector->isImageSelect()) {
        imageFormat=imageFormatOnSelect();
    }
    else if(editor->cursorPositionDetector->isCursorOnImage()) {
        // Если изображение не выбрано, но курсор находится в позиции изображения
        imageFormat=imageFormatOnCursor();
    }

    // Выясняется имя картинки в ресурсах документа
    QString imageName=imageFormat.name();

    // По имени из ресурсов вытягивается кратинка
    QUrl urlName(imageName);
    QVariant imageData=textArea->document()->resource(QTextDocument::ImageResource, urlName);
    QImage image=imageData.value<QImage>();

    // Выяснятся реальные размеры картики
    int realImageWidth=image.width();
    int realImageHeight=image.height();

    qDebug() << "Real image width " << realImageWidth << " height " << realImageHeight;
    qDebug() << "Format image width " << imageFormat.width() << " height " << imageFormat.height();

    // Создается и запускается диалог запроса размеров картинки
    EditorImageProperties dialog;
    dialog.set_info(tr("Real image size ") +
                    QString::number(realImageWidth) +
                    " x " +
                    QString::number(realImageHeight) +
                    tr(" pixels"));
    dialog.set_real_width(realImageWidth);
    dialog.set_real_height(realImageHeight);

    // Если в форматировании картинки не задан размер картинки
    if(imageFormat.width()==0 && imageFormat.height()==0)
    {
        // В окне настройки стартовый размер задается как размер картинки
        dialog.set_width(realImageWidth);
        dialog.set_height(realImageHeight);
    }
    else
    {
        // В окне настройки стартовый размер задается согласно форматированию
        dialog.set_width(imageFormat.width());
        dialog.set_height(imageFormat.height());
    }

    dialog.update_percent();


    // Запуск диалога на выполнение
    if(dialog.exec()!=QDialog::Accepted)
        return;

    imageFormat.setWidth(dialog.get_width());
    imageFormat.setHeight(dialog.get_height());

    // Если в новом формате картинки нет никаких ошибок
    if(imageFormat.isValid())
    {

        // Если выбрано изображение
        if(editor->cursorPositionDetector->isImageSelect())
        {
            QTextFragment fragment;

            // Блок, в пределах которого находится курсор
            QTextBlock currentBlock = textArea->textCursor().block();
            QTextBlock::iterator it;

            // Перебиратся фрагметы блока
            // Так как известно, что картинка выделена, поиск фрагмента будет успешным
            for(it = currentBlock.begin(); !(it.atEnd()); ++it)
            {
                fragment = it.fragment();

                // Если фрагмент содержит изображение, для которого редактировались свойства
                if(fragment.isValid() &&
                        fragment.charFormat().isImageFormat() &&
                        fragment.charFormat().toImageFormat().name()==imageName) {

                    int fragmentStart=fragment.position();
                    int fragmentEnd=fragmentStart+fragment.length();
                    int selectionStart=textArea->textCursor().selectionStart();
                    int selectionEnd=textArea->textCursor().selectionEnd();

                    // Если начало и конец фрагмента совпадает с координатами выделения
                    // Проверяется и случай, когда выделение было в обратную сторону
                    if( (fragmentStart==selectionStart && fragmentEnd==selectionEnd) ||
                            (fragmentStart==selectionEnd && fragmentEnd==selectionStart) ) {
                        break; // Переменная fragment содержит только картинку, для которой редактируются свойства
                    }
                }
            }

            QTextCursor helpCursor = textArea->textCursor();

            helpCursor.setPosition(fragment.position());
            helpCursor.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
            helpCursor.setCharFormat(imageFormat);
        }


        // Если изображение не выбрано, но курсор находится в позиции изображения
        if(editor->cursorPositionDetector->isCursorOnImage())
        {
            int cursorPosition=textArea->textCursor().position();

            QTextCursor helper=textArea->textCursor();

            helper.setPosition(cursorPosition);

            if(textArea->textCursor().atBlockStart())
                helper.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            else
                helper.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

            helper.setCharFormat(imageFormat);
        }
    }

}


// Обработка клавиши добавления картинки
void ImageFormatter::onInsertImageFromFileClicked(void)
{

  // Если выделена картинка
  if(editor->cursorPositionDetector->isImageSelect() ||
     editor->cursorPositionDetector->isCursorOnImage())
  {
    qDebug() << "Image selected";

    editImageProperties();
  }
  else
  {
    // Иначе картинка не выделена, и срабатывает режим добавления
    // картинки из файла

    // Диалог выбора файлов катинок
    QFileDialog imageSelectDialog(editor);
    imageSelectDialog.setFileMode(QFileDialog::ExistingFiles); // QFileDialog::ExistingFile
    imageSelectDialog.setNameFilter("*.png *.jpg *.gif");
    imageSelectDialog.setWindowTitle(tr("Insert image"));
    imageSelectDialog.setDirectory(QDir::homePath());
    int result=imageSelectDialog.exec();

    // Если не было нажато "Ок"
    if(result!=QDialog::Accepted)
    {
        return;
    }

    // Выясняется список выбранных файлов
    QStringList files=imageSelectDialog.selectedFiles();

    // Если ни один файл не выбран
    if(files.size()==0)
    {
        return;
    }

    // Перебираются файлы выбранных картинок
    for(int i=0; i<files.size(); ++i)
    {
      // Текущее имя файла
      QString currFileName=files.at(i);

      // Картинка загружается из файла
      QImage image = QImageReader(currFileName).read();

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
    } // Закончился цикл перебора файлов картинок
  } // Завершилось условие что картинка не выбрана и нужно добавлять из файла

}


// Вызов открытия изображения
void ImageFormatter::onContextMenuOpenImage()
{
  // Для картинки с формулой свойства изображения редактироваться не должны
  if(editor->cursorPositionDetector->isMathExpressionSelect() ||
      editor->cursorPositionDetector->isCursorOnMathExpression()) {
    return;
  }

  // Если выделена картинка
  if(editor->cursorPositionDetector->isImageSelect() ||
      editor->cursorPositionDetector->isCursorOnImage())
  {
    qDebug() << "Open image selected";

    openImage();
  }
}


// Вызов окна настройки свойств изображения
void ImageFormatter::onContextMenuEditImageProperties()
{
  // Для картинки с формулой свойства изображения редактироваться не должны
  if(editor->cursorPositionDetector->isMathExpressionSelect() ||
     editor->cursorPositionDetector->isCursorOnMathExpression()) {
    return;
  }

  // Если выделена картинка
  if(editor->cursorPositionDetector->isImageSelect() ||
     editor->cursorPositionDetector->isCursorOnImage())
  {
    qDebug() << "Edit image selected";

    editImageProperties();
  }
}


void ImageFormatter::onClickOnImage(void)
{
    openImage();
}


void ImageFormatter::onDoubleClickOnImage(void)
{
    onContextMenuEditImageProperties();
}


void ImageFormatter::onDownloadImages(const QString html)
{
  // qDebug() << "HTML for download images: " << html;

  // Создается временный документ на основе HTML (именно документ, так как у QTextDocumentFragment нет методов перебора блоков текста)
  QTextDocument textDocument;
  QTextCursor textCursor(&textDocument);
  textCursor.insertHtml(html);

  QStringList downloadReferences; // Список ссылок на изображения, которые надо загрузить

  QMap<QString, QString> referencesAndInternalNames; // Соответствие ссылок на изображения и внутренних имен изображений

  QMessageBox msgBox;

  QTextBlock textBlock = textDocument.begin();
  while(textBlock.isValid())
  {
    bool resetBlock=false;

    QTextBlock::iterator it;

    for(it = textBlock.begin(); !(it.atEnd()); ++it)
    {
      QTextFragment currentFragment = it.fragment();
      if(currentFragment.isValid())
      {
        if(currentFragment.charFormat().isImageFormat()) // Если найден блок с картинкой
        {
          // qDebug() << "Fragment text: " << currentFragment.text();

          // Выясняется формат картинки
          QTextImageFormat imgFmt = currentFragment.charFormat().toImageFormat();

          // Из формата выясняется имя картинки
          QString imageName=imgFmt.name();
          qDebug() << "Find " << imageName << "\n"; // имя файла

          // Если имя файла не является "внутренним", значит картинка еще не добавлена
          if(!imageName.contains(QRegExp("^image\\d{10}[a-z0-9]+.png$")))
          {
            if(msgBox.text().length()==0)
            {
              msgBox.setText(tr("Images download initiating..."));
              msgBox.setStandardButtons(QMessageBox::NoButton);
              msgBox.setWindowModality(Qt::WindowModal);
              msgBox.show();
              qApp->processEvents();
            }

            qDebug() << "Set file for download" << imageName;

            QString internalImageName;

            // Если ссылки на картинку еще нет в списке загружаемых
            if(!downloadReferences.contains(imageName))
            {
              // Ссылка на картинку добавляется в массив скачиваемых файлов
              downloadReferences << imageName;

              // Ссылке ставится в соответствие уникальное внутреннее имя картинки
              internalImageName=getUniqueImageName();
              referencesAndInternalNames[imageName]=internalImageName;
            }
            else
            {
              // Иначе картинка повторяется

              // Внутреннее имя картинки берется из уже существующих
              internalImageName=referencesAndInternalNames[imageName];
            }

            // Символ внешней картинки заменяется на символ внутренней
            unsigned int position=currentFragment.position();
            textCursor.setPosition(position);
            textCursor.deleteChar(); // Удаляется символ - внешняя картинка (попробовать deletePreviousChar)
            textCursor.insertImage(internalImageName); // Вставляется символ - внутренняя картинка

            // Выход из цикла перебора фрагмента, т. к. документ изменился
            resetBlock=true;
            break;
          }
        }
      }
    }

    if(resetBlock)
      textBlock = textDocument.begin(); // Документ изменился, его надо обрабатывать с самого начала
    else
      textBlock = textBlock.next();
  }


  // Если есть изображения, которые надо скачать
  if(downloadReferences.count()>0)
  {
    msgBox.hide();

    // Виджет скачивания файлов
    Downloader downloader;
    downloader.setAboutText(tr("Download images"));
    downloader.setDownloadMode(Downloader::memory);
    downloader.setReferencesList(downloadReferences);

    // Установка ширины для виджета скачивания файлов
    int dialogWidth=int(0.8*(float)textArea->width());
    downloader.setMinimumWidth( dialogWidth );
    downloader.resize( downloader.size() );

    // На передний план
    downloader.raise();

    // Запуск виджета скачивания файлов
    downloader.run();

    // Если при скачивании изображений появились какие-то ошибки
    if(!downloader.isSuccess())
    {
      QMessageBox msgBox;
      msgBox.setText(tr("Probably error in a process of images download."));

      if(downloader.getErrorLog().length()>0)
        msgBox.setDetailedText(downloader.getErrorLog());

      msgBox.exec();
    }

    emit downloadImagesSuccessfull( textDocument.toHtml(), downloader.getReferencesAndMemoryFiles(), referencesAndInternalNames );
    return;
  }

  emit downloadImagesSuccessfull( html, QMap<QString, QByteArray>(), QMap<QString, QString>() );
}

