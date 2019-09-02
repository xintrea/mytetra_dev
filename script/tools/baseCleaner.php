<?php

  // Скрипт очищает каталог базы данных от подкаталогов с несуществующими записями
  // Такие каталоги могут оставаться при тестировании отладочной версии программы

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
  $recordDirs=[];
  foreach($root->getElementsByTagName("record") as $record) {
    $recordDirs[$record->getAttribute("id")]=$record->getAttribute("dir");
  }

  print_r($recordDirs);

  // Заполняется массив с директориями записей на диске
  $driveDirs=[];
  $dir = opendir('./base');
  while($file = readdir($dir)) {
    echo 'Drive file/dir: '.$file;
    if (is_dir('./base/'.$file) && $file != '.' && $file != '..') {
      $driveDirs[]=$file;
      echo ' - Is dir';
    }
    echo "\n";
  }
 
  foreach($driveDirs as $driveDir) {

    $result=array_search($driveDir, $recordDirs);

    /*
    if($result!==false) {
        echo 'Correct directory: '.$driveDir.' for id: '.$result."\n";
    }
    */

    if(!in_array($driveDir, $recordDirs)) {
        dirDel('./base/'.$driveDir);
        echo "Remove dir: ".$driveDir."\n";
    }
  }




function dirDel ($dir)
{ 
	$d=opendir($dir); 
	while(($entry=readdir($d))!==false)
	{
		if ($entry != "." && $entry != "..")
		{
			if (is_dir($dir."/".$entry))
			{ 
				dirDel($dir."/".$entry); 
			}
			else
			{ 
				unlink ($dir."/".$entry); 
			}
		}
	}
	closedir($d); 
	rmdir ($dir); 
}



?>
