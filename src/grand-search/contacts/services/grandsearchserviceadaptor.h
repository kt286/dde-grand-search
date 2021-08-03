/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     wangchunlin<wangchunlin@uniontech.com>
 *
 * Maintainer: wangchunlin<wangchunlin@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp com.deepin.dde.GrandSearch.xml -i grandsearchservice.h -a grandsearchserviceadaptor -c GrandSearchServiceAdaptor
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef GRANDSEARCHSERVICEADAPTOR_H
#define GRANDSEARCHSERVICEADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "grandsearchservice.h"
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.deepin.dde.GrandSearch
 */
class GrandSearchServiceAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.dde.GrandSearch")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.deepin.dde.GrandSearch\">\n"
"    <signal name=\"VisibleChanged\">\n"
"      <arg direction=\"out\" type=\"b\" name=\"vidible\"/>\n"
"    </signal>\n"
"    <method name=\"IsVisible\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"SetVisible\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"visible\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    GrandSearchServiceAdaptor(QObject *parent);
    virtual ~GrandSearchServiceAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    bool IsVisible();
    void SetVisible(bool visible);
Q_SIGNALS: // SIGNALS
    void VisibleChanged(bool vidible);
};

#endif