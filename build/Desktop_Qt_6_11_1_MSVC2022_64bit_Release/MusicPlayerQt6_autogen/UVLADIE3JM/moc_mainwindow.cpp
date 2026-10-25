/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/mainwindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onCoverClicked",
        "",
        "onBackToMain",
        "onPlayPause",
        "onPrev",
        "onNext",
        "onPositionChanged",
        "pos",
        "onDurationChanged",
        "dur",
        "onSliderMoved",
        "value",
        "onAddToFavorite",
        "onCreatePlaylist",
        "onUserButtonClicked",
        "onSongDoubleClicked",
        "row",
        "column",
        "onUserPlaylistSelected",
        "QListWidgetItem*",
        "item",
        "onPlaylistTableContextMenu",
        "QPoint",
        "onAddSongToPlaylist",
        "onRemoveSongFromPlaylist",
        "onAddSongToOtherPlaylist",
        "onBatchAddSongs",
        "onBatchDeleteSongs",
        "onDeletePlaylist",
        "onFavoriteListClicked",
        "mousePressEvent",
        "QMouseEvent*",
        "event",
        "mouseMoveEvent",
        "mouseReleaseEvent",
        "keyPressEvent",
        "QKeyEvent*",
        "eventFilter",
        "watched",
        "QEvent*"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onCoverClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBackToMain'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPlayPause'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPrev'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNext'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPositionChanged'
        QtMocHelpers::SlotData<void(qint64)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 8 },
        }}),
        // Slot 'onDurationChanged'
        QtMocHelpers::SlotData<void(qint64)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 10 },
        }}),
        // Slot 'onSliderMoved'
        QtMocHelpers::SlotData<void(int)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 12 },
        }}),
        // Slot 'onAddToFavorite'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCreatePlaylist'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUserButtonClicked'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSongDoubleClicked'
        QtMocHelpers::SlotData<void(int, int)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 17 }, { QMetaType::Int, 18 },
        }}),
        // Slot 'onUserPlaylistSelected'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
        // Slot 'onPlaylistTableContextMenu'
        QtMocHelpers::SlotData<void(const QPoint &)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 23, 8 },
        }}),
        // Slot 'onAddSongToPlaylist'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemoveSongFromPlaylist'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddSongToOtherPlaylist'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBatchAddSongs'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBatchDeleteSongs'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeletePlaylist'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFavoriteListClicked'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
        // Slot 'mousePressEvent'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 32, 33 },
        }}),
        // Slot 'mouseMoveEvent'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 32, 33 },
        }}),
        // Slot 'mouseReleaseEvent'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 32, 33 },
        }}),
        // Slot 'keyPressEvent'
        QtMocHelpers::SlotData<void(QKeyEvent *)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 37, 33 },
        }}),
        // Slot 'eventFilter'
        QtMocHelpers::SlotData<bool(QObject *, QEvent *)>(38, 2, QMC::AccessPrivate, QMetaType::Bool, {{
            { QMetaType::QObjectStar, 39 }, { 0x80000000 | 40, 33 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onCoverClicked(); break;
        case 1: _t->onBackToMain(); break;
        case 2: _t->onPlayPause(); break;
        case 3: _t->onPrev(); break;
        case 4: _t->onNext(); break;
        case 5: _t->onPositionChanged((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 6: _t->onDurationChanged((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 7: _t->onSliderMoved((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->onAddToFavorite(); break;
        case 9: _t->onCreatePlaylist(); break;
        case 10: _t->onUserButtonClicked(); break;
        case 11: _t->onSongDoubleClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 12: _t->onUserPlaylistSelected((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 13: _t->onPlaylistTableContextMenu((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 14: _t->onAddSongToPlaylist(); break;
        case 15: _t->onRemoveSongFromPlaylist(); break;
        case 16: _t->onAddSongToOtherPlaylist(); break;
        case 17: _t->onBatchAddSongs(); break;
        case 18: _t->onBatchDeleteSongs(); break;
        case 19: _t->onDeletePlaylist(); break;
        case 20: _t->onFavoriteListClicked((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 21: _t->mousePressEvent((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 22: _t->mouseMoveEvent((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 23: _t->mouseReleaseEvent((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 24: _t->keyPressEvent((*reinterpret_cast<std::add_pointer_t<QKeyEvent*>>(_a[1]))); break;
        case 25: { bool _r = _t->eventFilter((*reinterpret_cast<std::add_pointer_t<QObject*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QEvent*>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 26)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 26;
    }
    return _id;
}
QT_WARNING_POP
