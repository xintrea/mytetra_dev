/****************************************************************************
** Meta object code from reading C++ file 'TreeScreen.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/tree/TreeScreen.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TreeScreen.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TreeScreen_t {
    QByteArrayData data[23];
    char stringdata0[323];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TreeScreen_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TreeScreen_t qt_meta_stringdata_TreeScreen = {
    {
QT_MOC_LITERAL(0, 0, 10), // "TreeScreen"
QT_MOC_LITERAL(1, 11, 20), // "expand_all_subbranch"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 22), // "collapse_all_subbranch"
QT_MOC_LITERAL(4, 56, 26), // "expand_or_collapse_recurse"
QT_MOC_LITERAL(5, 83, 5), // "index"
QT_MOC_LITERAL(6, 89, 4), // "mode"
QT_MOC_LITERAL(7, 94, 13), // "ins_subbranch"
QT_MOC_LITERAL(8, 108, 10), // "ins_branch"
QT_MOC_LITERAL(9, 119, 11), // "edit_branch"
QT_MOC_LITERAL(10, 131, 10), // "del_branch"
QT_MOC_LITERAL(11, 142, 14), // "del_one_branch"
QT_MOC_LITERAL(12, 157, 14), // "move_up_branch"
QT_MOC_LITERAL(13, 172, 14), // "move_dn_branch"
QT_MOC_LITERAL(14, 187, 10), // "cut_branch"
QT_MOC_LITERAL(15, 198, 11), // "copy_branch"
QT_MOC_LITERAL(16, 210, 12), // "paste_branch"
QT_MOC_LITERAL(17, 223, 15), // "paste_subbranch"
QT_MOC_LITERAL(18, 239, 14), // "encrypt_branch"
QT_MOC_LITERAL(19, 254, 14), // "decrypt_branch"
QT_MOC_LITERAL(20, 269, 19), // "on_knowtree_clicked"
QT_MOC_LITERAL(21, 289, 29), // "on_customContextMenuRequested"
QT_MOC_LITERAL(22, 319, 3) // "pos"

    },
    "TreeScreen\0expand_all_subbranch\0\0"
    "collapse_all_subbranch\0"
    "expand_or_collapse_recurse\0index\0mode\0"
    "ins_subbranch\0ins_branch\0edit_branch\0"
    "del_branch\0del_one_branch\0move_up_branch\0"
    "move_dn_branch\0cut_branch\0copy_branch\0"
    "paste_branch\0paste_subbranch\0"
    "encrypt_branch\0decrypt_branch\0"
    "on_knowtree_clicked\0on_customContextMenuRequested\0"
    "pos"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TreeScreen[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x08 /* Private */,
       3,    0,  110,    2, 0x08 /* Private */,
       4,    2,  111,    2, 0x08 /* Private */,
       7,    0,  116,    2, 0x08 /* Private */,
       8,    0,  117,    2, 0x08 /* Private */,
       9,    0,  118,    2, 0x08 /* Private */,
      10,    1,  119,    2, 0x08 /* Private */,
      10,    0,  122,    2, 0x28 /* Private | MethodCloned */,
      11,    1,  123,    2, 0x08 /* Private */,
      12,    0,  126,    2, 0x08 /* Private */,
      13,    0,  127,    2, 0x08 /* Private */,
      14,    0,  128,    2, 0x08 /* Private */,
      15,    0,  129,    2, 0x08 /* Private */,
      16,    0,  130,    2, 0x08 /* Private */,
      17,    0,  131,    2, 0x08 /* Private */,
      18,    0,  132,    2, 0x08 /* Private */,
      19,    0,  133,    2, 0x08 /* Private */,
      20,    1,  134,    2, 0x08 /* Private */,
      21,    1,  137,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Bool,    5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    5,
    QMetaType::Void, QMetaType::QPoint,   22,

       0        // eod
};

void TreeScreen::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TreeScreen *_t = static_cast<TreeScreen *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->expand_all_subbranch(); break;
        case 1: _t->collapse_all_subbranch(); break;
        case 2: _t->expand_or_collapse_recurse((*reinterpret_cast< QModelIndex(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->ins_subbranch(); break;
        case 4: _t->ins_branch(); break;
        case 5: _t->edit_branch(); break;
        case 6: _t->del_branch((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->del_branch(); break;
        case 8: _t->del_one_branch((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        case 9: _t->move_up_branch(); break;
        case 10: _t->move_dn_branch(); break;
        case 11: _t->cut_branch(); break;
        case 12: { bool _r = _t->copy_branch();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: _t->paste_branch(); break;
        case 14: _t->paste_subbranch(); break;
        case 15: _t->encrypt_branch(); break;
        case 16: _t->decrypt_branch(); break;
        case 17: _t->on_knowtree_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 18: _t->on_customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject TreeScreen::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TreeScreen.data,
      qt_meta_data_TreeScreen,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TreeScreen::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TreeScreen::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TreeScreen.stringdata0))
        return static_cast<void*>(const_cast< TreeScreen*>(this));
    return QWidget::qt_metacast(_clname);
}

int TreeScreen::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
