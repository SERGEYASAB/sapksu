#ifndef PREFIXHEADER_H
#define PREFIXHEADER_H

#if defined __cplusplus
/* Add C++ includes here */

#define SAP_KSU // MECHOS // MECHOS  // SAP_SU
#define QML_DESIGN
#define IODEBUG

#define SQLITE_PATH "C:/Users/Operator/Downloads/sqlitestudio-3.1.1/SQLiteStudio/SQLiteStudio.exe"
#define ENERGOFORM_PATH "\"C:\\Program Files (x86)\\MarsEnergo\\EnergoForm\\Enform.exe"

//std
#include <memory>
#include <functional>
#include <type_traits>
#include <typeinfo>
#include <exception>
#include <limits>
#include <iostream>
#include <vector>
#include <chrono>

//qt
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtXml>
#include <QtNetwork>
#include <QtSerialPort>
#include <QtSql>
#include <QtQml>
#include <QtScript>
#include <QtScriptTools>
#include <QScriptEngineDebugger>

//sap ksu
#include "Staticconstvalues.h"
#include "CommonTemplates.h"
#include "PushButtonPlus.h"
#include "ThreadWorkingTemplates.h"
#include "CommonEnums.h"


//platform files
#ifdef Q_OS_WIN
#include <windows.h>
#endif // Q_OS_WIN


// Псевдонимы для умных указателей

template<typename T>
using SUPtr = std::unique_ptr<T>;

template<typename T>
using SSPtr = std::shared_ptr<T>;

template<typename T>
using SWPtr = std::weak_ptr<T>;

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr)
Q_DECLARE_SMART_POINTER_METATYPE(std::unique_ptr)
Q_DECLARE_SMART_POINTER_METATYPE(std::weak_ptr)


#endif // defined __cplusplus

#endif // PREFIXHEADER_H
