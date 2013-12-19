#include <QStringList>

#include "deviceinfo.h"

DeviceInfo::DeviceInfo(QObject* parent)
  : QObject(parent)
  , addr()
  , port()
  , info() {
}

/*
* !cECNnnnnnn/ppppp/dd/iiiiiiiiiiii  ex.) Device is AV Receiver TX-NR609, North American model, and ISCP Port number is 60128
* c: device category                 1: AV Receiver or Stereo Receiver
* nnnnnnn: model name of device      TX-NR609, (variable length [ max. 64 letters ])
* ppppp: ISCP port number            60128, (variable length [ max. 64 letters ])
* dd: destinaton area of device      DX: North American model, XX: European or Asian model, JJ: Japanese model, (fixed 2 characters)
* iiiiiiiiiiii: Identifier           0009B0****** ex.) Mac Address, (variable length [ max. 12 letters ])
* /: Separater
*/
QString DeviceInfo::toString() {
  QStringList tmp = info.toString().split("/");

  if (tmp.size() == 4) {
    model = tmp[0];

    if (model.size() > 4) {
      model.remove(0, 3);
    }
  }

  return model;
}
