#include "Network.h"

#include <QHostAddress>
#include "iscpmessage.h"
#include <arpa/inet.h>

Network::Network(QObject* parent) : QObject(parent) {
  qDebug() <<  __PRETTY_FUNCTION__;
  connect(&tcp, SIGNAL(connected()), parent, SLOT(connected()));
  connect(&tcp, SIGNAL(disconnected()), parent, SLOT(disconnected()));
  connect(&tcp, SIGNAL(readyRead()), this, SLOT(readData()));

  broadcastSocket.bind(QHostAddress::Any, 60128, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
  connect(&broadcastSocket, SIGNAL(readyRead()), this, SLOT(readBroadcastDatagram()));
  
  hostAddress.append(QHostAddress("127.0.0.1"));

  foreach (const QHostAddress & address, QNetworkInterface::allAddresses()) {
    if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
      qDebug() <<  __PRETTY_FUNCTION__ << "Localhost: " << address.toString();
      hostAddress.append(address);
    }
  }
}

Network::~Network() {
  qDebug() <<  __PRETTY_FUNCTION__;
  disconnect();
  tcp.close();
}

QString Network::readData() {
  qDebug() <<  __PRETTY_FUNCTION__;
  QDataStream in(&tcp);

  if (curr_status.isNull()) {
    if (tcp.bytesAvailable() < IscpMessage::header_size)
      return "";

    char header[ IscpMessage::header_size ];
    in.readRawData(&header[0], IscpMessage::header_size);

    //check format
    if (
      header[0] != 'I'
      || header[1] != 'S'
      || header[2] != 'C'
      || header[3] != 'P'
    ) {
      curr_status.reset(0);
      tcp.abort();
      qWarning() <<  __PRETTY_FUNCTION__ << "Bad ISCP format message";
      return "";
    }

    quint32 dataSize = qFromBigEndian<qint32> (reinterpret_cast<uchar*>(&header[8]));

    curr_status.reset(new IscpMessage(dataSize));
    curr_status->bytes().prepend(header, sizeof(header));
  }

  if (tcp.bytesAvailable() < curr_status->message_size())
    return "";

  //else get data message

  in.readRawData(curr_status->message(), curr_status->message_size()) ;
  qDebug() <<  __PRETTY_FUNCTION__ << "Received message: " << curr_status->toString();
  QString tmp = curr_status->toString();
  curr_status.reset(0);
  emit filterMessage(tmp);
  return tmp;
}

void Network::discover() {
  qDebug() <<  __PRETTY_FUNCTION__;
  IscpMessage qry;
  qry.make_rawcommand("!xECNQSTN");
  QList<QNetworkInterface>    infs =  QNetworkInterface::allInterfaces();

  foreach (QNetworkInterface interface, infs) {
    foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
      if (!entry.broadcast().isNull() && entry.ip() != QHostAddress("127.0.0.1")) {
        broadcastSocket.writeDatagram(qry.bytes(), qry.bytes().size(), QHostAddress::Broadcast, 60128);
        qDebug() <<  __PRETTY_FUNCTION__ << "Broadcasting to " << entry.broadcast().toString();
      }
    }
  }
}
  
void Network::readBroadcastDatagram() {
  qDebug() <<  __PRETTY_FUNCTION__;
  while (broadcastSocket.hasPendingDatagrams())  {
    dev.info.bytes().resize(broadcastSocket.pendingDatagramSize());
    broadcastSocket.readDatagram(dev.info.bytes().data(), dev.info.bytes().size(), &dev.addr, &dev.port);
    qDebug() << __PRETTY_FUNCTION__ << "Evaluating response from: " << dev.addr.toString();
    qDebug() << __PRETTY_FUNCTION__ << "Information: " + dev.toString();

    if (!hostAddress.contains(dev.addr) && dev.info.isEISCP()) {
      qDebug() << __PRETTY_FUNCTION__ << "Found Onkyo device on: " + dev.addr.toString();
      qDebug() << __PRETTY_FUNCTION__ << "Information: " + dev.toString();
      start();
      return;
    }

    dev.clear();
  }
  qDebug() << __PRETTY_FUNCTION__ << "Done, nothing recieved";
}

void Network::start() {
  qDebug() << __PRETTY_FUNCTION__ << "Will connect to: " << dev.addr.toString() << ":" << ISCP_PORT;
  tcp.connectToHost(dev.addr, ISCP_PORT);
}

bool Network::isConnected() {
  qDebug() <<  __PRETTY_FUNCTION__;
  return tcp.isOpen();
}

void Network::disconnect() {
  qDebug() << __PRETTY_FUNCTION__ << "Will disconnect from: " << dev.addr.toString() << ":" << ISCP_PORT;

  if (tcp.isOpen()) {
    tcp.disconnectFromHost();
  }
}

void Network::command(const QString& cmd) {
  qDebug() << __PRETTY_FUNCTION__ << "Command sent: " << cmd;
  IscpMessage msg;
  tcp.write(msg.make_command(cmd));
}
