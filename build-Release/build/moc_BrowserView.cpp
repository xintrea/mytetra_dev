/****************************************************************************
** Meta object code from reading C++ file 'BrowserView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/BrowserView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BrowserView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BrowserView_t {
    QByteArrayData data[18];
    char stringdata0[170];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BrowserView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BrowserView_t qt_meta_stringdata_BrowserView = {
    {
QT_MOC_LITERAL(0, 0, 11), // "BrowserView"
QT_MOC_LITERAL(1, 12, 6), // "setUrl"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 3), // "url"
QT_MOC_LITERAL(4, 24, 13), // "setScrollbars"
QT_MOC_LITERAL(5, 38, 4), // "hide"
QT_MOC_LITERAL(6, 43, 8), // "setCache"
QT_MOC_LITERAL(7, 52, 5), // "cache"
QT_MOC_LITERAL(8, 58, 9), // "cacheSize"
QT_MOC_LITERAL(9, 68, 8), // "finished"
QT_MOC_LITERAL(10, 77, 14), // "QNetworkReply*"
QT_MOC_LITERAL(11, 92, 5), // "reply"
QT_MOC_LITERAL(12, 98, 9), // "sslErrors"
QT_MOC_LITERAL(13, 108, 16), // "QList<QSslError>"
QT_MOC_LITERAL(14, 125, 6), // "errors"
QT_MOC_LITERAL(15, 132, 15), // "getactionFreeze"
QT_MOC_LITERAL(16, 148, 8), // "QAction*"
QT_MOC_LITERAL(17, 157, 12) // "setupActions"

    },
    "BrowserView\0setUrl\0\0url\0setScrollbars\0"
    "hide\0setCache\0cache\0cacheSize\0finished\0"
    "QNetworkReply*\0reply\0sslErrors\0"
    "QList<QSslError>\0errors\0getactionFreeze\0"
    "QAction*\0setupActions"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BrowserView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x0a /* Public */,
       4,    1,   52,    2, 0x0a /* Public */,
       6,    2,   55,    2, 0x0a /* Public */,
       9,    1,   60,    2, 0x0a /* Public */,
      12,    2,   63,    2, 0x0a /* Public */,
      15,    0,   68,    2, 0x0a /* Public */,
      17,    0,   69,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    7,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 13,   11,   14,
    0x80000000 | 16,
    QMetaType::Void,

       0        // eod
};

void BrowserView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BrowserView *_t = static_cast<BrowserView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setUrl((*reinterpret_cast< QUrl(*)>(_a[1]))); break;
        case 1: _t->setScrollbars((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->setCache((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->finished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 4: _t->sslErrors((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< const QList<QSslError>(*)>(_a[2]))); break;
        case 5: { QAction* _r = _t->getactionFreeze();
            if (_a[0]) *reinterpret_cast< QAction**>(_a[0]) = _r; }  break;
        case 6: _t->setupActions(); break;
        default: ;
        }
    }
}

const QMetaObject BrowserView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BrowserView.data,
      qt_meta_data_BrowserView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *BrowserView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BrowserView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_BrowserView.stringdata0))
        return static_cast<void*>(const_cast< BrowserView*>(this));
    return QWidget::qt_metacast(_clname);
}

int BrowserView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
