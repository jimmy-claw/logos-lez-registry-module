/****************************************************************************
** Meta object code from reading C++ file 'lez_registry_module.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/lez_registry_module.h"
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lez_registry_module.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
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
struct qt_meta_tag_ZN17LezRegistryModuleE_t {};
} // unnamed namespace

template <> constexpr inline auto LezRegistryModule::qt_create_metaobjectdata<qt_meta_tag_ZN17LezRegistryModuleE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LezRegistryModule",
        "eventResponse",
        "",
        "eventName",
        "QVariantList",
        "data",
        "idlFetched",
        "content",
        "error",
        "programRegistered",
        "registerFailed",
        "initLogos",
        "LogosAPI*",
        "logosAPIInstance",
        "registerProgram",
        "argsJson",
        "updateProgram",
        "listPrograms",
        "getProgramByName",
        "name",
        "getProgramById",
        "programId",
        "uploadIdl",
        "downloadIdl",
        "cid",
        "fetchIdlAsync",
        "registerAsync"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'eventResponse'
        QtMocHelpers::SignalData<void(const QString &, const QVariantList &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Signal 'idlFetched'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'programRegistered'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'registerFailed'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Method 'initLogos'
        QtMocHelpers::MethodData<void(LogosAPI *)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Method 'registerProgram'
        QtMocHelpers::MethodData<QString(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'updateProgram'
        QtMocHelpers::MethodData<QString(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'listPrograms'
        QtMocHelpers::MethodData<QString(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'getProgramByName'
        QtMocHelpers::MethodData<QString(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 19 },
        }}),
        // Method 'getProgramById'
        QtMocHelpers::MethodData<QString(const QString &)>(20, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 21 },
        }}),
        // Method 'uploadIdl'
        QtMocHelpers::MethodData<QString(const QString &)>(22, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'downloadIdl'
        QtMocHelpers::MethodData<QString(const QString &)>(23, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 24 },
        }}),
        // Method 'fetchIdlAsync'
        QtMocHelpers::MethodData<void(const QString &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
        // Method 'registerAsync'
        QtMocHelpers::MethodData<void(const QString &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LezRegistryModule, qt_meta_tag_ZN17LezRegistryModuleE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LezRegistryModule::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17LezRegistryModuleE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17LezRegistryModuleE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17LezRegistryModuleE_t>.metaTypes,
    nullptr
} };

void LezRegistryModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LezRegistryModule *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->eventResponse((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantList>>(_a[2]))); break;
        case 1: _t->idlFetched((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->programRegistered(); break;
        case 3: _t->registerFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->initLogos((*reinterpret_cast< std::add_pointer_t<LogosAPI*>>(_a[1]))); break;
        case 5: { QString _r = _t->registerProgram((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 6: { QString _r = _t->updateProgram((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 7: { QString _r = _t->listPrograms((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 8: { QString _r = _t->getProgramByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 9: { QString _r = _t->getProgramById((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 10: { QString _r = _t->uploadIdl((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 11: { QString _r = _t->downloadIdl((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->fetchIdlAsync((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->registerAsync((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (LezRegistryModule::*)(const QString & , const QVariantList & )>(_a, &LezRegistryModule::eventResponse, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (LezRegistryModule::*)(const QString & , const QString & )>(_a, &LezRegistryModule::idlFetched, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (LezRegistryModule::*)()>(_a, &LezRegistryModule::programRegistered, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (LezRegistryModule::*)(const QString & )>(_a, &LezRegistryModule::registerFailed, 3))
            return;
    }
}

const QMetaObject *LezRegistryModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LezRegistryModule::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17LezRegistryModuleE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "PluginInterface"))
        return static_cast< PluginInterface*>(this);
    if (!strcmp(_clname, "ILezRegistryModule"))
        return static_cast< ILezRegistryModule*>(this);
    if (!strcmp(_clname, "com.example.PluginInterface"))
        return static_cast< PluginInterface*>(this);
    return QObject::qt_metacast(_clname);
}

int LezRegistryModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void LezRegistryModule::eventResponse(const QString & _t1, const QVariantList & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void LezRegistryModule::idlFetched(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void LezRegistryModule::programRegistered()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void LezRegistryModule::registerFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

#ifdef QT_MOC_EXPORT_PLUGIN_V2
static constexpr unsigned char qt_pluginMetaDataV2_LezRegistryModule[] = {
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1c,  'o',  'r',  'g',  '.',  'l', 
    'o',  'g',  'o',  's',  '.',  'i',  'l',  'e', 
    'z',  'r',  'e',  'g',  'i',  's',  't',  'r', 
    'y',  'm',  'o',  'd',  'u',  'l',  'e', 
    // "className"
    0x03,  0x71,  'L',  'e',  'z',  'R',  'e',  'g', 
    'i',  's',  't',  'r',  'y',  'M',  'o',  'd', 
    'u',  'l',  'e', 
    // "MetaData"
    0x04,  0xa8,  0x66,  'a',  'u',  't',  'h',  'o', 
    'r',  0x6a,  'j',  'i',  'm',  'm',  'y',  '-', 
    'c',  'l',  'a',  'w',  0x6c,  'd',  'e',  'p', 
    'e',  'n',  'd',  'e',  'n',  'c',  'i',  'e', 
    's',  0x80,  0x6b,  'd',  'e',  's',  'c',  'r', 
    'i',  'p',  't',  'i',  'o',  'n',  0x78,  0x4e, 
    'L',  'E',  'Z',  ' ',  'P',  'r',  'o',  'g', 
    'r',  'a',  'm',  ' ',  'R',  'e',  'g',  'i', 
    's',  't',  'r',  'y',  ' ',  uchar('\xe2'), uchar('\x80'), uchar('\x94'),
    ' ',  'b',  'r',  'o',  'w',  's',  'e',  ',', 
    ' ',  'r',  'e',  'g',  'i',  's',  't',  'e', 
    'r',  ',',  ' ',  'a',  'n',  'd',  ' ',  'q', 
    'u',  'e',  'r',  'y',  ' ',  'o',  'n',  '-', 
    'c',  'h',  'a',  'i',  'n',  ' ',  'p',  'r', 
    'o',  'g',  'r',  'a',  'm',  ' ',  'm',  'e', 
    't',  'a',  'd',  'a',  't',  'a',  0x67,  'i', 
    'n',  'c',  'l',  'u',  'd',  'e',  0x82,  0x76, 
    'l',  'i',  'b',  'l',  'e',  'z',  '_',  'r', 
    'e',  'g',  'i',  's',  't',  'r',  'y',  '_', 
    'f',  'f',  'i',  '.',  's',  'o',  0x78,  0x19, 
    'l',  'i',  'b',  'l',  'e',  'z',  '_',  'r', 
    'e',  'g',  'i',  's',  't',  'r',  'y',  '_', 
    'f',  'f',  'i',  '.',  'd',  'y',  'l',  'i', 
    'b',  0x64,  'n',  'a',  'm',  'e',  0x76,  'l', 
    'i',  'b',  'l',  'e',  'z',  '_',  'r',  'e', 
    'g',  'i',  's',  't',  'r',  'y',  '_',  'm', 
    'o',  'd',  'u',  'l',  'e',  0x64,  't',  'y', 
    'p',  'e',  0x6a,  'b',  'l',  'o',  'c',  'k', 
    'c',  'h',  'a',  'i',  'n',  0x62,  'u',  'i', 
    0xa5,  0x69,  'c',  'o',  'm',  'p',  'o',  'n', 
    'e',  'n',  't',  0x6f,  'L',  'e',  'z',  'R', 
    'e',  'g',  'i',  's',  't',  'r',  'y',  'V', 
    'i',  'e',  'w',  0x71,  'c',  'o',  'n',  't', 
    'e',  'x',  't',  'P',  'r',  'o',  'p',  'e', 
    'r',  't',  'i',  'e',  's',  0x82,  0x70,  'l', 
    'e',  'z',  'R',  'e',  'g',  'i',  's',  't', 
    'r',  'y',  'M',  'o',  'd',  'e',  'l',  0x71, 
    'l',  'e',  'z',  'R',  'e',  'g',  'i',  's', 
    't',  'r',  'y',  'M',  'o',  'd',  'u',  'l', 
    'e',  0x64,  'i',  'c',  'o',  'n',  0x63,  uchar('\xe2'),
    uchar('\xac'), uchar('\xa1'), 0x66,  'q',  'm',  'l',  'U',  'r', 
    'l',  0x78,  0x25,  'q',  'r',  'c',  ':',  '/', 
    'l',  'e',  'z',  '-',  'r',  'e',  'g',  'i', 
    's',  't',  'r',  'y',  '/',  'L',  'e',  'z', 
    'R',  'e',  'g',  'i',  's',  't',  'r',  'y', 
    'V',  'i',  'e',  'w',  '.',  'q',  'm',  'l', 
    0x65,  't',  'i',  't',  'l',  'e',  0x6c,  'L', 
    'E',  'Z',  ' ',  'R',  'e',  'g',  'i',  's', 
    't',  'r',  'y',  0x67,  'v',  'e',  'r',  's', 
    'i',  'o',  'n',  0x65,  '0',  '.',  '1',  '.', 
    '0', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN_V2(LezRegistryModule, LezRegistryModule, qt_pluginMetaDataV2_LezRegistryModule)
#else
QT_PLUGIN_METADATA_SECTION
Q_CONSTINIT static constexpr unsigned char qt_pluginMetaData_LezRegistryModule[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1c,  'o',  'r',  'g',  '.',  'l', 
    'o',  'g',  'o',  's',  '.',  'i',  'l',  'e', 
    'z',  'r',  'e',  'g',  'i',  's',  't',  'r', 
    'y',  'm',  'o',  'd',  'u',  'l',  'e', 
    // "className"
    0x03,  0x71,  'L',  'e',  'z',  'R',  'e',  'g', 
    'i',  's',  't',  'r',  'y',  'M',  'o',  'd', 
    'u',  'l',  'e', 
    // "MetaData"
    0x04,  0xa8,  0x66,  'a',  'u',  't',  'h',  'o', 
    'r',  0x6a,  'j',  'i',  'm',  'm',  'y',  '-', 
    'c',  'l',  'a',  'w',  0x6c,  'd',  'e',  'p', 
    'e',  'n',  'd',  'e',  'n',  'c',  'i',  'e', 
    's',  0x80,  0x6b,  'd',  'e',  's',  'c',  'r', 
    'i',  'p',  't',  'i',  'o',  'n',  0x78,  0x4e, 
    'L',  'E',  'Z',  ' ',  'P',  'r',  'o',  'g', 
    'r',  'a',  'm',  ' ',  'R',  'e',  'g',  'i', 
    's',  't',  'r',  'y',  ' ',  uchar('\xe2'), uchar('\x80'), uchar('\x94'),
    ' ',  'b',  'r',  'o',  'w',  's',  'e',  ',', 
    ' ',  'r',  'e',  'g',  'i',  's',  't',  'e', 
    'r',  ',',  ' ',  'a',  'n',  'd',  ' ',  'q', 
    'u',  'e',  'r',  'y',  ' ',  'o',  'n',  '-', 
    'c',  'h',  'a',  'i',  'n',  ' ',  'p',  'r', 
    'o',  'g',  'r',  'a',  'm',  ' ',  'm',  'e', 
    't',  'a',  'd',  'a',  't',  'a',  0x67,  'i', 
    'n',  'c',  'l',  'u',  'd',  'e',  0x82,  0x76, 
    'l',  'i',  'b',  'l',  'e',  'z',  '_',  'r', 
    'e',  'g',  'i',  's',  't',  'r',  'y',  '_', 
    'f',  'f',  'i',  '.',  's',  'o',  0x78,  0x19, 
    'l',  'i',  'b',  'l',  'e',  'z',  '_',  'r', 
    'e',  'g',  'i',  's',  't',  'r',  'y',  '_', 
    'f',  'f',  'i',  '.',  'd',  'y',  'l',  'i', 
    'b',  0x64,  'n',  'a',  'm',  'e',  0x76,  'l', 
    'i',  'b',  'l',  'e',  'z',  '_',  'r',  'e', 
    'g',  'i',  's',  't',  'r',  'y',  '_',  'm', 
    'o',  'd',  'u',  'l',  'e',  0x64,  't',  'y', 
    'p',  'e',  0x6a,  'b',  'l',  'o',  'c',  'k', 
    'c',  'h',  'a',  'i',  'n',  0x62,  'u',  'i', 
    0xa5,  0x69,  'c',  'o',  'm',  'p',  'o',  'n', 
    'e',  'n',  't',  0x6f,  'L',  'e',  'z',  'R', 
    'e',  'g',  'i',  's',  't',  'r',  'y',  'V', 
    'i',  'e',  'w',  0x71,  'c',  'o',  'n',  't', 
    'e',  'x',  't',  'P',  'r',  'o',  'p',  'e', 
    'r',  't',  'i',  'e',  's',  0x82,  0x70,  'l', 
    'e',  'z',  'R',  'e',  'g',  'i',  's',  't', 
    'r',  'y',  'M',  'o',  'd',  'e',  'l',  0x71, 
    'l',  'e',  'z',  'R',  'e',  'g',  'i',  's', 
    't',  'r',  'y',  'M',  'o',  'd',  'u',  'l', 
    'e',  0x64,  'i',  'c',  'o',  'n',  0x63,  uchar('\xe2'),
    uchar('\xac'), uchar('\xa1'), 0x66,  'q',  'm',  'l',  'U',  'r', 
    'l',  0x78,  0x25,  'q',  'r',  'c',  ':',  '/', 
    'l',  'e',  'z',  '-',  'r',  'e',  'g',  'i', 
    's',  't',  'r',  'y',  '/',  'L',  'e',  'z', 
    'R',  'e',  'g',  'i',  's',  't',  'r',  'y', 
    'V',  'i',  'e',  'w',  '.',  'q',  'm',  'l', 
    0x65,  't',  'i',  't',  'l',  'e',  0x6c,  'L', 
    'E',  'Z',  ' ',  'R',  'e',  'g',  'i',  's', 
    't',  'r',  'y',  0x67,  'v',  'e',  'r',  's', 
    'i',  'o',  'n',  0x65,  '0',  '.',  '1',  '.', 
    '0', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(LezRegistryModule, LezRegistryModule)
#endif  // QT_MOC_EXPORT_PLUGIN_V2

QT_WARNING_POP
