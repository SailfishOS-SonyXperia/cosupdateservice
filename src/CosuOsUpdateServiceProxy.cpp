/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "CosuOsUpdateServiceProxy.h"
#include "CosuOsUpdateServiceInfo.h"
#include "config.h"
#include "cosu_debug_proxy.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>

CosuOsUpdateServiceProxy::CosuOsUpdateServiceProxy(QObject *parent)
    : QObject(parent)
{
    auto sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.isConnected()) {
        qCCritical(COSU_PROXY) << "Couldn't connect to sessionbus, exiting";
        exit(EXIT_FAILURE);
    }

    if (!sessionBus.connect(SAILFISHOS_UPDATER_SERVICE,
                            SAILFISHOS_UPDATER_PATH,
                            SAILFISHOS_UPDATER_INTERFACE,
                            SAILFISHOS_UPDATER_CHANGED_SIGNAL,
                            this,
                            SLOT(onOsUpdateStatusChanged(int)))) {
        qCCritical(COSU_PROXY) << "Couldn't connect to "
                               << SAILFISHOS_UPDATER_SERVICE << " , exiting";
        exit(EXIT_FAILURE);
    }
}

void CosuOsUpdateServiceProxy::onOsUpdateStatusChanged(int status)
{
    ServiceStatus serviceStatus = static_cast<ServiceStatus>(status);

    switch (serviceStatus) {
    case ServiceStatus::Checking:
        break;
    case ServiceStatus::Updated:
        if (previousStatus == ServiceStatus::Checking)
            this->triggerService();
        break;
    case ServiceStatus::UpdateAvialable:
        break;
    }

    this->previousStatus = serviceStatus;
}

void CosuOsUpdateServiceProxy::triggerService()
{
    auto message = QDBusMessage::createMethodCall(COSU_DBUS_SERVICENAME,
                                                  COSU_DBUS_PATH,
                                                  COSU_DBUS_INTERFACE,
                                                  "checkForOsUpdate");

    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    if (response.type() == QDBusMessage::ErrorMessage)
        qDebug() << response.errorMessage();
}
