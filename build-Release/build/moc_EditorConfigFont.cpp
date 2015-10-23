/****************************************************************************
** Meta object code from reading C++ file 'EditorConfigFont.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libraries/wyedit/EditorConfigFont.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorConfigFont.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EditorConfigFont_t {
    QByteArrayData data[7];
    char stringdata0[157];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditorConfigFont_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditorConfigFont_t qt_meta_stringdata_EditorConfigFont = {
    {
QT_MOC_LITERAL(0, 0, 16), // "EditorConfigFont"
QT_MOC_LITERAL(1, 17, 36), // "on_monospace_applysize_state_..."
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 1), // "i"
QT_MOC_LITERAL(4, 57, 31), // "on_code_applysize_state_changed"
QT_MOC_LITERAL(5, 89, 33), // "on_code_applyindent_state_cha..."
QT_MOC_LITERAL(6, 123, 33) // "on_code_select_color_button_c..."

    },
    "EditorConfigFont\0on_monospace_applysize_state_changed\0"
    "\0i\0on_code_applysize_state_changed\0"
    "on_code_applyindent_state_changed\0"
    "on_code_select_color_button_click"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditorConfigFont[] = {

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

void EditorConfigFont::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorConfigFont *_t = static_cast<EditorConfigFont *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_monospace_applysize_state_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_code_applysize_state_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_code_applyindent_state_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_code_select_color_button_click(); break;
        default: ;
        }
    }
}

const QMetaObject EditorConfigFont::staticMetaObject = {
    { &ConfigPage::staticMetaObject, qt_meta_stringdata_EditorConfigFont.data,
      qt_meta_data_EditorConfigFont,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *EditorConfigFont::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditorConfigFont::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_EditorConfigFont.stringdata0))
        return static_cast<void*>(const_cast< EditorConfigFont*>(this));
    return ConfigPage::qt_metacast(_clname);
}

int EditorConfigFont::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ConfigPage::qt_metacall(_c, _id, _a);
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
