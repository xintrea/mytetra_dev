/****************************************************************************
** Meta object code from reading C++ file 'AppConfigPage_Crypt.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/appConfigWindow/AppConfigPage_Crypt.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppConfigPage_Crypt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AppConfigPage_Crypt_t {
    QByteArrayData data[6];
    char stringdata0[116];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AppConfigPage_Crypt_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AppConfigPage_Crypt_t qt_meta_stringdata_AppConfigPage_Crypt = {
    {
QT_MOC_LITERAL(0, 0, 19), // "AppConfigPage_Crypt"
QT_MOC_LITERAL(1, 20, 27), // "onPassRetrieveButtonClicked"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 31), // "onAutoClosePasswordEnableToggle"
QT_MOC_LITERAL(4, 81, 7), // "checked"
QT_MOC_LITERAL(5, 89, 26) // "onPasswordSaveEnableToggle"

    },
    "AppConfigPage_Crypt\0onPassRetrieveButtonClicked\0"
    "\0onAutoClosePasswordEnableToggle\0"
    "checked\0onPasswordSaveEnableToggle"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AppConfigPage_Crypt[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x08 /* Private */,
       3,    1,   30,    2, 0x08 /* Private */,
       5,    1,   33,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,

       0        // eod
};

void AppConfigPage_Crypt::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AppConfigPage_Crypt *_t = static_cast<AppConfigPage_Crypt *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onPassRetrieveButtonClicked(); break;
        case 1: _t->onAutoClosePasswordEnableToggle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->onPasswordSaveEnableToggle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject AppConfigPage_Crypt::staticMetaObject = {
    { &ConfigPage::staticMetaObject, qt_meta_stringdata_AppConfigPage_Crypt.data,
      qt_meta_data_AppConfigPage_Crypt,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AppConfigPage_Crypt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppConfigPage_Crypt::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AppConfigPage_Crypt.stringdata0))
        return static_cast<void*>(const_cast< AppConfigPage_Crypt*>(this));
    return ConfigPage::qt_metacast(_clname);
}

int AppConfigPage_Crypt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ConfigPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
