<?php

 $files=scandir(".");
 
 foreach($files as $k => $v)
  if(strcmp($v,'.')!=0 && strcmp($v,'..')!=0)
   if(!(strstr($v,'.cpp')==false && strstr($v,'.h')==false))
    {
     $count=countsym($v,'"');
     $mod=$count % 2;
     if($mod==1) print 'Warning! '.$v.' symbol " count '.$count."\n";


     $c1=countsym($v,'(');
     $c2=countsym($v,')');
     if($c1!=$c2) print 'Warning! '.$v.' symbol () not close! '.$c1.' '.$c2."\n";
     
     $c1=countsym($v,'{');
     $c2=countsym($v,'}');
     if($c1!=$c2) print 'Warning! '.$v.' symbol {} not close! '.$c1.' '.$c2."\n";

     $c1=countsym($v,'#if');
     $c2=countsym($v,'#endif');
     if($c1!=$c2) print 'Warning! '.$v.' symbol #if #endif not close! '.$c1.' '.$c2."\n";
    }


function countsym($file, $sym)
{

 $count=0;

 $afile=file($file);
 $cfile=count($afile);
 for($n=0;$n<=$cfile-1;$n++) 
  {
   $count=$count+substr_count($afile[$n],$sym);
  } 

 return $count;          

}

function no1310($s)
{
 $s=str_replace(chr(13),'',$s);
 $s=str_replace(chr(10),'',$s);
 return $s;
}

?>