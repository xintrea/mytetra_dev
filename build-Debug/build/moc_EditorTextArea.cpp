/****************************************************************************
** Meta object code from reading C++ file 'EditorTextArea.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libraries/wyedit/EditorTextArea.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorTextArea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EditorTextArea_t {
    QByteArrayData data[7];
    char stringdata0[109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditorTextArea_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditorTextArea_t qt_meta_stringdata_EditorTextArea = {
    {
QT_MOC_LITERAL(0, 0, 14), // "EditorTextArea"
QT_MOC_LITERAL(1, 15, 25), // "tapAndHoldGestureFinished"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 30), // "updateIndentlineGeometrySignal"
QT_MOC_LITERAL(4, 73, 14), // "show_indetedge"
QT_MOC_LITERAL(5, 88, 1), // "i"
QT_MOC_LITERAL(6, 90, 18) // "set_indentedge_pos"

    },
    "EditorTextArea\0tapAndHoldGestureFinished\0"
    "\0updateIndentlineGeometrySignal\0"
    "show_indetedge\0i\0set_indentedge_pos"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditorTextArea[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       3,    0,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   38,    2, 0x0a /* Public */,
       6,    1,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPoint,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void EditorTextArea::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorTextArea *_t = static_cast<EditorTextArea *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tapAndHoldGestureFinished((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 1: _t->updateIndentlineGeometrySignal(); break;
        case 2: _t->show_indetedge((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->set_indentedge_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (EditorTextArea::*_t)(const QPoint & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorTextArea::tapAndHoldGestureFinished)) {
                *result = 0;
            }
        }
        {
            typedef void (EditorTextArea::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorTextArea::updateIndentlineGeometrySignal)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject EditorTextArea::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_EditorTextArea.data,
      qt_meta_data_EditorTextArea,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *EditorTextArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditorTextArea::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_EditorTextArea.stringdata0))
        return static_cast<void*>(const_cast< EditorTextArea*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int EditorTextArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void EditorTextArea::tapAndHoldGestureFinished(const QPoint & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EditorTextArea::updateIndentlineGeometrySignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
