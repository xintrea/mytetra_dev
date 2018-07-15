<?php
  
  $inFile='mytetra_ru.ts';
  $outFile='mytetra_ru.html';

  // Создается DOM документ на основе XML файла
  $dom=new DomDocument();
  $dom->load($inFile);


  // Список элементов <context>
  $contexts=$dom->getElementsByTagName('context');

  echo '<table border="1">';

  // Перебираются контексты
  foreach($contexts as $context) 
   {
    // Выясняется имя контекста
    $names=$context->getElementsByTagName('name');
    $context_name=$names->item(0)->textContent;

    echo '<tr><td colspan=3><b>'.$context_name."</b></td></tr>\n";

    $messages=$context->getElementsByTagName('message');

    // Перебираются сообщения
    foreach($messages as $message) 
     {
      $sources=$message->getElementsByTagName('source');
      $sources_text=$sources->item(0)->textContent;

      echo "<tr>\n";
      echo '<td>'.$sources_text."</td>\n";
      echo "<td>-</td>\n";
      echo "<td>-</td>\n";
      echo "</tr>\n";
     }

   }

  echo '</table>';

?>