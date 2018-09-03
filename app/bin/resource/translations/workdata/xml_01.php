<?php
  
  $inFile='mytetra_ru.ts';
  $outFile='mytetra_ru.html';

  // Создается DOM документ на основе XML файла
  $dom=new DomDocument();
  $dom->load($inFile);


  // Список элементов <context>
  $contexts=$dom->getElementsByTagName('context');

  // Перебираются контексты
  foreach($contexts as $context) 
   {
    // Выясняется имя контекста
    $name_elements=$context->getElementsByTagName('name');
    $context_name=$name_elements->item(0)->textContent;

    echo $context_name."\n";
   }

?>