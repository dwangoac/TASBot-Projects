/****************************************************************************
** Meta object code from reading C++ file 'bot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "bot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_bot_t {
    QByteArrayData data[14];
    char stringdata0[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_bot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_bot_t qt_meta_stringdata_bot = {
    {
QT_MOC_LITERAL(0, 0, 3), // "bot"
QT_MOC_LITERAL(1, 4, 13), // "trigger_emote"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 5), // "emote"
QT_MOC_LITERAL(4, 25, 1), // "x"
QT_MOC_LITERAL(5, 27, 1), // "y"
QT_MOC_LITERAL(6, 29, 13), // "trigger_color"
QT_MOC_LITERAL(7, 43, 5), // "color"
QT_MOC_LITERAL(8, 49, 5), // "space"
QT_MOC_LITERAL(9, 55, 4), // "join"
QT_MOC_LITERAL(10, 60, 7), // "channel"
QT_MOC_LITERAL(11, 68, 14), // "processMessage"
QT_MOC_LITERAL(12, 83, 18), // "IrcPrivateMessage*"
QT_MOC_LITERAL(13, 102, 7) // "message"

    },
    "bot\0trigger_emote\0\0emote\0x\0y\0trigger_color\0"
    "color\0space\0join\0channel\0processMessage\0"
    "IrcPrivateMessage*\0message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_bot[] = {

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
       1,    3,   34,    2, 0x06 /* Public */,
       6,    2,   41,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   46,    2, 0x0a /* Public */,
      11,    1,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    7,    8,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, 0x80000000 | 12,   13,

       0        // eod
};

void bot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        bot *_t = static_cast<bot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->trigger_emote((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->trigger_color((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->join((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->processMessage((*reinterpret_cast< IrcPrivateMessage*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< IrcPrivateMessage* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (bot::*_t)(QString , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&bot::trigger_emote)) {
                *result = 0;
            }
        }
        {
            typedef void (bot::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&bot::trigger_color)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject bot::staticMetaObject = {
    { &IrcConnection::staticMetaObject, qt_meta_stringdata_bot.data,
      qt_meta_data_bot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *bot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *bot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_bot.stringdata0))
        return static_cast<void*>(const_cast< bot*>(this));
    return IrcConnection::qt_metacast(_clname);
}

int bot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = IrcConnection::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void bot::trigger_emote(QString _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void bot::trigger_color(QString _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
