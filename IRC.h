#pragma once
#include <QUrl>
#include <QDateTime>
#include <QTcpSocket>
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QTreeWidget>
#include <QAction>
#include <QSplitter>
#include <queue>
#include <QTextStream>
#include <QTimer>
#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QProcess>
#include "JarvisServer.h"

class Client;

class Channel : public QMainWindow {
    Q_OBJECT
public:
    Channel(QString name, Client &client);
    void addUser(QString);
    void removeUser(QString);
    void addMessage(QString,QString);
signals:
    void notify(QString,QString);
    void send(QString,QString);
    void send_cmd(QString);
public slots:
    void send(QString);

private slots:
    void highlight(QString);
    void send();
private:
    Client &client;
    QString currentnick = "jarver";
    QString wanswer;
    QString channel;
    QListWidget users;
    QPlainTextEdit text;
    QLineEdit lineEdit;
};

class Client : public QObject {
    Q_OBJECT
public:
    JarvisServer server;
    QThread serverthread;
    QNetworkAccessManager networkmanag;
    bool quiet{false};

    Client();
    Channel* newChannel(QString target);
public slots:
    //void notify(QString,QString);
    void broadcast(QString msg);
    void print(QString msg) { qtout << msg << "\n"; qtout.flush(); }
    void nwreply(QNetworkReply *reply);

protected:
    //void hideEvent(QHideEvent *);
private slots:
    //void open(QTreeWidgetItem*);
    //void fitWindow();

private:
    //QMap<QString,QTreeWidgetItem*> channels;
    QTextStream qtout{stdout};
    QList<Channel*> actual_channels;
    //TerminalPrinter &printer;
};

class Network : public QTcpSocket {
    Q_OBJECT
public:
    Network(QUrl,Client*);
    ~Network();
private slots:
    void receive();
    void send(QString);
    void send(QString,QString);
    void actual_send() { if (! queue.empty()) { send(queue.front()); queue.pop(); } }
private:
    Channel* getChannel(QString);

    Client* client;
    QMap<QString,Channel*> channels;
    QString user;
    std::queue<QString> queue;
};
