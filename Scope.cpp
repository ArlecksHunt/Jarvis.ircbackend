#include "Scope.h"
#include "ClientConnection.h"

void Scope::getInitInfo(QDataStream &stream) const
{   
    stream << static_cast<quint32>(clients.length());
    for (const auto &client : clients) stream << client->nick();
    stream << static_cast<quint32>(scope_info.variables.size());
    for (const auto &item : scope_info.variables)
            stream << QString::fromStdString(item.first) << QString::fromStdString(item.second->toString());
    stream << static_cast<quint32>(scope_info.functions.size());
    for (const auto &item : scope_info.functions) {
        stream << QString::fromStdString(item.first.first) << static_cast<quint32>(item.second.first.size());
        for (const auto &arg : item.second.first) stream << QString::fromStdString(arg);
        stream << QString::fromStdString(item.second.second->toString());
    }
}

void Scope::removeClient(ClientConnection *client)
{
    if (clients.contains(client)) {
        clients.removeOne(client);
       for (const auto &it_client : clients) it_client->leaveClient(name, client->nick());
    }
}

void Scope::sendMsg(const QString &sender, const QString &msg)
{
    for (const auto &client : clients) client->sendMsg(name, sender, msg);
    try {
        std::unique_ptr<CAS::AbstractArithmetic> result = parser->parse(msg.toStdString());
        if (result->type() == CAS::AbstractArithmetic::ASSIGNMENT) {
            if (static_cast<CAS::Assignment*>(result.get())->getFirstOp()->type() == CAS::AbstractArithmetic::VARIABLE) {
                scope_info.variables[static_cast<CAS::Assignment*>(result.get())->getFirstOp()->toString()] =  std::shared_ptr<CAS::AbstractArithmetic>(static_cast<CAS::Assignment*>(result.get())->getSecondOp()->copy());
                for (const auto &client : clients)
                        client->newVariable(name, QString::fromStdString(static_cast<CAS::Assignment*>(result.get())->getFirstOp()->toString()), QString::fromStdString(static_cast<CAS::Assignment*>(result.get())->getSecondOp()->toString()));
                emit output("NewVariable(" + name + ", " + QString::fromStdString(static_cast<CAS::Assignment*>(result.get())->getFirstOp()->toString()) + ", " + QString::fromStdString(static_cast<CAS::Assignment*>(result.get())->getSecondOp()->toString()) + ")");
            } else if (static_cast<CAS::Assignment*>(result.get())->getFirstOp()->type() == CAS::AbstractArithmetic::FUNCTION) {
                const CAS::Function *func = static_cast<const CAS::Function*>(static_cast<CAS::Assignment*>(result.get())->getFirstOp().get());
                std::vector<std::string> argStrings;
                for (const auto &arg : func->getOperands()) argStrings.emplace_back(arg->toString());
                scope_info.functions[std::make_pair(func->getIdentifier(), argStrings.size())] = std::make_pair(argStrings, static_cast<CAS::Assignment*>(result.get())->getSecondOp()->copy());
                QStringList argQStrings;
                for (const auto &arg : argStrings) argQStrings.append(QString::fromStdString(arg));
                for (const auto &client : clients)
                        client->newFunction(name, QString::fromStdString(func->getIdentifier()), argQStrings, QString::fromStdString(static_cast<CAS::Assignment*>(result.get())->getSecondOp()->toString()));
                emit output("NewFunction(" + name + ", " + QString::fromStdString(func->getIdentifier()) + ", \"" + argQStrings.join(",") + "\", " +  QString::fromStdString(static_cast<CAS::Assignment*>(result.get())->getSecondOp()->toString()) + ")");
            }
        }
        QString resultString = QString::fromStdString(result->eval(scope_info)->toString());
        for (const auto &client : clients) client->sendMsg(name, "Jarvis", resultString);
    } catch (const char *) {
        for (const auto &client : clients) client->sendMsg(name, "Jarvis", "error bro");
    }
}

void Scope::addClient(ClientConnection *client)
{
    if (clients.contains(client)) throw 0;
    for (const auto &it_client : clients) it_client->enterClient(name, client->nick());
    clients.append(client);
}
