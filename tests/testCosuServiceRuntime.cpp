/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "../src/CosuServiceRuntime.h"
#include "../src/CosuSettings.h"
#include "../src/CosuVariables.h"
#include <QDebug>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QTest>

class CosuServiceRuntimeTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void verifyServiceRuntimeLoaded();
    void verifyServiceRuntimeLoaded_data();

private:
    // void verifyServiceRuntime_base(QMap<QString, QString> testDataList);
};

// void CosuServiceRuntimeTest::verifyServiceRuntime_base(
//     QMap<QString, QString> testDataList)
// {
//     QString actualDataPaths = testDataList["actual"];
//     std::unique_ptr<CosuServiceRuntime> actualServiceRuntime(
//         new CosuServiceRuntime(actualDataPaths,
//         QCoreApplication::instance()));
//     const QStringList actualKeysList = actualServiceRuntime->sections();
//     const QSet<QString> actualKeys(actualKeysList.constBegin(),
//                                    actualKeysList.constEnd());

//     QSettings expectedSettings(testDataList["expected"],
//     QSettings::IniFormat); const QStringList expectedKeyList =
//     expectedSettings.allKeys(); const QSet<QString>
//     expectedKeys(expectedKeyList.constBegin(),
//                                      expectedKeyList.constEnd());

//     for (const QString &section : actualServiceRuntime->sections()) {
//         qDebug() << "section: " << section;
//         expectedSettings.beginGroup(section);
//         for (const QString &sectionKey :
//              actualServiceRuntime->section(section).keys()) {
//             qDebug() << "actual: " <<
//             actualServiceRuntime->section(section).value(sectionKey);
//             qDebug() << "expected:" << expectedSettings.value(sectionKey);
//         }
//         expectedSettings.endGroup();
//     }

//     QTest::addColumn<QString>("actualValue");
//     QTest::addColumn<QString>("expectedValue");

//     for (const QString &key : actualKeys + expectedKeys) {
//         expectedSettings.beginGroup(key);

//         QTest::newRow(qPrintable(key))
//             << actualServiceRuntime->value(key).toString()
//             << expectedSettings.value(key).toString();
//     }
// }

void CosuServiceRuntimeTest::verifyServiceRuntimeLoaded_data()
{
    QMap<QString, QString> testDataList = {
        {"actual", {QFINDTESTDATA("./data/actual/verifyServiceRuntime")}},
        {"expected",
         QFINDTESTDATA("./data/expected/verifyServiceRuntime_expected.ini")}};

    QString actualDataPaths = testDataList["actual"];
    std::unique_ptr<CosuServiceRuntime> actualServiceRuntime(
        new CosuServiceRuntime(actualDataPaths, QCoreApplication::instance()));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    const QStringList actualKeysList = actualServiceRuntime->sections();
    const QSet<QString> actualKeys(actualKeysList.constBegin(),
                                   actualKeysList.constEnd());
#else
    const QSet<QString> actualKeys = actualServiceRuntime->sections().toSet();
#endif
    QSettings expectedSettings(testDataList["expected"], QSettings::IniFormat);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    const QStringList expectedKeyList = expectedSettings.allKeys();
    const QSet<QString> expectedKeys(expectedKeyList.constBegin(),
                                     expectedKeyList.constEnd());
#else
    const QSet<QString> expectedKeys = expectedSettings.allKeys().toSet();
#endif
    QTest::addColumn<QString>("actualValue");
    QTest::addColumn<QString>("expectedValue");

    for (const QString &section : actualServiceRuntime->sections()) {
        qDebug() << "section: " << section;
        expectedSettings.beginGroup(section);
        for (const QString &sectionKey :
             actualServiceRuntime->section(section).keys()) {
            qDebug() << "value" << sectionKey << "actual: "
                     << actualServiceRuntime->section(section).value(
                            sectionKey);
            qDebug() << "value" << sectionKey
                     << "expected:" << expectedSettings.value(sectionKey);
            QTest::newRow(qPrintable(sectionKey))
                << actualServiceRuntime->section(section)
                       .value(sectionKey)
                       .toString()
                << expectedSettings.value(sectionKey).toString();
        }
        expectedSettings.endGroup();
    }
}

void CosuServiceRuntimeTest::verifyServiceRuntimeLoaded()
{
    QFETCH(QString, actualValue);
    QFETCH(QString, expectedValue);

    QCOMPARE(actualValue, expectedValue);
}

QTEST_MAIN(CosuServiceRuntimeTest)

#include "testCosuServiceRuntime.moc"
