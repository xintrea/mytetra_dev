<?php

// Создание тестовой базы MyTetra для нагрузочного тестирования

$baseGenerator=new BaseGenerator();

class BaseGenerator
{
    protected $outDirName="/opt/mytetraHighLoad/data";

    protected $treeLevels=3;          // Уровень вложения генерируемого дерева
    protected $branchesQuantity=19;   // Количество веток на одном уровне вложения
    protected $notesQuantity=15;      // Количество записей в одной ветке
    protected $minSymbolsInNote=2000; // Минимальное кол-во символов в записи
    protected $maxSymbolsInNote=4000; // Максимальное кол-во символов в записи

    protected $dictFileName="./generateDictionary.txt";
    protected $dictWords=[];

    protected $branchCounter=0;
    protected $noteCounter=0;

        
    function __construct()
    {
        $this->printTotalNumberOfNotes();
        $this->createBaseDir();

        $this->generate();
        
    }


    // Создание директории с базой
    function createBaseDir()
    {
        delDir($this->outDirName);
        mkdir($this->outDirName, 0777, true); // Создание основной директории данных
        mkdir($this->outDirName."/base", 0777); // Создание директории для хранения текстов записей
    }


    // Предварительный подсчет количества генерируемых записей
    function printTotalNumberOfNotes()
    {
        echo "Total number of notes: ".( ($this->branchesQuantity ** $this->treeLevels) * $this->notesQuantity )."\n";
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
            $itemElement->setAttribute('name', 'node'.$this->branchCounter++);
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
            $noteElement->setAttribute('name', 'record'.$this->noteCounter++);
            $noteElement->setAttribute('id', getId());
            $noteElement->setAttribute('dir', getId());
            $noteElement->setAttribute('file', 'text.html');

            $recordTableElement->appendChild($noteElement);

            //Создание данных на диске для текущей записи (директория, файл с текстом записи)
            $this->createNoteData($noteElement->getAttribute('dir'), $noteElement->getAttribute('file'));
        }

        $itemElement->appendChild($recordTableElement);
    }        


    function createNoteData($dirName, $fileName)
    {
        $resultDirName=$this->outDirName."/base/".$dirName;
        mkdir($resultDirName, 0777);

        $resultFileName=$resultDirName."/text.html";
        $file=fopen($resultFileName, 'w');
        fwrite($file, "Hello MyTetra");
        fclose($file);
    }

}

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