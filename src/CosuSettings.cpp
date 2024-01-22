/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "config.h"
#include <QDebug>

#include <QCollator>
#include <QDir>
#include <QDirIterator>
#include <QSettings>

#include "CosuSettings.h"

#ifdef QT_TESTLIB_LIB
static QStringList cosuTestDirectories;
#endif

CosuSettings::CosuSettings(QObject *parent)
    : CosuSettings({COSU_CONF_DIRECTORIES}, parent)
{
}

CosuSettings::CosuSettings(QStringList configurationDirectories,
                           QObject *parent)
    : QSettings(QStringLiteral("/dev/null"),
                QSettings::IniFormat,
                parent) // FIXME, QSettings wants to save their settings
                        // We should provide a custom reader that doesn't
                        // provide write functions
{
#ifdef QT_TESTLIB_LIB
    if (configurationDirectories.first() == QLatin1String(""))
        configurationDirectories = cosuTestDirectories;
#endif
    m_configurationDirectories = configurationDirectories;
    this->loadSettings();
}

CosuSettings::~CosuSettings()
{
}

#ifdef QT_TESTLIB_LIB
void CosuSettings::setTestDirectories(QStringList directories)
{
    cosuTestDirectories = directories;
}
#endif

void CosuSettings::loadSettings()
{
    QStringList settingsFiles;

    for (const QString &directory : m_configurationDirectories) {
        QDir qDiretory(directory);
        qDiretory.setFilter(QDir::AllEntries | QDir::NoDot | QDir::NoDotDot);
        qDiretory.setSorting(QDir::NoSort);
        QStringList entryList = qDiretory.entryList();
        QCollator collator;
        collator.setNumericMode(true);
        std::sort(entryList.begin(), entryList.end(), collator);
        for (const QString &entry : entryList)
            settingsFiles.append(qDiretory.absoluteFilePath(entry));
    }
    for (const QString &settingsFile : settingsFiles) {
        QSettings settings(settingsFile, QSettings::IniFormat);

        for (const QString &childGroup : settings.childGroups()) {
            this->beginGroup(childGroup);
            settings.beginGroup(childGroup);

            for (const QString &key : settings.allKeys()) {
                this->setValue(key, settings.value(key));
            }
            settings.endGroup();
            this->endGroup();
        }
    }
}

#if 0

void CosuSettings::verifySettings()
{
    for (const QString &key : m_settings->allKeys())
    {
    }
}

#endif
