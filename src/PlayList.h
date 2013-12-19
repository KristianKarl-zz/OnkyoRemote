#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QString>
#include <QStringList>

class PlayList {
  public:
    PlayList() {};
    PlayList(QString title, int length, int menuDepth, int startFlag);
    QString title;
    QStringList SongTitles;
    int length;
    int menuDepth;
    bool looped;
    int loopStartFlag;
    int currentTenStartsAt;

};

#endif // PLAYLIST_H
