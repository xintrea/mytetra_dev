<?php

  set_time_limit(0);
 
  $mytetraXml="./mytetra.xml";

  // Получение DOM-дерева корневого файла
  $dom = new DomDocument();
  if( !$dom->load($mytetraXml) ) {
    echo "Bad XML file ".$mytetraXml."\n";
    exit(1);
  }   

  // Корневой элемент XML-документа
  $root=$dom->documentElement->getElementsByTagName("content")->item(0);

  // Заполняется массив, кгде ключ - это время создания
  $result=[];
  foreach($root->getElementsByTagName("record") as $record) {
    $result[$record->getAttribute("ctime")]=['id'=>$record->getAttribute("id"),
                                             'name'=>$record->getAttribute("name")];
  }

  // Сортировка по ключу
  krsort($result);

  // Печать результата
  foreach ($result as $key => $val) {
    $date=date('d.m.Y', $key).' - '.date('H:i:s', $key);

    echo $key." Date: ".$date." ID: ".$val['id']." Name: ".$val['name']."\n";
  }
  

  
  function findRecordByDirName($mode, $element, $dirName)
  {
    static $findRecord=false;

    // Инит поиска
    if($mode==0)
    {
      $findRecord=false;
      return false;
    }

    if($findRecord==true)
      return $findRecord;

    $nodeId=$element->getAttribute("id"); // echo "Node ID:".$nodeId."\n";

    $recordtableElement=getRecordTable($element);

    // Если у узла есть таблица конечных записей, нужно перебрать записи с целью поиска имени директории
    if($recordtableElement!==false)
      foreach($recordtableElement->getElementsByTagName("record") as $record)
      {
        if($record->getAttribute("dir")==$dirName)
        {
          $findRecord=true;
          return result;
        }
      }

    // Рекурсивный вызов
    foreach($element->childNodes as $currentElement)
      if($currentElement->nodeName==="node")
        findRecordByDirName( $mode, $currentElement, $dirName );

    return $findRecord;
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


  function getUnicalId()
  {
    $result=time().getRandomString(10);

    return $result;
  }


  function getRandomString($length)
  {
    $chars = 'abdefhiknrstyzABDEFGHKNQRSTYZ23456789';
    $numChars = strlen($chars);
    $string = '';

    for ($i = 0; $i < $length; $i++) 
      $string .= substr($chars, rand(1, $numChars) - 1, 1);

    return $string;
  }

?>
