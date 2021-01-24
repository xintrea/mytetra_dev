<?php

  // Сервисный скрипт GetBase - выкачиватель дерева в формате MyTetra
  
  // v.0.3 - Первая рабочая версия
  //       - Выкачивает формат данных 1.1, т.е. пока не поддерживает прикрепляемые файлы и иконки

  // *********
  // Настройки
  // *********
  $mytetraUrl="http://site.com/db/mytetra/mytetra.xml"; // Стартовый URL
  $downloadDir="./data"; // Место, куда будет идти сохранение
  $version="0.3";


  // Выкачивание может быть долгим, скрипт будет выполняться столько, сколько нужно
  set_time_limit(0);

  // Вычислимые параметры
  $rootFileName=$downloadDir."/mytetra.xml";
  $recordsDir=$downloadDir."/base";

  // Информация о программе
  echo "MyTetra Get Base v.".$version."\n";
  echo "Starting URL: ".$mytetraUrl."\n";
  echo "Download directory: ".$downloadDir."\n";

  // Если директория есть, ничего не делаем чтобы не испортить уже существующие данные
  if( is_dir($downloadDir) )
  {
    echo "Download directory already exists. Try remove or rename this directory."."\n";
    exit(1);
  }

  if( !mkdir($downloadDir) )
  {
    echo "Can't create download directory"."\n";
    exit(1);
  }

  // Скачивание корневого файла
  $text=file_get_contents($mytetraUrl);
  if($text===FALSE)
  {
    echo "Can't download root file ".$mytetraUrl."\n";
    exit(1);
  }

  // Сохранение корневого файла
  $file=fopen( $rootFileName, 'wt');
  fwrite($file, $text);
  fclose($file);

  // Получение DOM-дерева корневого файла
  $dom = new DomDocument;
  if( !$dom->load($rootFileName) )
  {
    echo "Bad XML file ".$rootFileName."\n";
    exit(1);
  }   

  // Корневой элемент
  $root=$dom->documentElement->getElementsByTagName("content")->item(0);

  echo "Start download"."\n";

  downloadRecurse($root);

  echo "Download stop"."\n";

  function downloadRecurse($element)
  {
    $nodeId=$element->getAttribute("id");
    echo "Node ID:".$nodeId."\n";

    $recordtableElement=getRecordTable($element);

    // Если у узла есть таблица конечных записей, нужно выкачивать каталоги с записями
    if($recordtableElement!==false)
      foreach($recordtableElement->getElementsByTagName("record") as $record)
        downloadRecord($record);

    // Рекурсивный вызов
    foreach($element->childNodes as $currentElement)
      if($currentElement->nodeName==="node")
        if($currentElement->getAttribute("crypt")==="1")
          continue;
        else
          downloadRecurse( $currentElement );
  }


  // Функция находит для переданного узла таблицу конечных записей
  // Возвращает элемент тега <recordtable> или false
  function getRecordTable($element)
  {
    // Определяется, есть ли у узла таблица записей
    foreach($element->childNodes as $childElement)
      if($childElement->nodeName==="recordtable")
        return $childElement;
    
    return false;
  }


  function downloadRecord($record)
  {
    global $mytetraUrl, $recordsDir;
    $rootUrl=substr($mytetraUrl, 0, -12);

    // Атрибуты записи
    $recordId=$record->getAttribute("id");
    $recordDir=$record->getAttribute("dir");
    $recordFileName=$record->getAttribute("file");
    echo "Record ID:".$recordId."\n";

    // Создается директория записи
    $fullRecordDir=$recordsDir."/".$recordDir;
    mkdir($fullRecordDir, 0777, true);

    $recodFileUrl=$rootUrl."/base/".$recordDir."/".$recordFileName;
    $recodFileNameForSave=$fullRecordDir."/".$recordFileName;

    // Скачивание файла с текстом записи
    $text=file_get_contents($recodFileUrl);
    if($text===FALSE)
    {
      echo "Can't download record file ".$recodFileUrl."\n";
      return;
    }

    // Сохранение текста записи в файл
    $file=fopen( $recodFileNameForSave, 'wt');
    fwrite($file, $text);
    fclose($file);

    // Выкачиваются картинки, принадлежащие записи
    downloadImages($recordDir, $recodFileNameForSave);
  }


  function downloadImages($recordDir, $recodFileName)
  {
    global $mytetraUrl, $recordsDir;
    $rootUrl=substr($mytetraUrl, 0, -12);
    
    // Получение DOM-дерева файла записи
    $dom = new DomDocument;
    if( !(@$dom->load($recodFileName)) )
    {
      echo "Bad HTML/XML file ".$recodFileName.", images not download."."\n";
      return;
    }   

    // Элементы с картинками
    $imagesElements=$dom->documentElement->getElementsByTagName("img");

    foreach($imagesElements as $imageElement)
    {
      $imgFileName=$imageElement->getAttribute("src");

      if(substr($imgFileName,0,4)==="http")
      {
        echo "Direct link to external image: ".$imgFileName.", Image discard."."\n";
        continue; 
      }

      $imgUrl=$rootUrl."/base/".$recordDir."/".$imgFileName;

      $imgFileNameForSave=$recordsDir."/".$recordDir."/".$imgFileName;

      // Скачивание файла с картинкой
      $text=@file_get_contents($imgUrl);
      if($text===FALSE)
      {
        echo "Can't download image file ".$imgUrl."\n";
        return;
      }

      echo "Image: ".$imgFileNameForSave."\n";

      // Сохранение картинки в файл
      $file=fopen( $imgFileNameForSave, 'wb');
      fwrite($file, $text);
      fclose($file);
    }
  }

?>