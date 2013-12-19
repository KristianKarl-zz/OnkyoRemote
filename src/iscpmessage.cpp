#include "iscpmessage.h"
#include "QString"
#include "QDataStream"


const int IscpMessage::header_size = 16;

IscpMessage::IscpMessage(int sz)
  :
  block(sz, 0) {
}

IscpMessage::IscpMessage(const QByteArray&  ba)
  :
  block(ba) {

}

QString IscpMessage::toString() const {
  QString tmp;

  if (message_size() > 5) {
    tmp = QString::fromUtf8(message() + 2, message_size() - 2).replace(QChar(0x1A), "").simplified();
  }

  return  tmp;
}

void IscpMessage::make_rawcommand(const QString &cmd) {
  QByteArray req = cmd.toUtf8();

  block.clear();
  // setup tcp packet
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setByteOrder(QDataStream::BigEndian);
  out.writeRawData("ISCP", 4);    //magik
  out << qint32(header_size);     //header size
  out << (qint32) 0;              //empty data size, see down
  out << (qint8) 0x01 << (qint8) 0 << (qint8) 0 << (qint8) 0; // version + 3 reserved
  out.writeRawData(req.begin(), req.size());   // request messqage
  out << (qint8) 0x0A;            //LF

  out.device()->seek(8);  //set data size
  out << quint32(block.size() - header_size);
}

const QByteArray& IscpMessage::make_command(const QString &cmd) {
  make_rawcommand(QString("!1") + cmd);
  return block;
}

bool IscpMessage::isEISCP() {
  return
    block.at(0) == 'I'
    && block.at(1) == 'S'
    && block.at(2) == 'C'
    && block.at(3) == 'P';
}
