/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "CosuZypp.h"
#include "cosu_debug_zypp.h"

#include <QDebug>
#include <QObject>
#include <zypp/PoolQuery.h>
#include <zypp/RepoManager.h>
#include <zypp/ZYppFactory.h>
#include <zypp/sat/SolvAttr.h>
#include <zypp/target/rpm/RpmDb.h>

CosuZypp::CosuZypp(QObject *parent)
    : CosuResolveAbstract(parent)
{
    // Initialize SSU target
    zypp::ZYppFactory::instance().getZYpp()->initializeTarget("/");
}

CosuZypp::~CosuZypp()
{
}

void CosuZypp::setRepositoryAlias(const QString &alias)
{
    m_repositoryAlias = alias;
}

void CosuZypp::resolvePackage(const QString &packageName)
{
    zypp::ZYpp::Ptr zypp = zypp::getZYpp();
    zypp->initializeTarget("/");
    zypp->getTarget()->load();

    zypp::RepoManager repoManager;

    for (zypp::RepoInfo &nrepo : repoManager.knownRepositories()) {
        if (!nrepo.enabled()) {
            qCDebug(COSU_ZYPP)
                << "Repository" << QString::fromStdString(nrepo.asUserString())
                << " disabled skipping";
            continue;
        }

        // Often volatile media are sipped in automated environments
        // to avoid media chagne requests:
        if (nrepo.url().schemeIsVolatile())
            continue;

        repoManager.loadFromCache(nrepo);
    }
    // qCDebug(COSU_ZYPP) << "Pool: " << QString::fromStdString(zypp->pool());

    m_poolQuery = std::unique_ptr<zypp::PoolQuery>(new zypp::PoolQuery);

    CosuPackage package;
    m_poolQuery->setMatchExact();
    /*
      Always include the system "repo" so that in case there will be package
      that is returned even when there is none in the target repository to
      compare against.
    */
    m_poolQuery->addRepo("@System");
    if (m_repositoryAlias != nullptr) {
        m_poolQuery->addRepo(m_repositoryAlias.toStdString());
        qCDebug(COSU_ZYPP) << "Got repository " << m_repositoryAlias
                           << "as search repository";
    }
    m_poolQuery->addKind(zypp::ResKind::package);
    m_poolQuery->addAttribute(zypp::sat::SolvAttr::name,
                              packageName.toStdString());
    zypp::sat::Solvable oAttrItter;

    for (auto attrIter : *m_poolQuery) {
        qCDebug(COSU_ZYPP) << "attrIter: "
                           << QString::fromStdString(attrIter.asString());
        if (oAttrItter.edition().version() < attrIter.edition().version()) {
            package.version =
                QString::fromStdString(attrIter.edition().version());
            package.name = QString::fromStdString(attrIter.name());
        }
        oAttrItter = attrIter;
    }
    emit resolvedPackage(package);
}

void CosuZypp::refreshCache(bool force)
{
    Q_UNUSED(force)
#if 0
    zypp::RepoManager manager;
    const std::list<zypp::RepoInfo> &repos(manager.knownRepositories());
    return;
#endif
}
