<?php

// Создание тестовой базы MyTetra для нагрузочного тестирования

$baseGenerator=new BaseGenerator();

class BaseGenerator
{

    public $treeLevels=3;          // Уровень вложения генерируемого дерева
    public $branchesQuantity=19;   // Количество веток на одном уровне вложения
    public $notesQuantity=15;      // Количество записей в одной ветке
    public $minSymbolsInNote=2000; // Минимальное кол-во символов в записи
    public $maxSymbolsInNote=4000; // Максимальное кол-во символов в записи

    protected $dictFileName="./generateDictionary.txt";
    protected $dictWords=[];
        
    protected $outDirName="./data";

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
        mkdir($this->outDirName, 0777);
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
                
        $rootElement=$doc->createElement('root');
        $doc->appendChild($rootElement);

        $formatElement=$doc->createElement('format');
        $formatElement->setAttribute('version', 1);
        $formatElement->setAttribute('subversion', 2);
        $rootElement->appendChild($formatElement);

        $contentElement=$doc->createElement('content');
        $rootElement->appendChild($contentElement);                

        $doc->preserveWhiteSpace = false;
        $doc->formatOutput = true;
        $doc->save($this->outDirName."/mytetra.xml");
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


?>