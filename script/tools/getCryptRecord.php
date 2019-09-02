<?php

  // Получение списка зашифрованных записей и информации о них

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

  // Заполняется массив [id_записи]=>имя_директории
  $records=[];
  foreach($root->getElementsByTagName("record") as $record) {
    if($record->hasAttribute("crypt") and $record->getAttribute("crypt")==='1') {
      $records[$record->getAttribute("id")]=$record->getAttribute("dir");
    }  
  }

  print_r($records);

  foreach($records as $id=>$dir) {
    $fileName='./base/'.$dir.'/text.html';

    // $resultCode=0;
    // $result=trim(system('hexdump -C '.$fileName.' | head -c 33', $resultCode));
    // echo 'id: '.$id.' dir: '.$dir.' signature: '.$result."\n";

    $result=[];
    exec('hexdump -C '.$fileName.' | head -c 33', $result);
    echo 'id: '.$id.' dir: '.$dir.' signature: '.$result[0]."\n";
    
    
  }

?>
