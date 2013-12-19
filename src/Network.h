#ifndef NETWORK_H
#define NETWORK_H

#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>

#include "deviceinfo.h"

class Network : public QObject {
    Q_OBJECT
  public:
    explicit Network(QObject* parent = 0);
    ~Network();
    bool discover();
    void start();
    bool isConnected();
    void disconnect();
    DeviceInfo* getDevice() {
      return &dev;
    }
  signals:
    void filterMessage(QString);
  public slots:
    void command(const QString& cmd);
    QString readData();
    void readBroadcastDatagram();
  private:
    QList<QHostAddress> hostAddress;
    QScopedPointer<IscpMessage> curr_status;
    DeviceInfo  dev;
    QTcpSocket tcp;
    QUdpSocket broadcastSocket;
};

#endif // NETWORK_H
