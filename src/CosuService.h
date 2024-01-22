/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#pragma once

#include "CosuResolveAbstract.h"
#include "config.h"
#include <QDBusContext>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QVersionNumber>
#include <memory>

#define COSU_DBUS_INTERFACE "org.sailfishos.CosUpdater"

class CosuServiceRuntime;
class CosuPackageKit;
class CosuZypp;

class CosuService : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", COSU_DBUS_INTERFACE)

public:
    CosuService(QObject *parent = nullptr);
    CosuService(bool DBusService = false, QObject *parent = nullptr);
    CosuService(QStringList testData,
                bool DBusService = false,
                QObject *parent = nullptr);

    ~CosuService();

    enum Activity { Idle, RefreshCache, FindOsUpdatePackage };
    Q_ENUM(Activity)

    Q_SCRIPTABLE void checkForOsUpdate(bool force = false);

    /**
     * @return status messsage conveying the action being currently performed
     */
    QString statusMessage() const;

signals:
    /*
      0. Refresh
      1. Finished
      2. onFinished
      3. onRefreshFinished
      4. findOsUpdatePackage
      5. Transaction search for old pkg
      6. Finished
      7. onOsUpdatePackageFound
      8. set current os version
      10. setActivity(CheckNewOsVersion)
      11. findosUpdatePackage()
      12. Transaction search for new pkg
      13. Finished
      14. osUpdatePackageFound
      15. compare versions
      16. if newer emit osUpdateReceived
      17. onOsUpdateReceived
      18. cleanup?
     */
    void refreshFinished();
    void osUpdatePackageFound();
    Q_SCRIPTABLE void osUpdateReceived(const QVersionNumber &version);

    void isActiveChanged();
    void networkStateChanged();

    void done();

public slots:
    void onOsUpdateReceived(const QVersionNumber &version);

private slots:
    void onRefreshFinished();
    void onResolverError(const QString &errorMessage);
    void onOsUpdatePackageFound(CosuPackage package);

private:
    void setActivity(Activity act);
    bool isNetworkOnline() const;
    QVersionNumber extractVersion(const QString &version);
    void findOsUpdatePackage();
    bool initializeSettings();

    Activity m_activity = Idle; //?
    QTimer m_idleTimer;
    std::unique_ptr<CosuServiceRuntime> m_runtime;
#if packagekitqt5_FOUND
    std::unique_ptr<CosuPackageKit> m_packagekit;
#endif
#if ZYPP_FOUND
    std::unique_ptr<CosuZypp> m_zypp;
#endif
#ifdef QT_TESTLIB_LIB
    friend class testCosuService;
#endif
};
