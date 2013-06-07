#include <QtGui/QApplication>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QDebug>

#include <iostream>

#include "OnkyoRemote.h"

QTextStream *out = 0;

void logOutput(QtMsgType type, const char *msg) {
  const char* symbols[] = { "DEBUG", "WARNING", "CRITICAL", "FATAL" };

  QString logTtext = QString("%1 [%2] %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss::zzz")).arg(symbols[type]).arg(msg);
  (*out) << logTtext << endl;
  std::cerr << logTtext.toStdString() << std::endl;

  if (QtFatalMsg == type) {
    abort();
  }
}



int main(int argc, char** argv) {
  QApplication app(argc, argv);
  QString fileName;

  if (QCoreApplication::applicationFilePath().endsWith("exe", Qt::CaseInsensitive)) {
    fileName = QCoreApplication::applicationFilePath().replace(".exe", ".log");
  } else {
    fileName = QCoreApplication::applicationFilePath().append(".log");
  }

  QFile *log = new QFile(fileName);

  if (log->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    out = new QTextStream(log);
    qInstallMsgHandler(logOutput);
  } else {
    qDebug() << "Error opening log file '" << fileName << "'. All debug output redirected to console.";
  }

  OnkyoRemote onkyoRemote;
  onkyoRemote.show();
  return app.exec();
}


