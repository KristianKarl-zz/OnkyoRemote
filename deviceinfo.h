#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QByteArray>
#include "iscpmessage.h"

class DeviceInfo: public QObject {
    Q_OBJECT

  public:
    explicit DeviceInfo(QObject* parent = 0);
    QString  toString() const  {
      return addr.toString() + ";" + info.toString();
    }

    QHostAddress addr;
    quint16      port;
    IscpMessage  info;
};


#endif // DEVICEINFO_H
