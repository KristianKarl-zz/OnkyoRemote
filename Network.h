#ifndef NETWORK_H
#define NETWORK_H

#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>

#include "ISCP.h"

class Network : public QObject
{
    Q_OBJECT
public:
    Network(QObject* parent = 0);
    ~Network();
    void start(QString address, quint16 port);
    bool isConnected();
    void disconnect();
public slots:
     void transfer(const QString& cmd, const QString& parameter);
private:
    QTcpSocket client;
};

#endif // NETWORK_H
