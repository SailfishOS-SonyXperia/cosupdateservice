/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "config.h"
#include <CosuOsUpdateServiceProxy.h>
#include <QCoreApplication>
#include <QDebug>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    CosuOsUpdateServiceProxy *proxyService =
        new CosuOsUpdateServiceProxy(QCoreApplication::instance());
    return app.exec();
}
