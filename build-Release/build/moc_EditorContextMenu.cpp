/****************************************************************************
** Meta object code from reading C++ file 'EditorContextMenu.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libraries/wyedit/EditorContextMenu.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorContextMenu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EditorContextMenu_t {
    QByteArrayData data[9];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditorContextMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditorContextMenu_t qt_meta_stringdata_EditorContextMenu = {
    {
QT_MOC_LITERAL(0, 0, 17), // "EditorContextMenu"
QT_MOC_LITERAL(1, 18, 4), // "undo"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 4), // "redo"
QT_MOC_LITERAL(4, 29, 3), // "cut"
QT_MOC_LITERAL(5, 33, 4), // "copy"
QT_MOC_LITERAL(6, 38, 5), // "paste"
QT_MOC_LITERAL(7, 44, 9), // "selectAll"
QT_MOC_LITERAL(8, 54, 30) // "contextMenuEditImageProperties"

    },
    "EditorContextMenu\0undo\0\0redo\0cut\0copy\0"
    "paste\0selectAll\0contextMenuEditImageProperties"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditorContextMenu[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,
       4,    0,   51,    2, 0x06 /* Public */,
       5,    0,   52,    2, 0x06 /* Public */,
       6,    0,   53,    2, 0x06 /* Public */,
       7,    0,   54,    2, 0x06 /* Public */,
       8,    0,   55,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void EditorContextMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorContextMenu *_t = static_cast<EditorContextMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->undo(); break;
        case 1: _t->redo(); break;
        case 2: _t->cut(); break;
        case 3: _t->copy(); break;
        case 4: _t->paste(); break;
        case 5: _t->selectAll(); break;
        case 6: _t->contextMenuEditImageProperties(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (EditorContextMenu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorContextMenu::undo)) {
                *result = 0;
            }
        }
        {
            typedef void (EditorContextMenu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorContextMenu::redo)) {
                *result = 1;
            }
        }
        {
            typedef void (EditorContextMenu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorContextMenu::cut)) {
                *result = 2;
            }
        }
        {
            typedef void (EditorContextMenu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorContextMenu::copy)) {
                *result = 3;
            }
        }
        {
            typedef void (EditorContextMenu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorContextMenu::paste)) {
                *result = 4;
            }
        }
        {
            typedef void (EditorContextMenu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorContextMenu::selectAll)) {
                *result = 5;
            }
        }
        {
            typedef void (EditorContextMenu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorContextMenu::contextMenuEditImageProperties)) {
                *result = 6;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject EditorContextMenu::staticMetaObject = {
    { &QMenu::staticMetaObject, qt_meta_stringdata_EditorContextMenu.data,
      qt_meta_data_EditorContextMenu,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *EditorContextMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditorContextMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_EditorContextMenu.stringdata0))
        return static_cast<void*>(const_cast< EditorContextMenu*>(this));
    return QMenu::qt_metacast(_clname);
}

int EditorContextMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void EditorContextMenu::undo()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void EditorContextMenu::redo()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void EditorContextMenu::cut()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void EditorContextMenu::copy()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void EditorContextMenu::paste()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void EditorContextMenu::selectAll()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void EditorContextMenu::contextMenuEditImageProperties()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
