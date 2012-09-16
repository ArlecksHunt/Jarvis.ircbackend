#ifndef SCOPE_H
#define SCOPE_H

#include <QString>
#include <QList>
#include <memory>
#include "Arithmetic/ScopeInfo.h"
#include "ExpressionParser.h"
#include "Arithmetic/Assignment.h"
#include "Arithmetic/Function.h"

class ClientConnection;

class Scope : public QObject
{
    Q_OBJECT

private:
    QString name;
    ExpressionParser *parser;
    CAS::ScopeInfo scope_info;
    QList<ClientConnection *> clients;

public:
    Scope() {};
    Scope(const QString &name, ExpressionParser *parser) : name(name), parser(parser) {};

    void getInitInfo(QDataStream &stream) const;
    void removeClient(ClientConnection *client);
    void sendMsg(const QString &sender, const QString &msg);
    void addClient(ClientConnection *client);
    bool hasClient(ClientConnection *candidate) { return clients.contains(candidate); }

signals:
    void output(const QString &);
};

#endif //SCOPE_H
