/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 * SPDX-FileCopyrightText: 2013 Jolla Ltd Bernd Wachter <bwachter@lart.info>
 *
 * Based on ssu ssuvarialbes_p.h
 */

#pragma once
#include <QHash>
#include <QObject>

#include "CosuSettings.h"

class CosuVariables : public QObject
{
    Q_OBJECT

public:
    CosuVariables(QObject *parent = nullptr);
    /**
     * Return a default variable section, if exists, or an empty string.
     *
     * To identify the default section the section name gets split at '-', and
     * the first token (or second token for "var-" sections) replaced with
     * "default". You should therefore avoid "-" in section names.
     */
    static QString defaultSection(CosuSettings *settings,
                                  const QString &section);
    QString defaultSection(const QString &section);

    /**
     * Resolve a whole string, containing several variables. Variables inside
     * variables are allowed
     */
    static QString resolveString(const QString &pattern,
                                 QHash<QString, QVariant> *variables,
                                 int recursionDepth = 0);
    /**
     * Resolve variables; variable can be passed as %(var) or var
     */
    static QString resolveVariable(const QString &variable,
                                   QHash<QString, QVariant> *variables);
    /**
     * Set the settings object to use
     */
    void setSettings(CosuSettings *settings);
    /*
     * Return the settings object used
     */
    CosuSettings *settings();
    /**
     * Return a variable from the given variable section. 'var'- is
     * automatically prepended to the section name if not specified already.
     * Recursive search through several variable sections (specified in the
     * section) is supported, returned will be the last occurence of the
     * variable.
     */
    QVariant variable(const QString &section, const QString &key);
    static QVariant variable(CosuSettings *settings,
                             const QString &section,
                             const QString &key);
    /**
     * Return the requested variable section, recursively looking up all
     * variable sections referenced inside with the 'variable' keyword. 'var-'
     * is automatically prepended to the section names of included variable
     * sections, but not for the parent section.
     *
     * Variables starting with _ are treated as local variables and are ignored.
     * The special key 'local' may contain a section-specific stringlist with
     * additional keys which should be treated local.
     *
     * This function tries to identify a default configuration section, and
     * merges the default section with the requested section.
     */
    void variableSection(const QString &section,
                         QHash<QString, QVariant> *storageHash);
    static void variableSection(CosuSettings *settings,
                                const QString &section,
                                QHash<QString, QVariant> *storageHash);

private:
    static void resolveSection(CosuSettings *settings,
                               const QString &section,
                               QHash<QString, QVariant> *storageHash,
                               int recursionDepth,
                               bool logOverride = true);
    static QVariant readVariable(CosuSettings *settings,
                                 const QString &section,
                                 const QString &key,
                                 int recursionDepth,
                                 bool logOverride = true);
    CosuSettings *m_settings;
};
