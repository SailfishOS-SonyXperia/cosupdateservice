/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#pragma once

#include "CosuVariables.h"

#include <QHash>
#include <QObject>
#include <memory>

class CosuSettings;

class CosuServiceRuntime : public QObject
{
    Q_OBJECT

public:
    CosuServiceRuntime(QObject *parent = nullptr);
    CosuServiceRuntime(const QString &deviceName, QObject *parent = nullptr);
    virtual ~CosuServiceRuntime() = default;

    const QVariant variale(const QString &section, const QString &key);
    const QVariant value(const QString &section, const QString &key);
    const QStringList sections();
    const QHash<QString, QVariant> section(const QString &key);
    void
    insert(const QString &section, const QString &key, const QVariant &value);

private:
    const QString maybeEmpty(const QString &_maybeEmpty);
    const QString maybeDefaultSection(const QString &section);

    std::unique_ptr<CosuSettings> m_settings;
    std::unique_ptr<CosuVariables> m_variables;
    QHash<QString, QHash<QString, QVariant>> m_settingsHash;
};
