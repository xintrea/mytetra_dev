<?php

$dirs = glob("*");
foreach ($dirs as $dir)
{
  if ($dir == "." || $dir == "..") 
    continue;

  if(is_dir($dir))
    parseDir($dir);
} 


function parseDir($dir)
{
  echo "Directory: ".$dir."\n"; 

  $files = glob("./".$dir."/*.svg");
  foreach ($files as $file)
  {
    if ($file == "." || $file == "..") 
      continue;

    echo "File: ".$file."\n"; 

    simplySvgFile($file);
  }
}


function simplySvgFile($file)
{
  $doc = new DOMDocument();
  $doc->load($file);

  // Корневой узел
  $svgElement=$dom->documentElement;

  foreach ( $svgElement->childNodes as $currentElement ) 
    if($currentElement->nodeName=='sodipodi' or $currentElement->nodeName='metadata')
    {
      echo "Find data for remove"."\n";
      $svgElement->removeChild($currentElement);
    }  

  $doc->save($file);
}

?>