/****************************************************************************
** Meta object code from reading C++ file 'IndentSlider.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libraries/wyedit/indentslider/IndentSlider.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'IndentSlider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_IndentSlider_t {
    QByteArrayData data[10];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IndentSlider_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IndentSlider_t qt_meta_stringdata_IndentSlider = {
    {
QT_MOC_LITERAL(0, 0, 12), // "IndentSlider"
QT_MOC_LITERAL(1, 13, 21), // "change_textindent_pos"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 1), // "i"
QT_MOC_LITERAL(4, 38, 21), // "change_leftindent_pos"
QT_MOC_LITERAL(5, 60, 22), // "change_rightindent_pos"
QT_MOC_LITERAL(6, 83, 13), // "mouse_release"
QT_MOC_LITERAL(7, 97, 18), // "set_textindent_pos"
QT_MOC_LITERAL(8, 116, 18), // "set_leftindent_pos"
QT_MOC_LITERAL(9, 135, 19) // "set_rightindent_pos"

    },
    "IndentSlider\0change_textindent_pos\0\0"
    "i\0change_leftindent_pos\0change_rightindent_pos\0"
    "mouse_release\0set_textindent_pos\0"
    "set_leftindent_pos\0set_rightindent_pos"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IndentSlider[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       5,    1,   55,    2, 0x06 /* Public */,
       6,    0,   58,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   59,    2, 0x0a /* Public */,
       8,    1,   62,    2, 0x0a /* Public */,
       9,    1,   65,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void IndentSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IndentSlider *_t = static_cast<IndentSlider *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->change_textindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->change_leftindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->change_rightindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->mouse_release(); break;
        case 4: _t->set_textindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->set_leftindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->set_rightindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (IndentSlider::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IndentSlider::change_textindent_pos)) {
                *result = 0;
            }
        }
        {
            typedef void (IndentSlider::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IndentSlider::change_leftindent_pos)) {
                *result = 1;
            }
        }
        {
            typedef void (IndentSlider::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IndentSlider::change_rightindent_pos)) {
                *result = 2;
            }
        }
        {
            typedef void (IndentSlider::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IndentSlider::mouse_release)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject IndentSlider::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_IndentSlider.data,
      qt_meta_data_IndentSlider,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *IndentSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IndentSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_IndentSlider.stringdata0))
        return static_cast<void*>(const_cast< IndentSlider*>(this));
    return QWidget::qt_metacast(_clname);
}

int IndentSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void IndentSlider::change_textindent_pos(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void IndentSlider::change_leftindent_pos(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void IndentSlider::change_rightindent_pos(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void IndentSlider::mouse_release()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
