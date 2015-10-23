/****************************************************************************
** Meta object code from reading C++ file 'RecordTableView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/recordTable/RecordTableView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RecordTableView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RecordTableView_t {
    QByteArrayData data[21];
    char stringdata0[298];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RecordTableView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RecordTableView_t qt_meta_stringdata_RecordTableView = {
    {
QT_MOC_LITERAL(0, 0, 15), // "RecordTableView"
QT_MOC_LITERAL(1, 16, 20), // "listSelectionChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 14), // "QItemSelection"
QT_MOC_LITERAL(4, 53, 8), // "selected"
QT_MOC_LITERAL(5, 62, 10), // "deselected"
QT_MOC_LITERAL(6, 73, 25), // "tapAndHoldGestureFinished"
QT_MOC_LITERAL(7, 99, 28), // "onCustomContextMenuRequested"
QT_MOC_LITERAL(8, 128, 3), // "pos"
QT_MOC_LITERAL(9, 132, 14), // "onSectionMoved"
QT_MOC_LITERAL(10, 147, 12), // "logicalIndex"
QT_MOC_LITERAL(11, 160, 14), // "oldVisualIndex"
QT_MOC_LITERAL(12, 175, 14), // "newVisualIndex"
QT_MOC_LITERAL(13, 190, 16), // "onSectionResized"
QT_MOC_LITERAL(14, 207, 7), // "oldSize"
QT_MOC_LITERAL(15, 215, 7), // "newSize"
QT_MOC_LITERAL(16, 223, 16), // "editFieldContext"
QT_MOC_LITERAL(17, 240, 18), // "onSelectionChanged"
QT_MOC_LITERAL(18, 259, 16), // "selectionChanged"
QT_MOC_LITERAL(19, 276, 15), // "onClickToRecord"
QT_MOC_LITERAL(20, 292, 5) // "index"

    },
    "RecordTableView\0listSelectionChanged\0"
    "\0QItemSelection\0selected\0deselected\0"
    "tapAndHoldGestureFinished\0"
    "onCustomContextMenuRequested\0pos\0"
    "onSectionMoved\0logicalIndex\0oldVisualIndex\0"
    "newVisualIndex\0onSectionResized\0oldSize\0"
    "newSize\0editFieldContext\0onSelectionChanged\0"
    "selectionChanged\0onClickToRecord\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RecordTableView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x06 /* Public */,
       6,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   67,    2, 0x0a /* Public */,
       9,    3,   70,    2, 0x0a /* Public */,
      13,    3,   77,    2, 0x0a /* Public */,
      16,    0,   84,    2, 0x0a /* Public */,
      17,    2,   85,    2, 0x09 /* Protected */,
      18,    2,   90,    2, 0x09 /* Protected */,
      19,    1,   95,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, QMetaType::QPoint,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   10,   11,   12,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   10,   14,   15,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, QMetaType::QModelIndex,   20,

       0        // eod
};

void RecordTableView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RecordTableView *_t = static_cast<RecordTableView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->listSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 1: _t->tapAndHoldGestureFinished((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->onCustomContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 3: _t->onSectionMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: _t->onSectionResized((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: _t->editFieldContext(); break;
        case 6: _t->onSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 7: _t->selectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 8: _t->onClickToRecord((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QItemSelection >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QItemSelection >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QItemSelection >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RecordTableView::*_t)(const QItemSelection & , const QItemSelection & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RecordTableView::listSelectionChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (RecordTableView::*_t)(const QPoint & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RecordTableView::tapAndHoldGestureFinished)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject RecordTableView::staticMetaObject = {
    { &QTableView::staticMetaObject, qt_meta_stringdata_RecordTableView.data,
      qt_meta_data_RecordTableView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RecordTableView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RecordTableView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RecordTableView.stringdata0))
        return static_cast<void*>(const_cast< RecordTableView*>(this));
    return QTableView::qt_metacast(_clname);
}

int RecordTableView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void RecordTableView::listSelectionChanged(const QItemSelection & _t1, const QItemSelection & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RecordTableView::tapAndHoldGestureFinished(const QPoint & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
