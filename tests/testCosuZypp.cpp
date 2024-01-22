/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "../src/CosuResolveAbstract.h"
#include "../src/CosuZypp.h"

#include <memory>

#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

class testCosuZypp : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void refreshCache();
    void resolvePackage();
    void onResolvedPackage(CosuPackage &package);
};

void testCosuZypp::refreshCache()
{
    std::unique_ptr<CosuZypp> cosuZypp(new CosuZypp(this->parent()));
}

void testCosuZypp::resolvePackage()
{
    struct CosuPackage testPackage = {.name = "bash", .version = "1.1"};
    std::unique_ptr<CosuZypp> cosuZypp(new CosuZypp(this->parent()));
    qRegisterMetaType<CosuPackage>("CosuPackage");
    QSignalSpy spy(cosuZypp.get(), SIGNAL(resolvedPackage(CosuPackage)));
    connect(cosuZypp.get(),
            &CosuZypp::resolvedPackage,
            this,
            &testCosuZypp::onResolvedPackage);
    const QString testPackageName = "bash";
    cosuZypp->resolvePackage(testPackageName);
    spy.dumpObjectInfo();
    spy.dumpObjectTree();
}

void testCosuZypp::onResolvedPackage(CosuPackage &package)
{
    qDebug() << package.name;
    qDebug() << package.version;
}

QTEST_MAIN(testCosuZypp)

#include "testCosuZypp.moc"
