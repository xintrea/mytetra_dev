#include <QTextBlock>
#include <QTextFragment>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QTextDocumentFragment>
#include <QMessageBox>

#include "ImageFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorImageProperties.h"
#include "../EditorCursorPositionDetector.h"

#include "../../Downloader.h"


ImageFormatter::ImageFormatter()
{

}


// Формат картинки, которая выделена (если выделена единственная картинка)
QTextImageFormat ImageFormatter::imageFormatOnSelect(void)
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

            return imageFormat;
          }
        }
    }
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


void ImageFormatter::editImageProperties(void)
{
  // Данные обрабатываемой картинки
  QTextImageFormat imageFormat;

  // Если выбрано изображение
  if(editor->cursorPositionDetector->isImageSelect())
    imageFormat=imageFormatOnSelect();

  // Если изображение не выбрано, но курсор находится в позиции изображения
  if(editor->cursorPositionDetector->isCursorOnImage())
    imageFormat=imageFormatOnCursor();


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

        // Если фрагмент содержит изображение
        if(fragment.isValid() &&
           fragment.charFormat().isImageFormat())
          break; // Переменная fragment содержит только картинку
      }


      QTextCursor helper = textArea->textCursor();

      helper.setPosition(fragment.position());
      helper.setPosition(fragment.position() + fragment.length(),
                         QTextCursor::KeepAnchor);
      helper.setCharFormat(imageFormat);
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
    imageSelectDialog.exec();

    // Выясняется список выбранных файлов
    QStringList files=imageSelectDialog.selectedFiles();

    // Если ни один файл не выбран
    if(files.size()==0)
      return;

    // Перебираются файлы выбранных картинок
    for(int i=0; i<files.size(); ++i)
    {
      // Текущее имя файла
      QString currFileName=files.at(i);

      // Картинка загружается из файла
      QImage image = QImageReader(currFileName).read();

      // Внутреннее имя картинки
      QString imageName="image"+QString::number(rand())+".png";

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


// Вызов окна настройки свойств изображения
void ImageFormatter::onContextMenuEditImageProperties()
{
  // Если выделена картинка
  if(editor->cursorPositionDetector->isImageSelect() ||
     editor->cursorPositionDetector->isCursorOnImage())
  {
    qDebug() << "Image selected";

    editImageProperties();
  }
}


void ImageFormatter::onDownloadImages(const QString html)
{
  // Создается временный документ на основе HTML (именно документ, так как у QTextDocumentFragment нет методов перебора блоков текста)
  QTextDocument textDocument;
  QTextCursor textCursor(&textDocument);
  textCursor.insertHtml(html);

  // Перебираются блоки документа и находятся блоки с картинками
  QStringList imagesNames; // В список сохраняются имена найденных картинок
  QStringList downloadReferences; // Список ссылок на изображения, которые надо загрузить
  QTextBlock textBlock = textDocument.begin();
  while(textBlock.isValid())
  {
    QTextBlock::iterator it;

    for(it = textBlock.begin(); !(it.atEnd()); ++it)
    {
      QTextFragment currentFragment = it.fragment();
      if(currentFragment.isValid())
      {
        if(currentFragment.charFormat().isImageFormat()) // Если найден блок с картинкой
        {
          // Выясняется формат картинки
          QTextImageFormat imgFmt = currentFragment.charFormat().toImageFormat();

          // Из формата выясняется имя картинки
          QString imageName=imgFmt.name();
          imagesNames << imageName;
          qDebug() << "Find  " << imageName << "\n"; // имя файла

          // Если файла картинки не существует
          QString imageFileName=editor->getWorkDirectory()+"/"+imageName;
          QFileInfo tryFile(imageFileName);
          if(tryFile.exists()==false)
          {
            qDebug() << "Set file for download" << imageName;
            downloadReferences << imageName;
          }

        }
      }
    }
    textBlock = textBlock.next();
  }


  // Если есть изображения, которые надо скачать
  if(downloadReferences.count()>0)
  {
    // Скачивание изображений
    Downloader downloader;
    downloader.setSaveMode(Downloader::memory);
    downloader.setReferencesList(downloadReferences);
    downloader.run();

    if(!downloader.isSuccess())
    {
      QMessageBox msgBox;
      msgBox.setText(tr("Error at download process. Maybe not all images is dowload."));
      msgBox.exec();
    }
  }

  // Временная заглушка
  QTextDocumentFragment textFragment;
  emit downloadImagesSuccessfull(textFragment);
}

