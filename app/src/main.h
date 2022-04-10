#ifndef MYTETRA_MAIN_H
#define MYTETRA_MAIN_H

#include <stdio.h>

#include <QApplication>
#include <QClipboard>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QtGlobal>

#include <QDomNode>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QToolBar>
#include <QAbstractItemView>


// ----------------------------------------------------------
// MyTetra - программа для накопления осмысленной информации
// Волгодонск, Санкт-Петербург 2010 - 2020 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензиями:
// - GPL v.3
// - Modified BSD
// © Степанов С. М. и товарищи 2010 - 2020
// ----------------------------------------------------------

// Версия программы
#define APPLICATION_RELEASE_VERSION         1
#define APPLICATION_RELEASE_SUBVERSION     44
#define APPLICATION_RELEASE_MICROVERSION  165

// Поддерживаемая версия формата базы (хранилища)
#define CURRENT_FORMAT_VERSION    1
#define CURRENT_FORMAT_SUBVERSION 2

// Управление трассировкой
#ifdef QT_DEBUG
#define TRACELOG TraceLogger logger(__FILE__, __FUNCTION__, __LINE__);
#else
#define TRACELOG
#endif

#ifdef Q_CC_MSVC
[[noreturn]] __forceinline void __builtin_unreachable() {__assume(false);}
#endif

#endif // MYTETRA_MAIN_H
