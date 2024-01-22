/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "../src/CosuService.h"
#include "../src/CosuSettings.h"
#include "config.h"
#if packagekitqt5_FOUND
#include "CosuPackageKit.h"
#endif
#if ZYPP_FOUND
#include "CosuZypp.h"
#endif
#include <memory>

#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <QVersionNumber>

class testCosuService : public QObject
{
    Q_OBJECT
public:
public slots:
    void onOsUpdatePackageFound(CosuPackage &package);
    void onOsUpdate(const QVersionNumber &version);
private Q_SLOTS:
    void initTestCase();
    void testCase();

private:
    std::unique_ptr<CosuService> m_service;
};

void testCosuService::initTestCase()
{
    qRegisterMetaType<CosuPackage>("CosuPackage");

    CosuSettings::setTestDirectories({/* path relative to ./build directory
                                         relative to project root */
                                      QFINDTESTDATA("../../data/conf.d/."),
                                      QFINDTESTDATA("./data/serviceData")});

    m_service = std::unique_ptr<CosuService>(
        new CosuService(QCoreApplication::instance()));

#if ZYPP_FOUND
    connect(m_service.get()->m_zypp.get(),
#else
    connect(m_service.get()->m_packagekit.get(),
#endif
            &CosuResolveAbstract::resolvedPackage,
            this,
            &testCosuService::onOsUpdatePackageFound);

    connect(m_service.get(),
            &CosuService::osUpdateReceived,
            this,
            &testCosuService::onOsUpdate);
    // QCoreApplication::processEvents();
}

void testCosuService::testCase()
{
#if ZYPP_FOUND
    QSignalSpy spy(m_service.get()->m_zypp.get(),
#else
    QSignalSpy spy(m_service.get()->m_packagekit.get(),
#endif
                   &CosuResolveAbstract::resolvedPackage);
    m_service->checkForOsUpdate();
    spy.wait(6000000);
    QCOMPARE(spy.count(), 1);
    spy.dumpObjectInfo();
}

void testCosuService::onOsUpdatePackageFound(CosuPackage &package)
{
    qDebug() << QStringLiteral("Got package %1: %2")
                    .arg(package.name)
                    .arg(package.version);
    /*
      Call slot from class to be tested, not sure if it should be this way.
     */
    m_service->onOsUpdatePackageFound(package);
}

void testCosuService::onOsUpdate(const QVersionNumber &version)
{
    qDebug() << QStringLiteral("Got version: %1").arg(version.toString());
}

QTEST_MAIN(testCosuService)

#include "testCosuService.moc"
