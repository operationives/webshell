0020 #include <QtTest/QtTest>
0021 
0022 #include <qdir.h>
0023 #include <qwebframe.h>
0024 #include <qwebpage.h>
0025 #include <qwebplugindatabase.h>
0026 #include <qwebsettings.h>
0027 #include <qvariant.h>
0028 
0029 class tst_QWebPluginDatabase : public QObject
0030 {
0031     Q_OBJECT
0032 
0033 private Q_SLOTS:
0034     void installedPlugins();
0035     void searchPaths();
0036     void null_data();
0037     void null();
0038     void pluginForMimeType();
0039     void enabled();
0040     void operatorequal_data();
0041     void operatorequal();
0042     void preferredPlugin();
0043     void operatorassign_data();
0044     void operatorassign();
0045 };
0046 
0047 typedef QWebPluginInfo::MimeType MimeType;
0048 
0049 void tst_QWebPluginDatabase::installedPlugins()
0050 {
0051     QWebPage page;
0052     page.settings()->setAttribute(QWebSettings::PluginsEnabled, true);
0053     QWebFrame* frame = page.mainFrame();
0054 
0055     QVariantMap jsPluginsMap = frame->evaluateJavaScript("window.navigator.plugins").toMap();
0056     QList<QWebPluginInfo> plugins = QWebSettings::pluginDatabase()->plugins();
0057     QCOMPARE(plugins, QWebSettings::pluginDatabase()->plugins());
0058 
0059     int length = jsPluginsMap["length"].toInt();
0060     QCOMPARE(length, plugins.count());
0061 
0062     for (int i = 0; i < length; ++i) {
0063         QWebPluginInfo plugin = plugins.at(i);
0064 
0065         QVariantMap jsPlugin = frame->evaluateJavaScript(QString("window.navigator.plugins[%1]").arg(i)).toMap();
0066         QString name = jsPlugin["name"].toString();
0067         QString description = jsPlugin["description"].toString();
0068         QString fileName = jsPlugin["filename"].toString();
0069 
0070         QCOMPARE(plugin.name(), name);
0071         QCOMPARE(plugin.description(), description);
0072         QCOMPARE(QFileInfo(plugin.path()).fileName(), fileName);
0073 
0074         QList<MimeType> mimeTypes;
0075         int mimeTypesCount = jsPlugin["length"].toInt();
0076 
0077         for (int j = 0; j < mimeTypesCount; ++j) {
0078             QVariantMap jsMimeType = frame->evaluateJavaScript(QString("window.navigator.plugins[%1][%2]").arg(i).arg(j)).toMap();
0079 
0080             MimeType mimeType;
0081             mimeType.name = jsMimeType["type"].toString();
0082             mimeType.description = jsMimeType["description"].toString();
0083             mimeType.fileExtensions = jsMimeType["suffixes"].toString().split(',', QString::SkipEmptyParts);
0084 
0085             mimeTypes.append(mimeType);
0086             QVERIFY(plugin.supportsMimeType(mimeType.name));
0087         }
0088 
0089         QCOMPARE(plugin.mimeTypes(), mimeTypes);
0090 
0091         QVERIFY(!plugin.isNull());
0092         QVERIFY(plugin.isEnabled());
0093     }
0094 }
0095 
0096 void tst_QWebPluginDatabase::searchPaths()
0097 {
0098     QWebPluginDatabase* database = QWebSettings::pluginDatabase();
0099     QList<QWebPluginInfo> plugins = database->plugins();
0100     QStringList directories = database->searchPaths();
0101     QCOMPARE(QWebPluginDatabase::defaultSearchPaths(), directories);
0102 
0103     database->setSearchPaths(directories);
0104     QCOMPARE(QWebPluginDatabase::defaultSearchPaths(), directories);
0105     QCOMPARE(database->searchPaths(), directories);
0106     QCOMPARE(database->plugins(), plugins);
0107     database->refresh();
0108     QCOMPARE(database->plugins(), plugins);
0109 
0110     database->setSearchPaths(QStringList());
0111     QCOMPARE(QWebPluginDatabase::defaultSearchPaths(), directories);
0112     QCOMPARE(database->searchPaths(), QStringList());
0113     QCOMPARE(database->plugins().count(), 0);
0114 
0115     database->setSearchPaths(directories);
0116     QCOMPARE(database->searchPaths(), directories);
0117     database->addSearchPath(QDir::tempPath());
0118     QCOMPARE(database->searchPaths().count(), directories.count() + 1);
0119     QVERIFY(database->searchPaths().contains(QDir::tempPath()));
0120     directories.append(QDir::tempPath());
0121     QCOMPARE(database->searchPaths(), directories);
0122 
0123     // As an empty set of search paths has been set, the database has been rebuilt
0124     // from scratch after bringing the old path set back.
0125     // Because the QWebPlugins no longer point to the same PluginPackages,
0126     // the list is also no longer equal to the older one, even though it contains
0127     // the same information.
0128     QCOMPARE(database->plugins().count(), plugins.count());
0129     plugins = database->plugins();
0130     QCOMPARE(database->plugins(), plugins);
0131 
0132     for (int i = (directories.count() - 1); i >= 0; --i) {
0133         QDir directory(directories.at(i));
0134         if (!directory.exists() || !directory.count())
0135             directories.removeAt(i);
0136     }
0137 
0138     database->setSearchPaths(directories);
0139     QCOMPARE(database->plugins(), plugins);
0140     database->refresh();
0141     QCOMPARE(database->plugins(), plugins);
0142 
0143     database->setSearchPaths(QWebPluginDatabase::defaultSearchPaths());
0144     directories = QWebPluginDatabase::defaultSearchPaths();
0145     QCOMPARE(QWebPluginDatabase::defaultSearchPaths(), directories);
0146     QCOMPARE(database->searchPaths(), directories);
0147     QCOMPARE(database->plugins(), plugins);
0148 }
0149 
0150 Q_DECLARE_METATYPE(QWebPluginInfo)
0151 void tst_QWebPluginDatabase::null_data()
0152 {
0153     QTest::addColumn<QWebPluginInfo>("plugin");
0154     QTest::addColumn<bool>("null");
0155 
0156     QTest::newRow("null") << QWebPluginInfo() << true;
0157     QTest::newRow("foo") << QWebSettings::pluginDatabase()->pluginForMimeType("foobarbaz") << true;
0158 
0159     QList<QWebPluginInfo> plugins = QWebSettings::pluginDatabase()->plugins();
0160     for (int i = 0; i < plugins.count(); ++i)
0161         QTest::newRow(QString::number(i).toUtf8().constData()) << plugins.at(i) << false;
0162 }
0163 
0164 void tst_QWebPluginDatabase::null()
0165 {
0166     QFETCH(QWebPluginInfo, plugin);
0167     QFETCH(bool, null);
0168 
0169     QCOMPARE(plugin.isNull(), null);
0170 }
0171 
0172 void tst_QWebPluginDatabase::pluginForMimeType()
0173 {
0174     QMultiMap<QString, QWebPluginInfo> pluginsMap;
0175     QWebPluginDatabase* database = QWebSettings::pluginDatabase();
0176     QList<QWebPluginInfo> plugins = database->plugins();
0177 
0178     for (int i = 0; i < plugins.count(); ++i) {
0179         QWebPluginInfo plugin = plugins.at(i);
0180 
0181         QList<MimeType> mimeTypes = plugin.mimeTypes();
0182         for (int j = 0; j < mimeTypes.count(); ++j) {
0183             QString mimeType = mimeTypes.at(j).name;
0184             pluginsMap.insert(mimeType, plugin);
0185             QVERIFY(plugin.supportsMimeType(mimeType));
0186         }
0187     }
0188 
0189     for (int i = 0; i < plugins.count(); ++i) {
0190         QWebPluginInfo plugin = plugins.at(i);
0191 
0192         QList<MimeType> mimeTypes = plugin.mimeTypes();
0193         for (int j = 0; j < mimeTypes.count(); ++j) {
0194             QString mimeType = mimeTypes.at(j).name;
0195 
0196             QVERIFY(pluginsMap.count(mimeType) > 0);
0197             if (pluginsMap.count(mimeType) > 1)
0198                 continue;
0199 
0200             QWebPluginInfo pluginForMimeType = database->pluginForMimeType(mimeType);
0201             QCOMPARE(pluginForMimeType, plugin);
0202             database->setSearchPaths(database->searchPaths());
0203             QCOMPARE(pluginForMimeType, plugin);
0204             QCOMPARE(pluginForMimeType, database->pluginForMimeType(mimeType.toUpper()));
0205             QCOMPARE(pluginForMimeType, database->pluginForMimeType(mimeType.toLower()));
0206             QVERIFY(plugin.supportsMimeType(mimeType));
0207             QVERIFY(!pluginForMimeType.isNull());
0208             QVERIFY(!plugin.isNull());
0209         }
0210     }
0211 }
0212 
0213 void tst_QWebPluginDatabase::enabled()
0214 {
0215     QMultiMap<QString, QWebPluginInfo> pluginsMap;
0216     QWebPluginDatabase* database = QWebSettings::pluginDatabase();
0217     QList<QWebPluginInfo> plugins = database->plugins();
0218 
0219     for (int i = 0; i < plugins.count(); ++i) {
0220         QWebPluginInfo plugin = plugins.at(i);
0221 
0222         QList<MimeType> mimeTypes = plugin.mimeTypes();
0223         for (int j = 0; j < mimeTypes.count(); ++j) {
0224             QString mimeType = mimeTypes.at(j).name;
0225             pluginsMap.insert(mimeType, plugin);
0226             QVERIFY(plugin.supportsMimeType(mimeType));
0227         }
0228     }
0229 
0230     QMultiMap<QString, QWebPluginInfo>::iterator it = pluginsMap.begin();
0231     while (it != pluginsMap.end()) {
0232         QString mimeType = it.key();
0233         QWebPluginInfo plugin = it.value();
0234         QWebPluginInfo pluginForMimeType = database->pluginForMimeType(mimeType);
0235 
0236         QVERIFY(pluginsMap.count(mimeType) > 0);
0237 
0238         if (pluginsMap.count(mimeType) == 1) {
0239             QCOMPARE(plugin, pluginForMimeType);
0240 
0241             QVERIFY(plugin.isEnabled());
0242             QVERIFY(pluginForMimeType.isEnabled());
0243             plugin.setEnabled(false);
0244             QVERIFY(!plugin.isEnabled());
0245             QVERIFY(!pluginForMimeType.isEnabled());
0246         } else {
0247             QVERIFY(plugin.isEnabled());
0248             QVERIFY(pluginForMimeType.isEnabled());
0249             plugin.setEnabled(false);
0250             QVERIFY(!plugin.isEnabled());
0251         }
0252 
0253         QVERIFY(!plugin.isNull());
0254         QVERIFY(!pluginForMimeType.isNull());
0255 
0256         QWebPluginInfo pluginForMimeType2 = database->pluginForMimeType(mimeType);
0257         if (pluginsMap.count(mimeType) == 1) {
0258             QVERIFY(pluginForMimeType2 != plugin);
0259             QVERIFY(pluginForMimeType2.isNull());
0260         } else {
0261             QVERIFY(pluginForMimeType2 != plugin);
0262             QVERIFY(!pluginForMimeType2.isNull());
0263         }
0264 
0265         plugin.setEnabled(true);
0266 
0267         ++it;
0268     }
0269 }
0270 
0271 void tst_QWebPluginDatabase::operatorequal_data()
0272 {
0273     QTest::addColumn<QWebPluginInfo>("first");
0274     QTest::addColumn<QWebPluginInfo>("second");
0275     QTest::addColumn<bool>("equal");
0276 
0277     QWebPluginDatabase* database = QWebSettings::pluginDatabase();
0278     QTest::newRow("null") << QWebPluginInfo() << QWebPluginInfo() << true;
0279     QTest::newRow("application/x-shockwave-flash") << database->pluginForMimeType("application/x-shockwave-flash")
0280                                                    << database->pluginForMimeType("application/x-shockwave-flash") << true;
0281     QTest::newRow("foo/bar-baz") << database->pluginForMimeType("foo/bar-baz")
0282                                  << database->pluginForMimeType("foo/bar-baz") << true;
0283 
0284     QList<QWebPluginInfo> plugins = database->plugins();
0285     for (int i = 0; i < (plugins.count() - 1); ++i) {
0286         QWebPluginInfo first = plugins.at(i);
0287         QWebPluginInfo second = plugins.at(i + 1);
0288 
0289         QTest::newRow(QString("%1==%2").arg(first.name(), second.name()).toUtf8().constData())
0290                                     << first << second << false;
0291     }
0292 }
0293 
0294 void tst_QWebPluginDatabase::operatorequal()
0295 {
0296     QFETCH(QWebPluginInfo, first);
0297     QFETCH(QWebPluginInfo, second);
0298     QFETCH(bool, equal);
0299 
0300     QCOMPARE(first == second, equal);
0301 }
0302 
0303 void tst_QWebPluginDatabase::preferredPlugin()
0304 {
0305     QMultiMap<QString, QWebPluginInfo> pluginsMap;
0306     QWebPluginDatabase* database = QWebSettings::pluginDatabase();
0307     QList<QWebPluginInfo> plugins = database->plugins();
0308 
0309     for (int i = 0; i < plugins.count(); ++i) {
0310         QWebPluginInfo plugin = plugins.at(i);
0311 
0312         QList<MimeType> mimeTypes = plugin.mimeTypes();
0313         for (int j = 0; j < mimeTypes.count(); ++j) {
0314             QString mimeType = mimeTypes.at(j).name;
0315             pluginsMap.insert(mimeType, plugin);
0316         }
0317     }
0318 
0319     QMultiMap<QString, QWebPluginInfo>::iterator it = pluginsMap.begin();
0320     while (it != pluginsMap.end()) {
0321         QString mimeType = it.key();
0322 
0323         if (pluginsMap.count(mimeType) > 1) {
0324             QList<QWebPluginInfo> pluginsForMimeType = pluginsMap.values(mimeType);
0325             QWebPluginInfo plugin = database->pluginForMimeType(mimeType);
0326             QVERIFY(plugin.supportsMimeType(mimeType));
0327 
0328             pluginsForMimeType.removeAll(plugin);
0329             for (int i = 0; i < pluginsForMimeType.count(); ++i) {
0330                 QWebPluginInfo anotherPlugin = pluginsForMimeType.at(i);
0331                 QVERIFY(plugin.supportsMimeType(mimeType));
0332                 QVERIFY(plugin != anotherPlugin);
0333 
0334                 QCOMPARE(database->pluginForMimeType(mimeType), plugin);
0335                 database->setPreferredPluginForMimeType(mimeType, anotherPlugin);
0336                 QCOMPARE(database->pluginForMimeType(mimeType), anotherPlugin);
0337 
0338                 anotherPlugin.setEnabled(false);
0339                 QCOMPARE(database->pluginForMimeType(mimeType), plugin);
0340 
0341                 anotherPlugin.setEnabled(true);
0342                 QCOMPARE(database->pluginForMimeType(mimeType), anotherPlugin);
0343                 database->setSearchPaths(database->searchPaths());
0344                 QCOMPARE(database->pluginForMimeType(mimeType), anotherPlugin);
0345 
0346                 database->setPreferredPluginForMimeType(mimeType, QWebPluginInfo());
0347                 QCOMPARE(database->pluginForMimeType(mimeType), plugin);
0348             }
0349         } else {
0350             QWebPluginInfo plugin = database->pluginForMimeType(mimeType);
0351             QCOMPARE(pluginsMap.value(mimeType), plugin);
0352 
0353             database->setPreferredPluginForMimeType(mimeType, plugin);
0354             QCOMPARE(database->pluginForMimeType(mimeType), plugin);
0355 
0356             plugin.setEnabled(false);
0357             QCOMPARE(database->pluginForMimeType(mimeType), QWebPluginInfo());
0358             plugin.setEnabled(true);
0359 
0360             database->setPreferredPluginForMimeType(mimeType, QWebPluginInfo());
0361             QCOMPARE(database->pluginForMimeType(mimeType), plugin);
0362         }
0363 
0364         ++it;
0365     }
0366 
0367     if (pluginsMap.keys().count() >= 2) {
0368         QStringList mimeTypes = pluginsMap.uniqueKeys();
0369 
0370         QString mimeType1 = mimeTypes.at(0);
0371         QString mimeType2 = mimeTypes.at(1);
0372         QWebPluginInfo plugin1 = database->pluginForMimeType(mimeType1);
0373         QWebPluginInfo plugin2 = database->pluginForMimeType(mimeType2);
0374 
0375         int i = 2;
0376         while (plugin2.supportsMimeType(mimeType1)
0377                && !mimeType2.isEmpty()
0378                && i < mimeTypes.count()) {
0379             mimeType2 = mimeTypes.at(i);
0380             plugin2 = database->pluginForMimeType(mimeType2);
0381             ++i;
0382         }
0383 
0384         plugin1 = database->pluginForMimeType(mimeType1);
0385         QVERIFY(plugin1.supportsMimeType(mimeType1));
0386         QVERIFY(!plugin1.isNull());
0387         plugin2 = database->pluginForMimeType(mimeType2);
0388         QVERIFY(plugin2.supportsMimeType(mimeType2));
0389         QVERIFY(!plugin2.isNull());
0390 
0391         database->setPreferredPluginForMimeType(mimeType2, plugin1);
0392         QVERIFY(!plugin1.supportsMimeType(mimeType2));
0393         QCOMPARE(database->pluginForMimeType(mimeType2), plugin2);
0394 
0395         database->setPreferredPluginForMimeType(mimeType1, plugin1);
0396         QVERIFY(!plugin2.supportsMimeType(mimeType1));
0397         QCOMPARE(database->pluginForMimeType(mimeType2), plugin2);
0398     }
0399 }
0400 
0401 void tst_QWebPluginDatabase::operatorassign_data()
0402 {
0403     QTest::addColumn<QWebPluginInfo>("first");
0404     QTest::addColumn<QWebPluginInfo>("second");
0405 
0406     QWebPluginDatabase* database = QWebSettings::pluginDatabase();
0407     QTest::newRow("null") << QWebPluginInfo() << QWebPluginInfo();
0408 
0409     QList<QWebPluginInfo> plugins = database->plugins();
0410     for (int i = 0; i < (plugins.count() - 1); ++i) {
0411         QWebPluginInfo first = plugins.at(i);
0412         QWebPluginInfo second = plugins.at(i + 1);
0413 
0414         QTest::newRow(QString("%1=%2").arg(first.name(), second.name()).toUtf8().constData()) << first << second;
0415     }
0416 }
0417 
0418 void tst_QWebPluginDatabase::operatorassign()
0419 {
0420     QFETCH(QWebPluginInfo, first);
0421     QFETCH(QWebPluginInfo, second);
0422 
0423     QWebPluginInfo info;
0424     QCOMPARE(info.mimeTypes(), QList<MimeType>());
0425     QCOMPARE(info = first, first);
0426     QCOMPARE(info, first);
0427     QCOMPARE(info.mimeTypes(), first.mimeTypes());
0428     QCOMPARE(info = second, second);
0429     QCOMPARE(info, second);
0430     QCOMPARE(info.mimeTypes(), second.mimeTypes());
0431     QCOMPARE(info = QWebPluginInfo(), QWebPluginInfo());
0432     QCOMPARE(info.mimeTypes(), QList<MimeType>());
0433 }
0434 
0435 QTEST_MAIN(tst_QWebPluginDatabase)
0436 
0437 #include "tst_qwebplugindatabase.moc"