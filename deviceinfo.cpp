#include "deviceinfo.h"

DeviceInfo::DeviceInfo(QObject* parent)
  : QObject(parent)
  , addr()
  , port()
  , info() {
}

#include "deviceinfo.moc"
