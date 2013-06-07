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
    void setDisplay(const QString &);
    void setVolume(int);
  public slots:
    void command(const QString& cmd);
    QString readData();
  private:
    void parseStatus(QString status);
  private:
    QList<QHostAddress> hostAddress;
    QScopedPointer<IscpMessage> curr_status;
    DeviceInfo  dev;
    QTcpSocket tcp;
};

#endif // NETWORK_H
