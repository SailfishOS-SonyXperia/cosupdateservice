/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "../src/CosuSettings.h"
#include <QDebug>
#include <QObject>
#include <QTest>
#include <memory>

class CosuSettingsTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void verifySettingsLoaded();
    void verifySettingsLoaded_data();

    void verifySettingsShadowed();
    void verifySettingsShadowed_data();

private:
    void verifySettings_base(QMap<QString, QStringList> testDataList);
};

void CosuSettingsTest::verifySettings_base(
    QMap<QString, QStringList> testDataList)
{
    QStringList actualDataPaths = testDataList[QStringLiteral("actual")];
    std::unique_ptr<CosuSettings> actualSettings(
        new CosuSettings(actualDataPaths));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    const QStringList actualKeysList = actualSettings->allKeys();
    const QSet<QString> actualKeys(actualKeysList.constBegin(),
                                   actualKeysList.constEnd());
#else
    const QSet<QString> actualKeys = actualSettings->allKeys().toSet();
#endif
    QSettings expectedSettings(testDataList[QStringLiteral("expected")].first(),
                               QSettings::IniFormat);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    const QStringList expectedKeyList = expectedSettings.allKeys();
    const QSet<QString> expectedKeys(expectedKeyList.constBegin(),
                                     expectedKeyList.constEnd());
#else
    const QSet<QString> expectedKeys = expectedSettings.allKeys().toSet();
#endif

    for (const QString &key : actualSettings->allKeys()) {
        qDebug() << "key: " << key;
        qDebug() << "actual: " << actualSettings->value(key);
        qDebug() << "expected:" << expectedSettings.value(key);
    }

    QTest::addColumn<QString>("actualValue");
    QTest::addColumn<QString>("expectedValue");

    for (const QString &key : actualKeys + expectedKeys) {
        QTest::newRow(qPrintable(key))
            << actualSettings->value(key).toString()
            << expectedSettings.value(key).toString();
    }
}

void CosuSettingsTest::verifySettingsLoaded_data()
{
    QMap<QString, QStringList> testParameters = {
        {"actual", {QFINDTESTDATA("./data/actual/lib")}},
        {"expected",
         {QFINDTESTDATA("./data/expected/verifySettings_expected.ini")}}};
    this->verifySettings_base(testParameters);
}

void CosuSettingsTest::verifySettingsLoaded()
{
    QFETCH(QString, actualValue);
    QFETCH(QString, expectedValue);

    QCOMPARE(actualValue, expectedValue);
}

void CosuSettingsTest::verifySettingsShadowed_data()
{
    QMap<QString, QStringList> testParameters = {
        {"actual",
         {QFINDTESTDATA("./data/actual/lib"),
          QFINDTESTDATA("./data/actual/etc")}},
        {"expected",
         {QFINDTESTDATA(
             "./data/expected/verifySettingsShadowed_expected.ini")}}};
    this->verifySettings_base(testParameters);
}

void CosuSettingsTest::verifySettingsShadowed()
{
    QFETCH(QString, actualValue);
    QFETCH(QString, expectedValue);

    QCOMPARE(actualValue, expectedValue);
}

QTEST_MAIN(CosuSettingsTest)

#include "testCosuSettings.moc"
