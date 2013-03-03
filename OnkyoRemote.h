#ifndef OnkyoRemote_H
#define OnkyoRemote_H

#include <QtGui/QMainWindow>

class Network;
class QPushButton;

class OnkyoRemote : public QMainWindow
{
Q_OBJECT
public:
    OnkyoRemote();
    virtual ~OnkyoRemote();

public slots:
    void connected();
    void disconnected();
    void changeConnectionStatus();
    void volumeUp();
    void volumeDown();
    void cblSat();
    void game();
    void radio();
    void spotify();

private:
  Network* network;
  QPushButton *connectBtn;
};

#endif // OnkyoRemote_H
