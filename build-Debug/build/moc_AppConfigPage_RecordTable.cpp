/****************************************************************************
** Meta object code from reading C++ file 'AppConfigPage_RecordTable.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/appConfigWindow/AppConfigPage_RecordTable.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppConfigPage_RecordTable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AppConfigPage_RecordTable_t {
    QByteArrayData data[4];
    char stringdata0[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AppConfigPage_RecordTable_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AppConfigPage_RecordTable_t qt_meta_stringdata_AppConfigPage_RecordTable = {
    {
QT_MOC_LITERAL(0, 0, 25), // "AppConfigPage_RecordTable"
QT_MOC_LITERAL(1, 26, 23), // "recordTableConfigChange"
QT_MOC_LITERAL(2, 50, 0), // ""
QT_MOC_LITERAL(3, 51, 13) // "onFieldToggle"

    },
    "AppConfigPage_RecordTable\0"
    "recordTableConfigChange\0\0onFieldToggle"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AppConfigPage_RecordTable[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   25,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void AppConfigPage_RecordTable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AppConfigPage_RecordTable *_t = static_cast<AppConfigPage_RecordTable *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->recordTableConfigChange(); break;
        case 1: _t->onFieldToggle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AppConfigPage_RecordTable::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AppConfigPage_RecordTable::recordTableConfigChange)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject AppConfigPage_RecordTable::staticMetaObject = {
    { &ConfigPage::staticMetaObject, qt_meta_stringdata_AppConfigPage_RecordTable.data,
      qt_meta_data_AppConfigPage_RecordTable,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AppConfigPage_RecordTable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppConfigPage_RecordTable::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AppConfigPage_RecordTable.stringdata0))
        return static_cast<void*>(const_cast< AppConfigPage_RecordTable*>(this));
    return ConfigPage::qt_metacast(_clname);
}

int AppConfigPage_RecordTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ConfigPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void AppConfigPage_RecordTable::recordTableConfigChange()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
