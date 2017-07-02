<?php

// Создание тестовой базы MyTetra для нагрузочного тестирования

$baseGenerator=new BaseGenerator();

class BaseGenerator
{
    protected $outDirName="/opt/mytetraHighLoad/data";  // Директория, куда будут сложены сгенерированные данные
    protected $dictFileName="./generateDictionary.txt"; // Словарь используемых слов в текстах
    protected $treeLevels=3;          // Уровень вложения генерируемого дерева
    protected $branchesQuantity=19;   // Количество веток на одном уровне вложения
    protected $notesQuantity=15;      // Количество записей в одной ветке
    protected $minSymbolsInTitle=25;  // Примерное минимальное кол-во символов в названии записи
    protected $maxSymbolsInTitle=100; // Примерное максимальное кол-во символов в названии записи
    protected $minSymbolsInTags=10;   // Примерное минимальное кол-во символов в текстовых метках
    protected $maxSymbolsInTags=100;  // Примерное максимальное кол-во символов в текстовых метках
    protected $minSymbolsInNote=2000; // Примерное минимальное кол-во символов в тексте записи
    protected $maxSymbolsInNote=4000; // Примерное максимальное кол-во символов в тексте записи
    protected $minSymbolsInItem=5;    // Примерное минимальное кол-во символов в названии ветки
    protected $maxSymbolsInItem=20;   // Примерное максимальное кол-во символов в названии ветки
        

    protected $dictWords=[];
    protected $branchCounter=0;
    protected $noteCounter=0;

        
    function __construct()
    {
        $this->printTotalNumberOfNotes();

        $this->createBaseDir();

        $this->loadDict();

        $this->generate();
        
    }


    // Создание директории с базой
    function createBaseDir()
    {
        delDir($this->outDirName); // Удаление предыдущей директории
        mkdir($this->outDirName, 0777, true); // Создание основной директории данных
        mkdir($this->outDirName."/base", 0777); // Создание директории для хранения текстов записей
    }


    function loadDict()
    {
        $this->dictWords=file($this->dictFileName);
    }
    

    // Предварительный подсчет количества генерируемых записей
    function printTotalNumberOfNotes()
    {
        echo "Total number of notes: ".$this->calculateTotalNumberOfNotes()."\n";
    }


    function calculateTotalNumberOfNotes()
    {
        return ($this->branchesQuantity ** $this->treeLevels) * $this->notesQuantity;
    }


    function generate()
    {
        $doc=new DOMDocument('1.0', 'UTF-8');

        $dtd=(new DOMImplementation)->createDocumentType('mytetradoc');
        $doc->appendChild($dtd);
                
        // Создание корневого элемента
        $rootElement=$doc->createElement('root');
        $doc->appendChild($rootElement);

        // Создание элемента format
        $formatElement=$doc->createElement('format');
        $formatElement->setAttribute('version', 1);
        $formatElement->setAttribute('subversion', 2);
        $rootElement->appendChild($formatElement);

        // Создание элемента content
        $contentElement=$doc->createElement('content');
        $this->generateTree(1, $doc, $contentElement);
        $rootElement->appendChild($contentElement);                

        // Запись XML дерева на диск
        $doc->preserveWhiteSpace = false;
        $doc->formatOutput = true;
        $doc->save($this->outDirName."/mytetra.xml");
    }


    function generateTree($level, &$doc, &$currentElement)
    {
        for($i=0; $i<$this->branchesQuantity; $i++) {
            $itemElement=$doc->createElement('node');
            $itemElement->setAttribute('name', $this->generateItemTitle( 'node'.$this->branchCounter++ ));
            $itemElement->setAttribute('id', getId());

            // Добавление к ветке элементов записей
            $this->generateNotes($doc, $itemElement);

            $currentElement->appendChild($itemElement);

            if($level < $this->treeLevels) {
                $this->generateTree($level+1, $doc, $itemElement);
            }
        }
    }


    function generateNotes(&$doc, &$itemElement)
    {
        $recordTableElement=$doc->createElement('recordtable');
    
        for($i=0; $i<$this->notesQuantity; $i++) {
            $noteElement=$doc->createElement('record');
            $noteElement->setAttribute('name', $this->generateNoteTitle( 'record'.$this->noteCounter++ ));
            $noteElement->setAttribute('tags', $this->generateNoteTags());
            $noteElement->setAttribute('id', getId());
            $noteElement->setAttribute('dir', getId());
            $noteElement->setAttribute('file', 'text.html');

            $recordTableElement->appendChild($noteElement);

            //Создание данных на диске для текущей записи (директория, файл с текстом записи)
            $this->createNoteData($noteElement->getAttribute('dir'), $noteElement->getAttribute('file'));
        }

        $itemElement->appendChild($recordTableElement);

        echo 'Generate note '.$this->noteCounter.' from ~'.$this->calculateTotalNumberOfNotes()."\n";
    }        


    function createNoteData($dirName, $fileName)
    {
        $resultDirName=$this->outDirName."/base/".$dirName;
        mkdir($resultDirName, 0777);

        $resultFileName=$resultDirName."/text.html";
        $file=fopen($resultFileName, 'w');
        fwrite($file, $this->generateNoteText());
        fclose($file);
    }


    function generateItemTitle($prefixText)
    {
        return $prefixText.' '.$this->generateSmartText($this->minSymbolsInItem, $this->maxSymbolsInItem, ' ');
    }


    function generateNoteTitle($prefixText)
    {
        return $prefixText.' '.$this->generateSmartText($this->minSymbolsInTitle, $this->maxSymbolsInTitle, ' ');
    }
    

    function generateNoteTags()
    {
        return $this->generateSmartText($this->minSymbolsInTags, $this->maxSymbolsInTags, ', ');
    }


    function generateNoteText()
    {
        $text=$this->generateSmartText($this->minSymbolsInNote, $this->maxSymbolsInNote, ' ');

        $header=<<<'EOTHEADER'
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
<html><head>
<meta name="qrichtext" content="1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<style type="text/css">p, li { white-space: pre-wrap; }</style>
</head><body style=" font-family:'DejaVu Sans'; font-size:11pt; font-weight:400; font-style:normal;">
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
EOTHEADER;

        $footer=<<<'EOTFOOTER'
</p></body></html>
EOTFOOTER;

        return $header.$text.$footer;
    }


    function generateSmartText($minLen, $maxLen, $delimeter)
    {
        $targetTextLen=rand($minLen, $maxLen);
        $text='';
        
        while(mb_strlen($text)<$targetTextLen) {
            $randomWord=trim( $this->dictWords[ rand(0, ( count($this->dictWords))-1 ) ] );

            if($text==='') {
                $text.=$randomWord;
            }    
            else {
                $text.=$delimeter.$randomWord;
           }     
        }

        return trim($text);
    }

} // Закончился класс BaseGenerator


function delDir($dir) 
{
    if(!is_dir($dir))
        return;

    $files = array_diff(scandir($dir), array('.','..'));
    foreach ($files as $file) {
        (is_dir("$dir/$file")) ? delDir("$dir/$file") : unlink("$dir/$file");
    }

    rmdir($dir);

    return;
}     


function getId() 
{
    $result = '';
    $array = array_merge(range('a','z'), range('0','9'));

    for($i=0; $i<10; $i++){
  	    $result .= $array[mt_rand(0, 35)];
    }

    $result=((string) time()).$result;
    
    return $result;
}

?>