/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "CosuService.h"
#include "CosuSettings.h"

#include <QCommandLineParser>
#include <QCoreApplication>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    bool dbusService = true;

    QCommandLineParser parser;
    parser.addOptions(
        {{"no-dbus", "Don't initiate DBus Service, run once and exit"}});

    parser.process(app);

    if (parser.isSet(QStringLiteral("no-dbus"))) {
        dbusService = false;
    }

    CosuService *upd =
        new CosuService(dbusService, QCoreApplication::instance());
    QObject::connect(upd, &CosuService::done, qApp, &QCoreApplication::quit);
    return app.exec();
}
