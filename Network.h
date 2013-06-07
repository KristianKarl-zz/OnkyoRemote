#ifndef NETWORK_H
#define NETWORK_H

#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>

#include "ISCP.h"
#include "deviceinfo.h"

class Network : public QObject {
    Q_OBJECT
  public:
    explicit Network(QObject* parent = 0);
    ~Network();
    void discover();
    void start();
    bool isConnected();
    void disconnect();
signals:
    void setDisplay(const QString &);
public slots:
    void command(const QString& cmd);
    void readData();
  private:
    void parseStatus(QString status);
  private:
    QList<QHostAddress> hostAddress;
    QScopedPointer<IscpMessage> curr_status;
    DeviceInfo  dev;
    QTcpSocket tcp;
};

#endif // NETWORK_H
