/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "CosuServiceRuntime.h"

#include "CosuSettings.h"
#include "CosuVariables.h"
#include "ssu.h"
#include "ssudeviceinfo.h"

#define COSU_TEST_DEVICENAME qgetenv("COSU_TEST_DEVICENAME")

CosuServiceRuntime::CosuServiceRuntime(QObject *parent)
    : CosuServiceRuntime::CosuServiceRuntime(nullptr, parent)
{
}

const QString CosuServiceRuntime::maybeEmpty(const QString &_maybeEmpty)
{
    return _maybeEmpty == QLatin1String("") ? nullptr : _maybeEmpty;
}

const QString CosuServiceRuntime::maybeDefaultSection(const QString &section)
{
    if (!m_settingsHash.contains(section))
        return m_variables->defaultSection(section);
    return section;
}

CosuServiceRuntime::CosuServiceRuntime(const QString &deviceName,
                                       QObject *parent)
{
    /*
      [section1]
      variableA=value
      variableb=variableA
      [section2]
      variableA=section1/variableA

     */

    m_variables = std::unique_ptr<CosuVariables>(new CosuVariables(parent));
#ifndef QT_TESTLIB_LIB
    m_settings = std::unique_ptr<CosuSettings>(new CosuSettings(parent));
    SsuDeviceInfo deviceInfo(qEnvironmentVariableIsSet(COSU_TEST_DEVICENAME)
                                 ? COSU_TEST_DEVICENAME
                                 : deviceName);
#else
    m_settings = std::unique_ptr<CosuSettings>(
        new CosuSettings(QStringList(deviceName), parent));
    Q_UNUSED(deviceName)
#endif
    m_variables->setSettings(m_settings.get());

    for (const QString &childGroup : m_settings->childGroups()) {
        m_settings->beginGroup(childGroup);
        for (QString &key : m_settings->allKeys()) {
            m_settingsHash[childGroup].insert(key, m_settings->value(key));
        }
        m_settings->endGroup();
    }
#if 0
    for (const QString &section : m_settingsHash.keys()) {
        for (const QString &sectionKey : m_settingsHash[section].keys()) {
            QString value =
                m_variables->resolveString(m_settingsHash[section][sectionKey],
                                           &m_settingsHash[section]);
            if (value != "")
                m_settingsHash[section][sectionKey] = value;
        }
    }
#endif
#ifndef QT_TESTLIB_LIB
    const QString repoName = QStringLiteral("adaptatation0");
    QHash<QString, QString> adaptationVariables;
    deviceInfo.adaptationVariables(repoName, &adaptationVariables);
    for (const QString &adaptationVariable : adaptationVariables.keys()) {
        m_settingsHash["adaptation"].insert(
            adaptationVariable,
            static_cast<QVariant>(adaptationVariables[adaptationVariable]));
    }
    m_settingsHash["adaptation"]["deviceModel"] = deviceInfo.deviceModel();
    m_settingsHash["adaptation"]["deviceVariant"] = deviceInfo.deviceVariant();
    m_settingsHash["runtime"]["os-release"] = Ssu().release();
#endif
}

const QVariant CosuServiceRuntime::value(const QString &section,
                                         const QString &key)
{
    return m_settingsHash[section].value(key, QVariant());
}

const QVariant CosuServiceRuntime::variale(const QString &section,
                                           const QString &key)
{
    return m_variables->resolveString(
        key,
        &m_settingsHash[maybeDefaultSection(section)]);
}

const QStringList CosuServiceRuntime::sections()
{
    return m_settingsHash.keys();
}

const QHash<QString, QVariant> CosuServiceRuntime::section(const QString &key)
{
    return m_settingsHash[maybeDefaultSection(key)];
}

void CosuServiceRuntime::insert(const QString &section,
                                const QString &key,
                                const QVariant &value)
{
    m_settingsHash[section][key] = value;
}
