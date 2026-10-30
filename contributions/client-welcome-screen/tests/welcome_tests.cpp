// SPDX-License-Identifier: GPL-3.0-or-later
#include <QtTest>
#include <QTemporaryDir>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QLabel>
#include <QToolButton>
#include <QTextBrowser>
#include <QWebView>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QWebPage>
#include <QWebHistory>
#include <QTimer>
#include <QDialog>
#include "loginwindow.h"
#include "welcomescreen.h"

class WelcomeTests : public QObject {
 Q_OBJECT
 QString example;
 QString writeConfig(QTemporaryDir &dir, const QString &extra) {
   QFile f(dir.path()+"/welcome.ini"); f.open(QIODevice::WriteOnly);
   f.write(("[welcome]\nenabled=true\n"+extra).toUtf8()); f.close();
   return f.fileName();
 }
 void closeWindow(LoginWindow &window) { window.setAllowClose(true); window.close(); }
 private slots:
 void initTestCase() {
   QCoreApplication::setOrganizationName("LibkiWelcomeTests");
   QCoreApplication::setApplicationName("IsolatedDemo");
   QSettings::setDefaultFormat(QSettings::IniFormat);
   example=QDir::current().absoluteFilePath("welcome-example/welcome.ini");
   QVERIFY(QFileInfo(example).isFile());
   QSettings settings; settings.clear();
   settings.setValue("node/name","DEMO-01");
   settings.setValue("labels/username","Library card number:");
 }
 void cleanup() { QSettings settings; settings.remove("welcome"); settings.remove("node/no_passwords"); }
 void disabledAndMissingConfiguration() {
   QWidget surface; QGridLayout layout(&surface);
   WelcomeScreen absent(&surface,&layout,"");
   QVERIFY(!absent.isEnabled()); QCOMPARE(layout.count(),0);
   QTemporaryDir dir;
   QString file=writeConfig(dir,"");
   QSettings settings(file,QSettings::IniFormat); settings.setValue("welcome/enabled",false); settings.sync();
   WelcomeScreen disabled(&surface,&layout,file);
   QVERIFY(!disabled.isEnabled()); QCOMPARE(layout.count(),0);
 }
 void intervalValidation_data() {
   QTest::addColumn<QString>("input"); QTest::addColumn<int>("expected");
   QTest::newRow("default-invalid")<<QString("oops")<<180000;
   QTest::newRow("negative")<<QString("-1")<<180000;
   QTest::newRow("overflow")<<QString("999999999999")<<180000;
   QTest::newRow("too-large")<<QString("86401")<<180000;
   QTest::newRow("clamped")<<QString("1")<<5000;
   QTest::newRow("still")<<QString("0")<<0;
   QTest::newRow("custom")<<QString("30")<<30000;
 }
 void intervalValidation() {
   QFETCH(QString,input); QFETCH(int,expected);
   QTemporaryDir dir; QWidget surface; QGridLayout layout(&surface);
   WelcomeScreen screen(&surface,&layout,writeConfig(dir,"rotation_seconds="+input+"\n"));
   QCOMPARE(screen.intervalMilliseconds(),expected);
 }
 void imageValidationAndRotation() {
   QTemporaryDir dir;
   QImage a(20,20,QImage::Format_RGB32); a.fill(Qt::red); QVERIFY(a.save(dir.path()+"/a.png"));
   a.fill(Qt::blue); QVERIFY(a.save(dir.path()+"/b.png"));
   QWidget surface; surface.resize(600,400); QGridLayout layout(&surface);
   WelcomeScreen screen(&surface,&layout,writeConfig(dir,"images=missing.png, a.png, ../outside.png, b.png\nrotation_seconds=0\n"));
   QVERIFY(screen.currentImage().endsWith("/a.png"));
   screen.advanceBackground(); QVERIFY(screen.currentImage().endsWith("/b.png"));
   screen.advanceBackground(); QVERIFY(screen.currentImage().endsWith("/a.png"));
   surface.show(); QTest::qWait(40);
   surface.resize(800,500); QTest::qWait(40);
   QCOMPARE(surface.findChild<QLabel *>("welcomeBackground")->size(),surface.size());
 }
 void urlBoundaries_data() {
   QTest::addColumn<QString>("url"); QTest::addColumn<bool>("allowed");
   QTest::newRow("https")<<QString("https://library.example.org/help")<<true;
   QTest::newRow("explicit-default-port")<<QString("https://library.example.org:443/help")<<true;
   QTest::newRow("http")<<QString("http://library.example.org/")<<false;
   QTest::newRow("lookalike")<<QString("https://library.example.org.evil.invalid/")<<false;
   QTest::newRow("subdomain")<<QString("https://other.library.example.org/")<<false;
   QTest::newRow("credentials")<<QString("https://user:pass@library.example.org/")<<false;
   QTest::newRow("file")<<QString("file:///C:/private.txt")<<false;
   QTest::newRow("script")<<QString("javascript:alert(1)")<<false;
   QTest::newRow("port")<<QString("https://library.example.org:444/")<<false;
   QTest::newRow("relative")<<QString("/help")<<false;
 }
 void urlBoundaries() {
   QFETCH(QString,url); QFETCH(bool,allowed);
   QCOMPARE(WelcomeScreen::allowedUrl(QUrl(url),QStringList()<<QString("https://library.example.org")),allowed);
   QVERIFY(!WelcomeScreen::allowedUrl(QUrl(url),QStringList()));
 }
 void localPolicyAndNetworkRestriction() {
   QWidget surface; surface.resize(1000,700); QGridLayout layout(&surface);
   WelcomeScreen screen(&surface,&layout,example); surface.show();
   screen.openResource(0); QTest::qWait(80);
   QDialog *dialog=surface.findChild<QDialog *>("welcomeInformation"); QVERIFY(dialog);
   QTextBrowser *body=dialog->findChild<QTextBrowser *>("welcomePolicyText"); QVERIFY(body);
   QVERIFY(body->toPlainText().contains("A little help"));
   QVERIFY(!body->openExternalLinks());
   QVERIFY(QMetaObject::invokeMethod(&screen,"openLink",Q_ARG(QUrl,QUrl("https://unapproved.invalid/"))));
   QVERIFY(body->isVisible());
   QWebView *web=dialog->findChild<QWebView *>("welcomeLinkedPage"); QVERIFY(web);
   QNetworkReply *reply=web->page()->networkAccessManager()->get(QNetworkRequest(QUrl("file:///C:/windows/win.ini")));
   QSignalSpy finished(reply,SIGNAL(finished())); if (!reply->isFinished()) QVERIFY(finished.wait(3000));
   QVERIFY(reply->error()!=QNetworkReply::NoError); QVERIFY(reply->readAll().isEmpty()); reply->deleteLater();
   QNetworkCookieJar *jar=web->page()->networkAccessManager()->cookieJar();
   QList<QNetworkCookie> cookies; cookies<<QNetworkCookie("demo","value");
   QVERIFY(!jar->setCookiesFromUrl(cookies,QUrl("https://library.example.org/")));
   QVERIFY(jar->cookiesForUrl(QUrl("https://library.example.org/")).isEmpty());
   QTest::mouseClick(dialog->findChild<QPushButton *>("welcomeInformationClose"),Qt::LeftButton);
   QVERIFY(!dialog->isVisible());
   QCOMPARE(web->history()->count(),0);
 }
 void configurableContentAndTimer() {
   QTemporaryDir dir;
   QFile policy(dir.path()+"/custom.html"); QVERIFY(policy.open(QIODevice::WriteOnly));
   policy.write("<h1>Example Library</h1><p>Our locally approved words.</p>"); policy.close();
   QImage image(20,20,QImage::Format_RGB32); image.fill(Qt::red); QVERIFY(image.save(dir.path()+"/a.png"));
   image.fill(Qt::blue); QVERIFY(image.save(dir.path()+"/b.png"));
   QString path=writeConfig(dir,"title=Example Library\nimages=a.png, b.png\nrotation_seconds=5\n");
   QSettings config(path,QSettings::IniFormat);
   config.beginWriteArray("resources"); config.setArrayIndex(0);
   config.setValue("title","Our rules"); config.setValue("content_file","custom.html");
   config.endArray(); config.sync();
   QWidget surface; surface.resize(1024,768); QGridLayout layout(&surface);
   WelcomeScreen screen(&surface,&layout,path); surface.show();
   QCOMPARE(surface.findChild<QLabel *>("welcomeTitle")->text(),QString("Example Library"));
   QCOMPARE(surface.findChild<QToolButton *>("welcomeResource0")->text(),QString("Our rules"));
   screen.openResource(0); QTest::qWait(30);
   QVERIFY(surface.findChild<QTextBrowser *>("welcomePolicyText")->toPlainText().contains("Our locally approved words."));
   surface.findChild<QDialog *>("welcomeInformation")->close();
   QString first=screen.currentImage(); QTest::qWait(5200); QVERIFY(screen.currentImage()!=first);
   surface.hide(); QVERIFY(!screen.findChild<QTimer *>("welcomeRotationTimer")->isActive());
   surface.show(); QVERIFY(screen.findChild<QTimer *>("welcomeRotationTimer")->isActive());
 }
 void nativeLoginPreserved() {
   QSettings settings; settings.setValue("welcome/config",example); settings.sync();
   LoginWindow window; QTest::qWait(100);
   QVERIFY(window.passwordField->isVisible()); QVERIFY(window.usernameField->isVisible());
   QSignalSpy login(&window,SIGNAL(attemptLogin(QString,QString,bool)));
   window.usernameField->setText("DEMO-CARD"); window.passwordField->setText("DEMO-PASSWORD");
   QTest::mouseClick(window.loginButton,Qt::LeftButton); QCOMPARE(login.count(),1);
   QCOMPARE(login.at(0).at(0).toString(),QString("DEMO-CARD"));
   // Signal has no server connected in this harness.
   window.attemptLoginFailure("Demonstration error");
   QVERIFY(window.errorLabel->isVisible()); QVERIFY(window.usernameField->isEnabled());
   window.showServerAccessWarning("Demonstration server warning");
   QVERIFY(window.serverAccessWarning->isVisible());
   closeWindow(window);
 }
 void disabledNativeLayout() {
   QSettings settings; settings.remove("welcome"); settings.sync();
   LoginWindow window; QTest::qWait(50);
   QVERIFY(window.headerLabel->isVisible()); QVERIFY(window.logo->isVisible());
   QVERIFY(!window.findChild<QWidget *>("welcomePanel")); closeWindow(window);
 }
 void screenshots() {
   QSettings settings; settings.setValue("welcome/config",example);
   settings.setValue("node/no_passwords",1); settings.sync();
   LoginWindow window; QTest::qWait(100);
   window.showNormal(); window.setGeometry(0,0,1440,900); QTest::qWait(180);
   QDir().mkpath("screenshots");
   WelcomeScreen *screen=window.findChild<WelcomeScreen *>(); QVERIFY(screen);
   QVERIFY(window.grab().save("screenshots/01-winter-welcome.png"));
   screen->advanceBackground(); QTest::qWait(80);
   QVERIFY(window.grab().save("screenshots/02-riverfront-welcome.png"));
   screen->advanceBackground(); QTest::qWait(80);
   QVERIFY(window.grab().save("screenshots/03-garden-welcome.png"));
   QToolButton *button=window.findChild<QToolButton *>("welcomeResource0"); QVERIFY(button);
   QTest::mouseClick(button,Qt::LeftButton); QTest::qWait(80);
   QDialog *dialog=window.findChild<QDialog *>("welcomeInformation"); QVERIFY(dialog);
   QVERIFY(dialog->grab().save("screenshots/04-faq-panel.png"));
   QTest::mouseClick(dialog->findChild<QPushButton *>("welcomeInformationClose"),Qt::LeftButton);
   screen->openResource(3); QTest::qWait(60);
   QVERIFY(dialog->grab().save("screenshots/05-printing-panel.png"));
   QTest::mouseClick(dialog->findChild<QPushButton *>("welcomeInformationClose"),Qt::LeftButton);
   closeWindow(window);
 }
};
QTEST_MAIN(WelcomeTests)
#include "welcome_tests.moc"
