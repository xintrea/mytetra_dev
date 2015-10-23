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
    QByteArrayData data[20];
    char stringdata0[183];
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
QT_MOC_LITERAL(4, 24, 7), // "loadUrl"
QT_MOC_LITERAL(5, 32, 4), // "_url"
QT_MOC_LITERAL(6, 37, 13), // "setScrollbars"
QT_MOC_LITERAL(7, 51, 4), // "hide"
QT_MOC_LITERAL(8, 56, 8), // "setCache"
QT_MOC_LITERAL(9, 65, 5), // "cache"
QT_MOC_LITERAL(10, 71, 9), // "cacheSize"
QT_MOC_LITERAL(11, 81, 8), // "finished"
QT_MOC_LITERAL(12, 90, 14), // "QNetworkReply*"
QT_MOC_LITERAL(13, 105, 5), // "reply"
QT_MOC_LITERAL(14, 111, 9), // "sslErrors"
QT_MOC_LITERAL(15, 121, 16), // "QList<QSslError>"
QT_MOC_LITERAL(16, 138, 6), // "errors"
QT_MOC_LITERAL(17, 145, 15), // "getactionFreeze"
QT_MOC_LITERAL(18, 161, 8), // "QAction*"
QT_MOC_LITERAL(19, 170, 12) // "setupActions"

    },
    "BrowserView\0setUrl\0\0url\0loadUrl\0_url\0"
    "setScrollbars\0hide\0setCache\0cache\0"
    "cacheSize\0finished\0QNetworkReply*\0"
    "reply\0sslErrors\0QList<QSslError>\0"
    "errors\0getactionFreeze\0QAction*\0"
    "setupActions"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BrowserView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x0a /* Public */,
       4,    1,   62,    2, 0x0a /* Public */,
       4,    0,   65,    2, 0x2a /* Public | MethodCloned */,
       6,    1,   66,    2, 0x0a /* Public */,
       8,    2,   69,    2, 0x0a /* Public */,
      11,    1,   74,    2, 0x0a /* Public */,
      14,    2,   77,    2, 0x0a /* Public */,
      17,    0,   82,    2, 0x0a /* Public */,
      19,    0,   83,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl,    3,
    QMetaType::Void, QMetaType::QUrl,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    9,   10,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 15,   13,   16,
    0x80000000 | 18,
    QMetaType::Void,

       0        // eod
};

void BrowserView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BrowserView *_t = static_cast<BrowserView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->loadUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 2: _t->loadUrl(); break;
        case 3: _t->setScrollbars((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setCache((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->finished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 6: _t->sslErrors((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< const QList<QSslError>(*)>(_a[2]))); break;
        case 7: { QAction* _r = _t->getactionFreeze();
            if (_a[0]) *reinterpret_cast< QAction**>(_a[0]) = _r; }  break;
        case 8: _t->setupActions(); break;
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
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
