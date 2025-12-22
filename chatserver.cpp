#include "chatserver.h"
#include "serverworker.h"
#include <QJsonValue>
#include <QJsonObject>

chatServer::chatServer(QObject *parent): QTcpServer(parent)
{

}

void chatServer::incomingConnection(qintptr socketDescriptor)
{
    serverWorker *worker = new serverWorker(this);

    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }

    connect(worker, &serverWorker::logMessage, this, &chatServer::logMessage);
    connect(worker, &serverWorker::jsonReceived, this, &chatServer::jsonReceived);
    m_clients.append(worker);
    emit logMessage("新的用户连接上了。");
}

void chatServer::broadcast(const QJsonObject &message, serverWorker *exclude)
{
    for (serverWorker *worker : m_clients) {
        worker->sendJson(message);
    }
}

void chatServer::stopServer()
{
    close();
}

void chatServer::jsonReceived(serverWorker *sender, const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value("type");

    if (typeVal.isNull() || !typeVal.isString()) {
        return;
    }
    if (typeVal.toString().compare("message", Qt::CaseInsensitive) == 0) {
        const QJsonValue textVal = docObj.value("text");

        if (textVal.isNull() || !textVal.isString()) {
            return;
        }
        const QString text = textVal.toString().trimmed();
        if (text.isEmpty()) {
            return;
        }
        QJsonObject message;
        message["type"] = "message";
        message["text"] = text;
        message["sender"] = sender->userName();

        broadcast(message, sender);
    }
    else if (typeVal.toString().compare("login", Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value("text");

        if (usernameVal.isNull() || !usernameVal.isString()) {
            return;
        }

        sender->setUserName(usernameVal.toString());
        QJsonObject connectedMessage;
        connectedMessage["type"] = "newuser";
        connectedMessage["username"] = usernameVal.toString();

        broadcast(connectedMessage, sender);
    }
}
