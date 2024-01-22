/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "config.h"
#include <CosuOsUpdateServiceProxy.h>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addOptions({{"trigger", "Trigger update search"}});

    parser.process(app);
    if (parser.isSet("trigger")) {
        CosuOsUpdateServiceProxy::triggerService();
        return EXIT_SUCCESS;
    }
    CosuOsUpdateServiceProxy *proxyService =
        new CosuOsUpdateServiceProxy(QCoreApplication::instance());
    return app.exec();
}
