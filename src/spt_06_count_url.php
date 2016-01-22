<?php

 // Подсчет количества ссылок в текстах, хранимых в базе
 
 $workDir="/opt/mytetra/data";
 $mytetraXml=$workDir."/mytetra.xml";

 $countTitleUrl=0;
 $countTextUrl=0;
 
 // XML-код преобразуется в DOM модель
 $doc = new DOMDocument();
 echo "Load XML file:".$mytetraXml."\n";
 $doc->load($mytetraXml); 
 
 $root=$doc->documentElement;
 
 $recordList=$root->getElementsByTagName("record");

 foreach($recordList as $record) 
 {
   // Зашифрованные записи не рассматриваются
   if($record->hasAttribute("crypt"))
     if($record->getAttribute("crypt")=="1")  
       continue;

   if($record->hasAttribute("url"))
     if(strlen($record->getAttribute("url"))>0)
     {
       $countTitleUrl++;
       echo "Title URL: ".$record->getAttribute("url")."\n";
     }

   if($record->hasAttribute("dir") and $record->hasAttribute("file"))
     if(strlen($record->getAttribute("dir"))>0 and strlen($record->getAttribute("file"))>0)
     {
       $fileName=$workDir."/base/".$record->getAttribute("dir")."/".$record->getAttribute("file");
       
       $fp=fopen($fileName, 'r');
       if($fp) 
       {
         // Считывание файла по строкам
         while (!feof($fp))
         {
           $text = fgets($fp, 100000);

           preg_match_all('/<a\s+?href="(.*?)"/i', $text, $allUrls, PREG_PATTERN_ORDER);
           if(isset($allUrls[1]) and count($allUrls[1])>0)
             foreach($allUrls[1] as $currentUrl)
             {
               $countTextUrl++;
               echo "Text URL: ".$currentUrl."\n";
             }
         }
       }
     else 
       echo "Ошибка при открытии файла ".$fileName."\n";
  
     }
 }
 
 echo "Title URLs: ".$countTitleUrl."\n";
 echo "Text URLs: ".$countTextUrl."\n";

?>