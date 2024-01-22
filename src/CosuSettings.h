/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#pragma once
#include <QObject>
#include <QSettings>

class QSettings;
class SsuDeviceInfo;

class CosuSettings : public QSettings
{
    Q_OBJECT

public:
    CosuSettings(QObject *parent = nullptr);
    CosuSettings(QStringList configurationDirectories,
                 QObject *parent = nullptr);
    ~CosuSettings() override;

    QString updateSourceFeature();
    QString osUpdatePackage();

    void sync() = delete;
#ifdef QT_TESTLIB_LIB
    static void setTestDirectories(QStringList directories);
#endif

private:
    void loadSettings();
#if 0
    void verifySettings();
#endif
    QStringList m_configurationDirectories;
    QSettings *m_settings;
};
