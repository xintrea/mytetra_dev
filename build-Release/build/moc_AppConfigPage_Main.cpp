/****************************************************************************
** Meta object code from reading C++ file 'AppConfigPage_Main.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/appConfigWindow/AppConfigPage_Main.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppConfigPage_Main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AppConfigPage_Main_t {
    QByteArrayData data[8];
    char stringdata0[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AppConfigPage_Main_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AppConfigPage_Main_t qt_meta_stringdata_AppConfigPage_Main = {
    {
QT_MOC_LITERAL(0, 0, 18), // "AppConfigPage_Main"
QT_MOC_LITERAL(1, 19, 27), // "open_tetradir_select_dialog"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 27), // "open_trashdir_select_dialog"
QT_MOC_LITERAL(4, 76, 35), // "onDisableCustomDateTimeFormat..."
QT_MOC_LITERAL(5, 112, 7), // "checked"
QT_MOC_LITERAL(6, 120, 34), // "onEnableCustomDateTimeFormatT..."
QT_MOC_LITERAL(7, 155, 26) // "onDateTimeFormatHelpButton"

    },
    "AppConfigPage_Main\0open_tetradir_select_dialog\0"
    "\0open_trashdir_select_dialog\0"
    "onDisableCustomDateTimeFormatToggle\0"
    "checked\0onEnableCustomDateTimeFormatToggle\0"
    "onDateTimeFormatHelpButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AppConfigPage_Main[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    1,   41,    2, 0x08 /* Private */,
       6,    1,   44,    2, 0x08 /* Private */,
       7,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,

       0        // eod
};

void AppConfigPage_Main::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AppConfigPage_Main *_t = static_cast<AppConfigPage_Main *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->open_tetradir_select_dialog(); break;
        case 1: _t->open_trashdir_select_dialog(); break;
        case 2: _t->onDisableCustomDateTimeFormatToggle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->onEnableCustomDateTimeFormatToggle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->onDateTimeFormatHelpButton(); break;
        default: ;
        }
    }
}

const QMetaObject AppConfigPage_Main::staticMetaObject = {
    { &ConfigPage::staticMetaObject, qt_meta_stringdata_AppConfigPage_Main.data,
      qt_meta_data_AppConfigPage_Main,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AppConfigPage_Main::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppConfigPage_Main::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AppConfigPage_Main.stringdata0))
        return static_cast<void*>(const_cast< AppConfigPage_Main*>(this));
    return ConfigPage::qt_metacast(_clname);
}

int AppConfigPage_Main::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ConfigPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
