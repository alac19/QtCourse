#include "serverworker.h"
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>

serverWorker::serverWorker(QObject *parent)
    : QObject{parent}
{
    m_serverSocket = new QTcpSocket(this);
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &serverWorker::OnReadyRead);
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &serverWorker::disconnectFromClient);
}

bool serverWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

QString serverWorker::userName()
{
    return m_userName;
}

void serverWorker::setUserName(QString user)
{
    m_userName = user;
}

void serverWorker::OnReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_6_9);

    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;

        if (socketStream.commitTransaction()) {
            // emit logMessage(QString::fromUtf8(jsonData));
            // sendMessage("I received message.");

            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()) {
                    emit logMessage(QJsonDocument(jsonDoc).toJson(QJsonDocument::Compact));
                    emit jsonReceived(this, jsonDoc.object());
                }
            }
        }
        else {
            break;
        }
    }
}

void serverWorker::sendMessage(const QString &text, const QString &type)
{
    if (m_serverSocket->state() != QAbstractSocket::ConnectedState) {
        return;
    }
    if (!text.isEmpty()) {
        QDataStream serverStreaam(m_serverSocket);
        serverStreaam.setVersion(QDataStream::Qt_6_9);

        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        serverStreaam << QJsonDocument(message).toJson();
    }
}

void serverWorker::sendJson(const QJsonObject &json)
{
    const QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);
    emit logMessage(QLatin1String("Sending to ") + userName() + QLatin1String(" - ") + QString::fromUtf8(jsonData));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_6_9);
    socketStream << jsonData;
}
