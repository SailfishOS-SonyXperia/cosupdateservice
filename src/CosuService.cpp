/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "CosuService.h"
#if packagekitqt5_FOUND
#include "CosuPackageKit.h"
#endif
#if ZYPP_FOUND
#include "CosuZypp.h"
#endif
#include "CosuOsUpdateServiceInfo.h"
#include "CosuServiceRuntime.h"
#include "cosu_debug.h"
#include "cosu_debug_service.h"
#include "cosupdateradaptor.h"
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QDebug>
#include <QDir>
#include <QVersionNumber>

#define COSU_TEST_DEVICENAME qgetenv("COSU_TEST_DEVICENAME")

CosuService::CosuService(QObject *parent)
    : CosuService(true, parent)
{
}

CosuService::CosuService(bool DBusService, QObject *parent)
    : QObject(parent)
{
#ifdef QT_TESTLIB_LIB
    m_runtime = std::unique_ptr<CosuServiceRuntime>(new CosuServiceRuntime(
        qEnvironmentVariableIsSet(COSU_TEST_DEVICENAME) ? COSU_TEST_DEVICENAME
                                                        : nullptr,
        parent));
#else
    m_runtime =
        std::unique_ptr<CosuServiceRuntime>(new CosuServiceRuntime(parent));
#endif
    m_packagekit = std::unique_ptr<CosuPackageKit>(new CosuPackageKit(parent));
    m_zypp = std::unique_ptr<CosuZypp>(new CosuZypp(parent));

    connect(this,
            &CosuService::done,
            QCoreApplication::instance(),
            &QCoreApplication::quit);

    if (!this->initializeSettings()) {
        // QCoreApplication::exit(EXIT_FAILURE); doesn't exit
        exit(EXIT_FAILURE);
        return;
    }

    connect(this,
            &CosuService::osUpdateReceived,
            this,
            &CosuService::onOsUpdateReceived);

    this->m_idleTimer.setSingleShot(true);
    connect(&this->m_idleTimer,
            &QTimer::timeout,
            QCoreApplication::instance(),
            &QCoreApplication::quit);
    this->m_idleTimer.start(1000 * 60);

    if (DBusService) {
        QDBusConnection sessionBus = QDBusConnection::sessionBus();
        if (!sessionBus.isConnected()) {
            qCCritical(COSU_SERVICE)
                << "Couldn't connect to sessionbus, exiting";
            exit(EXIT_FAILURE);
        }
        new CosUpdaterAdaptor(this);
        sessionBus.registerObject(COSU_DBUS_PATH, this);
        sessionBus.registerService(COSU_DBUS_SERVICENAME);
    } else {
        this->checkForOsUpdate();
    }
}

CosuService::~CosuService()
{
}

bool CosuService::initializeSettings()
{
    bool sanityCheckOK = true;
    // check if variables and settings are present, do sanity check
    for (const QString &section : m_runtime->sections()) {
        const QStringList sanityVariables =
            m_runtime->value(section, "sanityVariables").toStringList();
        for (const QString &sanityVariable : sanityVariables) {
            if (!m_runtime->section(section).contains(sanityVariable)) {
                qCCritical(COSU)
                    << QStringLiteral(
                           "Required variable in section %1, %2 not "
                           "defined")
                           .arg(section)
                           .arg(sanityVariable);
                sanityCheckOK = false;
            }
        }
    }
    return sanityCheckOK;
}

void CosuService::setActivity(Activity act)
{
    if (act != m_activity) {
        m_activity = act;
        emit isActiveChanged();
    }
    if (act != Activity::Idle)
        m_idleTimer.stop();
    else
        m_idleTimer.start();
}

void CosuService::onResolverError(const QString &errorMessage)
{
    qDebug() << errorMessage;
}

void CosuService::checkForOsUpdate(bool force)
{
    qCInfo(COSU_SERVICE) << "Checking for OS updates";
    qCInfo(COSU_SERVICE) << "Refreshing cache first " << force;
    this->setActivity(Activity::RefreshCache);
#if packagekitqt5_FOUND
    connect(m_packagekit.get(),
            &CosuPackageKit::refreshFinished,
            this,
            &CosuService::onRefreshFinished);
    m_packagekit->refreshCache(force);
#else
#if ZYPP_FOUND
#warning "libzypp doesn't suport refresh at the moment"
    connect(m_zypp.get(),
            &CosuZypp::refreshFinished,
            this,
            &CosuService::onRefreshFinished)
#endif
#endif
}

QVersionNumber CosuService::extractVersion(const QString &version)
{
    /*
      Retrieve version pattern for version package that contains
      the next os release.

      E.g.
      %(version)%(version_sepator)%(release-counter) ->
      4.4.0.72+git1
    */
    const QString version_separator =
        m_runtime->variale("adaptation", "%(versionPackageSeparator)")
            .toString();
    int suffixIndex;
    const QVersionNumber newOsVersion =
        QVersionNumber::fromString(version.split(version_separator).first(),
                                   &suffixIndex);
    if (newOsVersion.isNull())
        qCWarning(COSU_SERVICE)
            << "newOsVersion empty, adaptation/%(versionPackageSeparator) not "
               "set?";
    return newOsVersion;
}

void CosuService::findOsUpdatePackage()
{
    /*
      Retrieve package that matches the pattern set that the
      os update package should be called:
      %(osUpdatePackageName)-%(deviceVariant)
    */
    const QString versionPackageName =
        m_runtime->variale("adaptation", "%(osupdate-package-pattern)")
            .toString();
    qCDebug(COSU_SERVICE) << "Got version package name" << versionPackageName;
#if ZYPP_FOUND
    if (m_runtime->section("site").contains("osUpdatePackageRepo"))
        m_zypp->setRepositoryAlias(
            m_runtime->variale("site", "%(osUpdatePackageRepo)").toString());
    connect(m_zypp.get(),
            &CosuZypp::resolvedPackage,
            this,
            &CosuService::onOsUpdatePackageFound);
    m_zypp->resolvePackage(versionPackageName);
#else
    connect(m_packagekit.get(),
            &CosuPackageKit::resolvedPackage,
            this,
            &CosuService::onOsUpdatePackageFound);
    m_packagekit->resolvePackage(versionPackageName);
#endif
}

void CosuService::onRefreshFinished()
{
    this->setActivity(Activity::FindOsUpdatePackage);
    this->findOsUpdatePackage();
}

void CosuService::onOsUpdatePackageFound(CosuPackage package)
{
    qDebug() << QString("Got Package %1, version %2")
                    .arg(package.name)
                    .arg(package.version);
    int suffixIndex;
    const QVersionNumber oldOsVersion = QVersionNumber::fromString(
        m_runtime->value("runtime", "os-release").toString(),
        &suffixIndex);
    const QVersionNumber newOsVersion = extractVersion(package.version);
    if (newOsVersion <= oldOsVersion) {
        qCInfo(COSU_SERVICE) << "Latest OS release already " << newOsVersion
                             << "installed, skipping.";
        emit done();
        return;
    }
    emit osUpdateReceived(newOsVersion);
}

void CosuService::onOsUpdateReceived(const QVersionNumber &version)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral(SAILFISHOS_UPDATER_SERVICE),
        QStringLiteral(SAILFISHOS_UPDATER_PATH),
        QStringLiteral(SAILFISHOS_UPDATER_INTERFACE),
        QStringLiteral(SAILFISHOS_UPDATER_METHOD));
    message << version.toString();
    QDBusMessage responseMessage = QDBusConnection::sessionBus().call(message);
    qCDebug(COSU_SERVICE) << "DBus response messsage: " << responseMessage;
    setActivity(Activity::Idle);
    emit done();
    return;
}
