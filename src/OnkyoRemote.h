#ifndef OnkyoRemote_H
#define OnkyoRemote_H

#include <QtGui/QMainWindow>

#include "PlayList.h"

class Network;
class QPushButton;
class QLabel;
class QSlider;
class QListView;

class OnkyoRemote : public QMainWindow {
    Q_OBJECT
  public:
    OnkyoRemote();
    virtual ~OnkyoRemote();

  public slots:
    void setInputSelection();
    void filterMessage(QString);
    void getInitialStatus();
    void setVolume();
    void volumeSliderMoved(int);
    void setPreset(const QString &);
    void setDisplay(const QString &);
    void connected();
    void disconnected();
    void changeConnectionStatus();
    void volumeUp();
    void volumeDown();
    void cblSat();
    void game();
    void radio();
    void spotify();
    void pc();
    void radioUp();
    void radioDown();

  private:
    void handleList(QString);
    void initListView();
    void handlePlayList(QString);

  private:
    Network* network;
    QPushButton *connectBtn;
    QLabel *displayText;
    QLabel *presetText;
    QSlider *volumeSlider;
    QListView *listView;
    QPushButton *cblSatBtn;
    QPushButton *gameBtn;
    QPushButton *radioBtn;
    QPushButton *spotifyBtn;
    QPushButton *pcBtn;

    // Status
  private:
    bool muted;
    bool power;
    bool musicOptimizer;
    QString mode;
    int masterVolume;
    QString imageString;
    QString timeInfo;
    QString artist;
    QString album;
    QString songName;
    int inputSelected;
    char repeatStatus;
    char shuffleStatus;
    char playStatus;
    QStringList  listViewdata;
    int listCursor;
    PlayList wholePlayList;
    int currentPlaylist;
    int playListSelected;
    QString lastListName;
};

#endif // OnkyoRemote_H
