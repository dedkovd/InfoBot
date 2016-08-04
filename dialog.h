#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSystemTrayIcon>
#include <QTimer>
#include "QXmppClient.h"
#include <QNetworkProxy>
#include "QXmppMucManager.h"
#include "QXmppMessage.h"
#include "QMenu"

namespace Ui {
    class Dialog;
}

class Menu: public QMenu
{
public:
   explicit Menu(QWidget *parent = 0)
   {
        new QMenu(parent);
   }

   void focusOutEvent(QFocusEvent *event)
   {
       hide();
       QMenu::focusOutEvent(event);
   }
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void closeEvent(QCloseEvent *event);

    void collectProxy();
private slots:
    void error(QXmppClient::Error error);
    void connected();
    void messageRecived(QXmppMessage message);
    void exit();
    void help();

    void on_pbExit_clicked();

    void on_pbConnect_clicked();

    void on_pbDisconnect_clicked();

private:
    Ui::Dialog *ui;
    QSystemTrayIcon *i;
    QXmppClient *client;
    QXmppMucManager *muc;
    QXmppMucRoom *room;
    QList<QPair<QString,QString> > victorina;
    QPair<QString,QString> currentQuestion;
    QTimer *victTimer;
    QHash<QString,int> scores;
    QNetworkProxy proxy;

    QString host;
    QString jid;
    QString pass;
    QString roomName;

    bool infinityLoop;

    int helpsCount;

    void loadQuestions();
    void question();

    void saveSettings();
    void loadSettings();

    void saveScores();
    void loadScores();

    void connectToRoom();
    void disconnectFromRoom();
};

#endif // DIALOG_H
