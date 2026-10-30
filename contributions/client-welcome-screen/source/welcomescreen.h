// SPDX-License-Identifier: GPL-3.0-or-later
// Adapted from the Clinton Public Library Libki welcome-screen modifications.
#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QVector>
class QWidget;
class QGridLayout;
class QLabel;
class QTimer;
class QPushButton;
class QDialog;
class QTextBrowser;
class QWebView;

struct WelcomeResource {
  QString title, icon, file;
  QUrl url;
};

class WelcomeScreen : public QObject {
  Q_OBJECT
 public:
  WelcomeScreen(QWidget *surface, QGridLayout *layout, const QString &configFile,
                QObject *parent = 0);
  bool isEnabled() const { return enabled; }
  int intervalMilliseconds() const;
  QString currentImage() const;
  static bool allowedUrl(const QUrl &url, const QStringList &origins);
 public slots:
  void advanceBackground();
  void openResource(int index);
 private slots:
  void resourceClicked();
  void openLink(const QUrl &url);
  void closeResource();
  void backToInformation();
  void pageLoaded(bool ok);
  void clearWebState();
 protected:
  bool eventFilter(QObject *object, QEvent *event);
 private:
  void paintBackground();
  QString localFile(const QString &name) const;
  QWidget *surface;
  QWidget *panel;
  QLabel *background;
  QTimer *timer;
  QDialog *dialog;
  QTextBrowser *text;
  QWebView *web;
  QLabel *dialogTitle;
  QLabel *status;
  QPushButton *returnButton;
  QString directory;
  QStringList images, origins;
  QVector<WelcomeResource> resources;
  int current;
  bool enabled;
};
#endif
