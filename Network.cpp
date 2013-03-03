#include "Network.h"

#include <QHostAddress>
#include <arpa/inet.h>

Network::Network(QObject* parent): QObject(parent)
{
    connect(&client, SIGNAL(connected()), parent, SLOT(connected()));
    connect(&client, SIGNAL(disconnected()), parent, SLOT(disconnected()));
}

Network::~Network()
{
    client.close();
}

void Network::start(QString address, quint16 port)
{
    QHostAddress addr(address);
    client.connectToHost(addr, port);
}

void Network::transfer(const QString& cmd, const QString& parameter)
{
    int result = 0;
    char *buffer_p = NULL;
    unsigned int bufferSize = 0;
    struct ISCP::MSG *msg_p;

    bufferSize = sizeof(*msg_p) + parameter.length() + 2;
    buffer_p = (char*)calloc(1, bufferSize);

    if (!buffer_p)
    {
        return;
    }

    msg_p = (struct ISCP::MSG *) buffer_p;

    memcpy(msg_p->header.id, "ISCP", 4);
    msg_p->header.headerSize = htonl(sizeof(msg_p->header));
    msg_p->header.dataSize = htonl(sizeof(msg_p->data) +
                                   parameter.length() +
                                   1);
    msg_p->header.version = 1;

    msg_p->data.start = (uint8_t) '!';
    msg_p->data.dest = (uint8_t) '1';
    memcpy(msg_p->data.cmd, cmd.toStdString().c_str(), sizeof(msg_p->data.cmd));
    sprintf(msg_p->data.param,
            "%s\r\n",
            parameter.toStdString().c_str());

    result = client.write(buffer_p, bufferSize);
    qDebug() << "Result: " << result;

    free(buffer_p);

}

bool Network::isConnected()
{
    return client.isOpen();
}

void Network::disconnect()
{
    client.disconnectFromHost();
}



#include "Network.moc"
