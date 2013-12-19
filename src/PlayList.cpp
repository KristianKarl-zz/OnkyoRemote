#include "PlayList.h"

PlayList::PlayList(QString title, int length, int menuDepth, int startFlag) {
  this->title = title;
  this->length = length;
  if (length == 0)
    length = 1;
  this->menuDepth = menuDepth;
  this->looped = false;
  this->loopStartFlag = startFlag;
  this->SongTitles = QStringList();
}
