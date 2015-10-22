mytetra_dev
===========

ENG: Central source repository for MyTetra development.<br/> 
* ["Read me" on english](https://github.com/xintrea/mytetra_dev/blob/experimental/README_ENG.md)
* [MyTetra official english page](http://webhamster.ru/site/page/index/articles/projectcode/138)

RUS: Основной репозитарий исходников для разработки MyTetra.<br/>
* [Описание проекта](https://github.com/xintrea/mytetra_dev/blob/experimental/README.md)
* [Официальная русскоязычная страница MyTetra](http://webhamster.ru/site/page/index/articles/projectcode/105)


Agreement on the coding (registration code)
===========================================

When writing code to be guided by the following agreements:

* Source file encoding: UTF-8;
* Comments in the code allowed the Russian and English languages;
* Tab prohibited padding spaces are made;
* Margin - 2 spaces;
* Comments must have the same indentation that of the code;
* The name of the file with the implementation of the class is based on the principle ItIsMyClass.cpp;
* The name of the file with the title of class is based on the principle ItIsMyClass.h;
* The name of the class is based on the principle ItIsMyClass;
* The name of the method is based on the principle itIsMyMethod;
* The name of the variables is based on the principle itIsVariable;
* The opening "{" and closing bracket "}" should be on separate lines. It is unacceptable to place an opening bracket in the line for (), if (), etc .;
* Each newly created method should contain a comment that describes what the method does. So it is desirable to describe the purpose of the method parameters;
* The header class, after the section is necessary in #include class assignment to write comments.

* Each header file must have the following structure:

```
#ifndef _CLIPBBRANCH_H_
#define _CLIPBBRANCH_H_
...
#endif // _CLIPBBRANCH_H_
```

* Always use the classes, and classes instead of Qt's containers and containers STL;
* The program is designed as a cross-platform MyTetra Qt-only application. MyTetra program should not depend on third-party libraries that are not part of Qt. If you need to use a third-party library of its source code to the project.

Branches at GIT
===============

The system Git created two main branches.

*  Master - the main branch with the latest stable release;
*  Experimental - branch to develop.

Besides these two branches have been established in other branches for the implementation of a functional. Upon completion of the changes flow into the branch experimental. After stabilization of the changes in the experimental, the changes flow into the master and release is issued.

Deciding to connect to the development MyTetra, please use the experimental branch for the current code.
