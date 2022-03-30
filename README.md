MyTetra - smart manager for information collecting
==================================================

MyTetra is open source and cross platform personal manager for information accumulation. 
It is powerful program for data memorization and structuring notes.


Repository mytetra_dev
======================

ENG: Central source repository for MyTetra development.<br/> 
* [ReadMe (eng)](https://github.com/xintrea/mytetra_dev/blob/experimental/README_ENG.md)
* [MyTetra official english page](https://webhamster.ru/site/page/index/articles/projectcode/138)
* [MyTetra Share (eng)](https://webhamster.ru/site/page/index/articles/projectcode/460)
* [MyTetra Web Client (eng)](https://webhamster.ru/site/page/index/articles/projectcode/236)

RUS: Основной репозитарий исходников для разработки MyTetra.<br/>
* [Описание проекта](https://github.com/xintrea/mytetra_dev/blob/experimental/README.md)
* [Официальная русскоязычная страница MyTetra](https://webhamster.ru/site/page/index/articles/projectcode/105)
* [MyTetra Share (rus)](https://webhamster.ru/site/page/index/articles/projectcode/235)
* [MyTetra Web Client (rus)](https://webhamster.ru/site/page/index/articles/projectcode/236)


Материалы о MyTetra
===================

* Статья на ХабраХабр: [Самая нужная программа на свете](https://habrahabr.ru/post/316814/)
* [В поисках мифического Memex. №2](http://blogerator.ru/page/memex-pim-organizers-chast-2)
* [Bидеообзор MyTetra в трех частях](https://www.youtube.com/watch?v=Bmp_dk2LA6I)


Соглашение о кодировании (оформление кода)
==========================================

При написании кода следует руководствоваться следующими соглашениями:

* кодировка файлов исходников: UTF-8;
* в коде допустимы комментарии на русском и английском языках;
* табуляция запрещена, отступы оформляются пробелами;
* ширина отступов - 4 пробела;
* комментарии должны иметь тот же отступ, что и у кода;
* название файла с реализацией класса строится по принципу: ЭтоМойКласс.cpp;
* название файла с заголовком класса строится по принципу: ЭтоМойКласс.h;
* название класса строится по принципу: ЭтоМойКласс;
* название метода строится по принципу: этоМетодКласса;
* название переменных строится по принципу: этоПеременная;
* Открывающая "{" и закрывающая блок скобка "}" должны стоять на отдельных строчках.
* Каждый вновь создаваемый метод должен содержать комментарий, описывающий что делает метод. Также желательно описать назначение параметров метода;
* В заголовочном файле класса, после секции #include необходимо в комментарии написать назначение класса.
* Каждый заголовочный файл должен иметь следующую конструкцию:

    ```
    #ifndef _CLIPBBRANCH_H_
    #define _CLIPBBRANCH_H_
    ...
    #endif // _CLIPBBRANCH_H_
    ```

* Всегда следует использовать классы и контейнеры Qt вместо классов и контейнеров STL;
* Программа MyTetra проектируется как кроссплатформенное Qt-only приложение. Программа MyTetra не должна зависеть от сторонних библиотек, не входящих в состав Qt. Если есть необходимость воспользоваться сторонней библиотекой, включаем ее исходники в проект.

Ветки в GIT
===========

В системе Git создано две основных ветки.

* master - основная ветка с последним стабильным релизом;
* experimental - ветка для разработки.

Помимо этих двух веток временно создаются другие ветки для реализации того или иного функционала. После завершения работ изменения вливаются в ветку experimental. После стабилизации изменений в experimental, изменения вливаются в master и выпускается релиз.

Решив подключиться к разработке MyTetra используйте, пожалуйста, ветку experimental для получения актуального кода.


Сборка
======

MyTetra является программой, написанной по принципу Qt-only.
Это значит, что для сборки MyTetra требуется только наличие Qt SDK.
Никаких дополнительных библиотек для сборки не требуется.

В Linux сборка проводится командами:
```
qmake .
make
```

Установка проводится командой:
```
make install
```

Установленный бинарник размещается по пути:
```
/usr/local/bin/mytetra
```

В Linux, Windows и MacOs сборку можно сделать через QtCreator.
Для этого в QtCreator надо открыть проект mytetra.pro, и выбрать:
```
Сборка - Собрать проект "mytetra"
```

Запуск
======

Для запуска MyTetra надо запустить собранный бинарник.
В разных операционных системах бинарник размещается по разным путям.
Для выяснения, по какому пути расположен собранный или установленный бинарник, надо просмотреть вывод сборки или вывод установки.

Для Linux и MacOs, при сборке генерируется бинарник с именем **mytetra**.
Для Windows при сборке генерируется бинарник с именем **mytetra.exe**.
