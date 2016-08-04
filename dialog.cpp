#include "dialog.h"
#include "ui_dialog.h"

#include <QDebug>
#include <QFile>
#include <QCloseEvent>
#include <QSettings>
#include <QNetworkProxy>
#include "QXmppLogger.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);    

    loadQuestions();

    Menu *m = new Menu(this);
    QAction *show = m->addAction(tr("Show"));
    QAction *exit = m->addAction(tr("Exit"));

    connect(exit,SIGNAL(triggered()),this,SLOT(exit()));
    connect(show,SIGNAL(triggered()),this,SLOT(show()));

    i = new QSystemTrayIcon(QIcon(":/info"),this);
    i->setContextMenu(m);
    i->show();

    QXmppLogger::getLogger()->setLoggingType(QXmppLogger::FileLogging);

    client = new QXmppClient(this);
    client->logger()->setLoggingType(QXmppLogger::StdoutLogging);

    connect(client,SIGNAL(error(QXmppClient::Error)),this,SLOT(error(QXmppClient::Error)));
    connect(client,SIGNAL(connected()),this,SLOT(connected()));
    connect(client,SIGNAL(messageReceived(QXmppMessage)),this,SLOT(messageRecived(QXmppMessage)));

    muc = new QXmppMucManager();
    client->addExtension(muc);

    victTimer = new QTimer(this);
    connect(victTimer,SIGNAL(timeout()),this,SLOT(help()));

    qsrand(QDateTime::currentDateTime().time().msec());

    loadSettings();

    infinityLoop = false;
}

Dialog::~Dialog()
{
    i->hide();
    delete ui;
}

void Dialog::closeEvent(QCloseEvent *event){
    if (i->isVisible()) {
        hide();
        event->ignore();
    }
}

void Dialog::exit()
{
    i->hide();
    i->setVisible(false);
    close();
}

void Dialog::error(QXmppClient::Error error)
{
    qDebug() << error;
}

void Dialog::connected()
{
    i->showMessage(tr("Connection"),tr("Connected to server!"));

    this->room = muc->addRoom(roomName);
    this->room->setNickName("InfoBot");
    this->room->join();
    this->room->sendMessage(tr("Hi All! Send me command 'help' for help."));
}

void Dialog::loadQuestions()
{
    QFile f(":/questions");

    if (f.open(QFile::ReadOnly))
    {
        QTextStream in(&f);

        QString line;
        do {
            line = in.readLine();

            QStringList question = line.split("|");
            victorina.append(QPair<QString,QString>(question.at(0),question.at(1)));

         } while (!line.isNull());

        f.close();
    }
}

void Dialog::help()
{
    QString help = currentQuestion.second;
    helpsCount++;

    if (help.size() == helpsCount)
    {
        this->room->sendMessage(tr("Answer: %1, nobody wins!").arg(help));
        currentQuestion = QPair<QString,QString>();
        victTimer->stop();

        if (infinityLoop) {
            question();
        }
    }
    else
    {
        this->room->sendMessage(tr("Help: %1").arg(help.left(helpsCount)));
    }
}

void Dialog::question()
{
    if (victorina.count() == 0) {
        this->room->sendMessage(tr("End questions. Strarts again."));
        loadQuestions();
    }

    int idx = qrand() % victorina.count();
    currentQuestion = victorina.at(idx);
    victorina.removeAt(idx);

    this->room->sendMessage(tr("Question: %1").arg(currentQuestion.first));
    victTimer->start(30000);
    helpsCount = 0;
}

void Dialog::messageRecived(QXmppMessage message)
{
    QString mes = message.body().toLower();

    if (currentQuestion != QPair<QString,QString>())
    {
        if (mes == currentQuestion.second.toLower())
        {
            QString winner = message.from().split("/").at(1);

            this->room->sendMessage(tr("Answer: %1! %2 wins! Scores: %3").arg(currentQuestion.second,winner).arg(++scores[winner]));
            currentQuestion = QPair<QString,QString>();

            saveScores();
            victTimer->stop();

            if (infinityLoop) {
                question();
            }
        }
    }

    if (mes.startsWith("infobot") && message.stamp().isNull())
    {
        qDebug() << mes << message.to() << message.stamp();

        if (mes.indexOf("help") >= 0)
        {
            QString aviableCommands = tr("'anek' - anekdot\n'quest' - victorina question\n'scores' - victorina scores\n'qstart' - infinity questions\n'qstop' - end infinity questions");
            this->room->sendMessage(aviableCommands);
        }

        if (mes.indexOf("quest") >= 0)
        {
            if (currentQuestion != QPair<QString,QString>())
            {
                this->room->sendMessage(tr("Question: %1").arg(currentQuestion.first));
            }
            else
            {
                question();
            }
        }

        if (mes.indexOf("qstart") >= 0)
        {
            if (currentQuestion != QPair<QString,QString>())
            {
                this->room->sendMessage(tr("Question: %1").arg(currentQuestion.first));
            }
            else
            {
                infinityLoop = true;
                question();
            }
        }

        if (mes.indexOf("qstop") >= 0) {
            infinityLoop = false;
        }

        if (mes.indexOf("scores") >= 0)
        {
            QString rep = tr("Scores: \n");
            foreach (QString member, scores.keys())
            {
                rep += QString("%1: %2\n").arg(member).arg(scores[member]);
            }
            if (scores.count() == 0)
            {
                rep += tr("no scores");
            }

            this->room->sendMessage(rep);
        }

        if (mes.indexOf("anek") >= 0)
        {
            QFile f(":/anek");

            QString rep;            
            if (f.open(QFile::ReadOnly))
            {
                QTextStream in(&f);
                QStringList list;

                QString s;
                QString line;
                do {
                    line = in.readLine();

                    if (line.startsWith("---"))
                    {
                        list.append(s);
                        s = "";
                    } else
                    {
                        s += line;
                    }
                 } while (!line.isNull());

                f.close();

                rep = list.at(qrand() % list.count());
            } else
            {
                rep = tr("File not found!");
            }

            this->room->sendMessage(rep);
        }
    }
}

void Dialog::on_pbExit_clicked()
{
    disconnectFromRoom();
    exit();
}

void Dialog::collectProxy()
{
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(ui->leProxyHost->text());
    proxy.setPort(ui->sbPort->value());
    proxy.setUser(ui->leProxyUser->text());
    proxy.setPassword(ui->leProxyPass->text());
}

void Dialog::saveSettings()
{
    host = ui->leHost->text();
    jid = ui->leUsername->text();
    pass = ui->lePassword->text();
    roomName = ui->leConference->text();

    QSettings s(this);
    s.beginGroup("Jabber settings");
    s.setValue("host", host);
    s.setValue("jid", jid);
    s.setValue("room", roomName);
    s.endGroup();

    this->collectProxy();

    s.beginGroup("Proxy settings");
    s.setValue("host", proxy.hostName());
    s.setValue("port", proxy.port());
    s.setValue("user", proxy.user());
    s.setValue("pass", proxy.password());
    s.endGroup();
}

void Dialog::loadSettings()
{
    QSettings s(this);
    s.beginGroup("Jabber settings");
    host = s.value("host","").toString();
    jid = s.value("jid","").toString();
    roomName = s.value("room","").toString();
    s.endGroup();

    ui->leHost->setText(host);
    ui->leUsername->setText(jid);
    ui->lePassword->setText(pass);
    ui->leConference->setText(roomName);

    s.beginGroup("Proxy settings");
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(s.value("host","").toString());
    proxy.setPort(s.value("port",0).toInt());
    proxy.setUser(s.value("user","").toString());
    proxy.setPassword(s.value("pass","").toString());
    s.endGroup();

    ui->leProxyHost->setText(proxy.hostName());
    ui->sbPort->setValue(proxy.port());
    ui->leProxyUser->setText(proxy.user());
    ui->leProxyPass->setText(proxy.password());

    loadScores();
}

void Dialog::loadScores()
{
    scores.clear();

    QSettings s(this);
    int cnt = s.beginReadArray(roomName);

    for (int i = 0; i<cnt; i++)
    {
        s.setArrayIndex(i);
        QString key = s.value("user","").toString();
        if (key.length() > 0)
        {
            scores[key] = s.value("score",0).toInt();
        }
    }

    s.endArray();
}

void Dialog::saveScores()
{
    QSettings s(this);
    s.beginWriteArray(roomName, scores.count());

    for (int i = 0; i<scores.count(); i++)
    {
        QString key = scores.keys().at(i);
        s.setArrayIndex(i);
        s.setValue("user", key);
        s.setValue("score",scores[key]);
    }

    s.endArray();
}

void Dialog::on_pbConnect_clicked()
{
    connectToRoom();
    hide();
}

void Dialog::on_pbDisconnect_clicked()
{
    disconnectFromRoom();
    hide();
}

void Dialog::connectToRoom()
{
    disconnect();
    saveSettings();

    QXmppConfiguration config;
    config.setHost(host);
    config.setPort(443);
    config.setStreamSecurityMode(QXmppConfiguration::TLSEnabled);
    config.setJid(jid);
    config.setPassword(pass);
    if (this->proxy.hostName().length() > 0)
    {
        config.setNetworkProxy(this->proxy);
    }

    client->connectToServer(config);
}

void Dialog::disconnectFromRoom()
{
    currentQuestion = QPair<QString,QString>();
    victTimer->stop();

    this->room->sendMessage(tr("Bye all!"));
    this->room->leave();
    client->disconnectFromServer();
}
