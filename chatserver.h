#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QObject>
#include <serverworker.h>

class chatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit chatServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
    QVector<serverWorker *>m_clients;

    void broadcast(const QJsonObject &message, serverWorker *exclude);

signals:
    void logMessage(const QString &msg);

public slots:
    void stopServer();
    void jsonReceived(serverWorker *sender, const QJsonObject &docObj);
};

#endif // CHATSERVER_H
