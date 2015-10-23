/****************************************************************************
** Meta object code from reading C++ file 'RecordTableController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/controllers/recordTable/RecordTableController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RecordTableController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RecordTableController_t {
    QByteArrayData data[17];
    char stringdata0[217];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RecordTableController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RecordTableController_t qt_meta_stringdata_RecordTableController = {
    {
QT_MOC_LITERAL(0, 0, 21), // "RecordTableController"
QT_MOC_LITERAL(1, 22, 3), // "cut"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 4), // "copy"
QT_MOC_LITERAL(4, 32, 5), // "paste"
QT_MOC_LITERAL(5, 38, 18), // "onEditFieldContext"
QT_MOC_LITERAL(6, 57, 13), // "deleteRecords"
QT_MOC_LITERAL(7, 71, 18), // "addNewToEndContext"
QT_MOC_LITERAL(8, 90, 19), // "addNewBeforeContext"
QT_MOC_LITERAL(9, 110, 18), // "addNewAfterContext"
QT_MOC_LITERAL(10, 129, 13), // "deleteContext"
QT_MOC_LITERAL(11, 143, 6), // "moveUp"
QT_MOC_LITERAL(12, 150, 6), // "moveDn"
QT_MOC_LITERAL(13, 157, 11), // "onSortClick"
QT_MOC_LITERAL(14, 169, 8), // "settings"
QT_MOC_LITERAL(15, 178, 25), // "onRecordTableConfigChange"
QT_MOC_LITERAL(16, 204, 12) // "onPrintClick"

    },
    "RecordTableController\0cut\0\0copy\0paste\0"
    "onEditFieldContext\0deleteRecords\0"
    "addNewToEndContext\0addNewBeforeContext\0"
    "addNewAfterContext\0deleteContext\0"
    "moveUp\0moveDn\0onSortClick\0settings\0"
    "onRecordTableConfigChange\0onPrintClick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RecordTableController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x0a /* Public */,
       3,    0,   90,    2, 0x0a /* Public */,
       4,    0,   91,    2, 0x0a /* Public */,
       5,    0,   92,    2, 0x0a /* Public */,
       6,    0,   93,    2, 0x0a /* Public */,
       7,    0,   94,    2, 0x0a /* Public */,
       8,    0,   95,    2, 0x0a /* Public */,
       9,    0,   96,    2, 0x0a /* Public */,
      10,    0,   97,    2, 0x0a /* Public */,
      11,    0,   98,    2, 0x0a /* Public */,
      12,    0,   99,    2, 0x0a /* Public */,
      13,    0,  100,    2, 0x0a /* Public */,
      14,    0,  101,    2, 0x0a /* Public */,
      15,    0,  102,    2, 0x0a /* Public */,
      16,    0,  103,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RecordTableController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RecordTableController *_t = static_cast<RecordTableController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cut(); break;
        case 1: _t->copy(); break;
        case 2: _t->paste(); break;
        case 3: _t->onEditFieldContext(); break;
        case 4: _t->deleteRecords(); break;
        case 5: _t->addNewToEndContext(); break;
        case 6: _t->addNewBeforeContext(); break;
        case 7: _t->addNewAfterContext(); break;
        case 8: _t->deleteContext(); break;
        case 9: _t->moveUp(); break;
        case 10: _t->moveDn(); break;
        case 11: _t->onSortClick(); break;
        case 12: _t->settings(); break;
        case 13: _t->onRecordTableConfigChange(); break;
        case 14: _t->onPrintClick(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject RecordTableController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RecordTableController.data,
      qt_meta_data_RecordTableController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RecordTableController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RecordTableController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RecordTableController.stringdata0))
        return static_cast<void*>(const_cast< RecordTableController*>(this));
    return QObject::qt_metacast(_clname);
}

int RecordTableController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
