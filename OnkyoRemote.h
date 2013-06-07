#ifndef OnkyoRemote_H
#define OnkyoRemote_H

#include <QtGui/QMainWindow>

class Network;
class QPushButton;
class QLabel;

class OnkyoRemote : public QMainWindow {
    Q_OBJECT
  public:
    OnkyoRemote();
    virtual ~OnkyoRemote();

  public slots:
    void setDisplay(const QString &);
    void connected();
    void disconnected();
    void querySelector();
    void changeConnectionStatus();
    void volumeUp();
    void volumeDown();
    void cblSat();
    void game();
    void radio();
    void spotify();
    void radioUp();
    void radioDown();

  private:
    Network* network;
    QPushButton *connectBtn;
    QLabel *displayText;
};

#endif // OnkyoRemote_H
