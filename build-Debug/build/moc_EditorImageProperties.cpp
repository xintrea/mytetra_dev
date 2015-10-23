/****************************************************************************
** Meta object code from reading C++ file 'EditorImageProperties.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libraries/wyedit/EditorImageProperties.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorImageProperties.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EditorImageProperties_t {
    QByteArrayData data[7];
    char stringdata0[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditorImageProperties_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditorImageProperties_t qt_meta_stringdata_EditorImageProperties = {
    {
QT_MOC_LITERAL(0, 0, 21), // "EditorImageProperties"
QT_MOC_LITERAL(1, 22, 18), // "on_changed_percent"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 1), // "n"
QT_MOC_LITERAL(4, 44, 16), // "on_changed_width"
QT_MOC_LITERAL(5, 61, 17), // "on_changed_height"
QT_MOC_LITERAL(6, 79, 19) // "on_click_reset_size"

    },
    "EditorImageProperties\0on_changed_percent\0"
    "\0n\0on_changed_width\0on_changed_height\0"
    "on_click_reset_size"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditorImageProperties[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       4,    1,   37,    2, 0x08 /* Private */,
       5,    1,   40,    2, 0x08 /* Private */,
       6,    0,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,

       0        // eod
};

void EditorImageProperties::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorImageProperties *_t = static_cast<EditorImageProperties *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_changed_percent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_changed_width((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_changed_height((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_click_reset_size(); break;
        default: ;
        }
    }
}

const QMetaObject EditorImageProperties::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EditorImageProperties.data,
      qt_meta_data_EditorImageProperties,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *EditorImageProperties::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditorImageProperties::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_EditorImageProperties.stringdata0))
        return static_cast<void*>(const_cast< EditorImageProperties*>(this));
    return QDialog::qt_metacast(_clname);
}

int EditorImageProperties::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
