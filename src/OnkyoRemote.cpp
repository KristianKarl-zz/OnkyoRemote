#include "OnkyoRemote.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListView>
#include <QtGui/QButtonGroup>
#include <QStringListModel>

#include "Network.h"

OnkyoRemote::OnkyoRemote() :
  muted(false),
  power(false),
  musicOptimizer(false),
  mode(""),
  masterVolume(0),
  imageString(""),
  timeInfo(""),
  artist(""),
  album(""),
  songName(""),
  inputSelected(0),
  repeatStatus('-'),
  shuffleStatus('-'),
  playStatus('-'),
  listCursor(0),
  currentPlaylist(0),
  playListSelected(0),
  lastListName("") {
  qDebug() <<  __PRETTY_FUNCTION__;
  
  setWindowIcon(QIcon(":/img/onkyo.png"));
  
  temporaryDisplayTextTimer = new QTimer(this);
  connect(temporaryDisplayTextTimer, SIGNAL(timeout()), this, SLOT(restoreDisplayText()));

  initListView();

  /*
   *  Connection
   */
  connectLabel = new QLabel("Searching for Onkyo receiver...");
  connectLabel->setStyleSheet("qproperty-alignment: AlignCenter; background-color: red;");
  QHBoxLayout *connectedLayout = new QHBoxLayout;
  connectedLayout->addWidget(connectLabel);

  /*
   *  Volume
   */
  QPushButton *volumeUp = new QPushButton("Volume Up");
  connect(volumeUp, SIGNAL(clicked()), this, SLOT(volumeUp()));

  volumeSlider = new QSlider(Qt::Horizontal);
  volumeSlider->setTickInterval(100);
  connect(volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeSliderMoved(int)));

  QPushButton *volumeDown = new QPushButton("Volume Down");
  connect(volumeDown, SIGNAL(clicked()), this, SLOT(volumeDown()));

  QHBoxLayout *volumeLayout = new QHBoxLayout;
  volumeLayout->addWidget(volumeDown);
  volumeLayout->addWidget(volumeSlider);
  volumeLayout->addWidget(volumeUp);

  /*
   *  Input selectors
   */
  cblSatBtn = new QPushButton("CBL/SAT");
  cblSatBtn->setCheckable(true);
  connect(cblSatBtn, SIGNAL(clicked()), this, SLOT(cblSat()));

  gameBtn = new QPushButton("Game/TV");
  gameBtn->setCheckable(true);
  connect(gameBtn, SIGNAL(clicked()), this, SLOT(game()));

  radioBtn = new QPushButton("Radio");
  radioBtn->setCheckable(true);
  connect(radioBtn, SIGNAL(clicked()), this, SLOT(radio()));

  spotifyBtn = new QPushButton("Spotify");
  spotifyBtn->setCheckable(true);
  connect(spotifyBtn, SIGNAL(clicked()), this, SLOT(spotify()));

  pcBtn = new QPushButton("PC");
  pcBtn->setCheckable(true);
  connect(pcBtn, SIGNAL(clicked()), this, SLOT(pc()));

  netBtn = new QPushButton("NET");
  netBtn->setCheckable(true);
  connect(netBtn, SIGNAL(clicked()), this, SLOT(net()));

  QButtonGroup *btnGroup = new QButtonGroup();
  btnGroup->setExclusive(true);
  btnGroup->addButton(gameBtn);
  btnGroup->addButton(cblSatBtn);
  btnGroup->addButton(radioBtn);
  btnGroup->addButton(spotifyBtn);
  btnGroup->addButton(pcBtn);
  btnGroup->addButton(netBtn);

  QHBoxLayout *inputSelectorLayout = new QHBoxLayout;
  inputSelectorLayout->addWidget(gameBtn);
  inputSelectorLayout->addWidget(cblSatBtn);
  inputSelectorLayout->addWidget(radioBtn);
  inputSelectorLayout->addWidget(spotifyBtn);
  inputSelectorLayout->addWidget(pcBtn);
  inputSelectorLayout->addWidget(netBtn);

  /*
   *  Radio
   */
  QPushButton *radioDown = new QPushButton("Down");
  connect(radioDown, SIGNAL(clicked()), this, SLOT(radioDown()));

  QPushButton *radioUp = new QPushButton("Up");
  connect(radioUp, SIGNAL(clicked()), this, SLOT(radioUp()));

  QHBoxLayout *radioLayout = new QHBoxLayout;
  radioLayout->addWidget(radioDown);
  radioLayout->addWidget(radioUp);

  /*
   *  Display
   */
  displayText = new QLabel("No value yet");
  displayText->setAlignment(Qt::AlignCenter);

  presetText = new QLabel("No value yet");
  presetText->setAlignment(Qt::AlignCenter);

  QHBoxLayout *displayLayout = new QHBoxLayout;
  displayLayout->addWidget(displayText);
  displayLayout->addWidget(presetText);

  /*
   *  List widget
   */
  listView = new QListView();
  listView->setModel(new QStringListModel(listViewdata));
  QHBoxLayout *listLayout = new QHBoxLayout;
  listLayout->addWidget(listView);

  /*
   *  Layouts
   */

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(connectedLayout);
  layout->addLayout(volumeLayout);
  layout->addLayout(inputSelectorLayout);
  layout->addLayout(radioLayout);
  layout->addLayout(displayLayout);
  layout->addLayout(listLayout);

  setCentralWidget(new QWidget);
  centralWidget()->setLayout(layout);

  network = new Network(this);
  connect(network, SIGNAL(filterMessage(QString)), this, SLOT(filterMessage(QString)));

  network->discover();
}

OnkyoRemote::~OnkyoRemote() {
  qDebug() <<  __PRETTY_FUNCTION__;
  delete network;
}

void OnkyoRemote::volumeUp() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("MVLUP");
}

void OnkyoRemote::volumeDown() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("MVLDOWN");
}

void OnkyoRemote::radioUp() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("PRSUP");
}

void OnkyoRemote::radioDown() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("PRSDOWN");
}

void OnkyoRemote::connected() {
  qDebug() <<  __PRETTY_FUNCTION__;
  connectLabel->setText(QString("Connected to %1, at %2").arg(network->getDevice()->model).arg(network->getDevice()->addr.toString()));
  connectLabel->setStyleSheet("background-color: lightgreen;");
  getInitialStatus();
}

void OnkyoRemote::disconnected() {
  qDebug() <<  __PRETTY_FUNCTION__;
  connectLabel->setText("Not connected to any Onkyo receiver");
  connectLabel->setStyleSheet("background-color: red;");
}

void OnkyoRemote::cblSat() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("SLI01");
}

void OnkyoRemote::game() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("SLI02");
}

void OnkyoRemote::radio() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("SLI24");
  network->command("TUNQSTN");
}

void OnkyoRemote::spotify() {
  qDebug() <<  __PRETTY_FUNCTION__;
  setDisplay("Switching up Spotify");
  network->command("SLI28");
  QTime dieTime = QTime::currentTime().addSecs(10);

  while (QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

  network->command("NSV0A");
}

void OnkyoRemote::pc() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("SLI05");
}

void OnkyoRemote::net() {
  qDebug() <<  __PRETTY_FUNCTION__;
  network->command("SLI2B");
}

void OnkyoRemote::setDisplay(const QString& text) {
  qDebug() <<  __PRETTY_FUNCTION__ << text;
  displayText->setText(text);
  originalDisplayText = text;
}

void OnkyoRemote::setTemporaryDisplay(const QString& text) {
  qDebug() <<  __PRETTY_FUNCTION__ << text;
  displayText->setText(text);
  temporaryDisplayTextTimer->start(1000);
}

void OnkyoRemote::restoreDisplayText() {
  qDebug() <<  __PRETTY_FUNCTION__;
  displayText->setText(originalDisplayText);
  temporaryDisplayTextTimer->stop();
}

void OnkyoRemote::setPreset(const QString& text) {
  qDebug() <<  __PRETTY_FUNCTION__ << text;
  presetText->setText(text);
}

void OnkyoRemote::setVolume() {
  qDebug() << __PRETTY_FUNCTION__ << masterVolume;
  volumeSlider->setValue(masterVolume);
  volumeSlider->setToolTip(QString::number(masterVolume));
  setTemporaryDisplay(QString("Volume: %1").arg(masterVolume));
}

void OnkyoRemote::volumeSliderMoved(int value) {
  qDebug() << __PRETTY_FUNCTION__ << value;
  network->command(QString("MVL%1").arg(QString::number(value, 16)));
}

void OnkyoRemote::getInitialStatus() {
  qDebug() << __PRETTY_FUNCTION__;
  // ask for power
  network->command("PWRQSTN");
  // ask for volume
  network->command("MVLQSTN");
  // ask for input selected
  network->command("SLIQSTN");
  // ask for muted
  network->command("AMTQSTN");
  // ask for music optimizer
  network->command("MOTQSTN");
}

void OnkyoRemote::filterMessage(QString message) {
  qDebug() << __PRETTY_FUNCTION__ << message;
  bool ok;
  if (message.length() <= 3) {}
  else if (message.endsWith("N/A")) {}
  else if (message.startsWith("PWR")) {
    power = message.endsWith("01");;
  } else if (message.startsWith("AMT")) {
    muted = message.endsWith("01");
  } else if (message.startsWith("MOT")) {
    musicOptimizer = message.endsWith("01");
  } else if (message.startsWith("MVL")) {
    masterVolume = message.split("MVL")[1].toInt(&ok, 16);
    setVolume();
  } else if (message.startsWith("NTM")) {
    timeInfo = message.split("NTM")[1];
  } else if (message.startsWith("NAT")) {
    artist = message.split("NAT")[1];
  } else if (message.startsWith("NAL")) {
    album = message.split("NAL")[1];
  } else if (message.startsWith("NAT")) {
    artist = message.split("NAT")[1];
  } else if (message.startsWith("NTI")) {
    songName = message.split("NTI")[1];
    setPreset(songName);
  } else if (message.startsWith("SLI")) {
    inputSelected = message.split("SLI")[1].toInt();
    setInputSelection();
  } else if (message.startsWith("TUN")) {
    setDisplay("Tuner FM " + message.split("TUN")[1]);
    radioBtn->setChecked(true);
  } else if (message.startsWith("PRS")) {
    setPreset(QString("Preset: %1").arg(message.split("PRS")[1].toInt()));
  } else if (message.startsWith("NST")) {
    //playSettings(message.split("NST")[1]);
  } else if (message.startsWith("NLS")) {
    if (message.contains("No Item")) {
      qDebug() << __PRETTY_FUNCTION__ << "No items, exiting";
      return;
    }
    handleList(message.split("NLS")[1]);
  } else if (message.startsWith("NKY")) {
    //waitInput(message.split("NKY")[1]);
  } else if (message.startsWith("NJA")) {
    makeJacketArt(message.split("NJA")[1]);
  } else if (message.startsWith("NTR")) {
    //listSizeData(message.split("NTR")[1]);
  } else if (message.startsWith("SWL")) {
    //tempSubwooferLvl = getVolumeData(message.split("SWL")[1]);
  } else if (message.startsWith("CTL")) {
    //tempCenterLvl = getVolumeData(message.split("CTL")[1]);
  } else if (message.startsWith("TFR")) {
    //trebleLevel = getTrebleData(message.split("TFR")[1]);
    //subLevel = getSubData(message.split("TFR")[1]);
  } else if (message.startsWith("NLT")) {
    handleNetMessage(message.split("NLT")[1]);
  }
}

void OnkyoRemote::setInputSelection() {
  qDebug() <<  __PRETTY_FUNCTION__;
  switch (inputSelected) {
    case 01:
      emit setDisplay("Cable/Satelite");
      cblSatBtn->setChecked(true);
      break;

    case 02:
      emit setDisplay("Game/TV");
      gameBtn->setChecked(true);
      break;

    case 24:
      emit setDisplay("Tuner");
      network->command("TUNQSTN");
      network->command("PRSQSTN");
      radioBtn->setChecked(true);
      break;

    case 28:
      emit setDisplay("Spotify");
      break;
  }
}

void OnkyoRemote::handleNetMessage(QString line) {
  qDebug() <<  __PRETTY_FUNCTION__ << line;

  netBtn->setChecked(true);

  // DLNA
  if (line.startsWith("00")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "DLNA";
  }
  // Favorite
  else if (line.startsWith("01")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Favorit";
  }
  // vTuner
  else if (line.startsWith("02")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "vTuner";
    setDisplay("vTuner");

    // UI Type
    if (line[2] == '0') {
      qDebug() <<  __PRETTY_FUNCTION__ << "List";
    } else if (line[2] == '1') {
      qDebug() <<  __PRETTY_FUNCTION__ << "Menu";
    } else if (line[2] == '2') {
      qDebug() <<  __PRETTY_FUNCTION__ << "Playback";
    } else if (line[2] == '3') {
      qDebug() <<  __PRETTY_FUNCTION__ << "Popup";
    } else if (line[2] == '4') {
      qDebug() <<  __PRETTY_FUNCTION__ << "Keyboard";
    } else if (line[2] == '5') {
      qDebug() <<  __PRETTY_FUNCTION__ << "Menu List";
    } else {
      qWarning() <<  __PRETTY_FUNCTION__ << "Unsupported UI Type";
    }

    // Layer Info
    if (line[3] == '0') {
      qDebug() <<  __PRETTY_FUNCTION__ << "NET TOP";
    } else if (line[3] == '1') {
      qDebug() <<  __PRETTY_FUNCTION__ << "Service Top,DLNA/USB/iPod Top";
    } else if (line[3] == '2') {
      qDebug() <<  __PRETTY_FUNCTION__ << "under 2nd Layer";
    } else {
      qWarning() <<  __PRETTY_FUNCTION__ << "Unsupported Layer info";
    }

    // Current cursor position
    qDebug() <<  __PRETTY_FUNCTION__ << "Current cursor position: " + line.mid(4 ,4);

    // Number of list items
    qDebug() <<  __PRETTY_FUNCTION__ << "Number of list items: " + line.mid(8 ,4);

    // Numberof of layers
    qDebug() <<  __PRETTY_FUNCTION__ << "Number of layers: " + line.mid(12 ,2);

    // Reserved
    qDebug() <<  __PRETTY_FUNCTION__ << "Reserved: " + line.mid(14 ,2);

    // Icon left of Title Bar
    qDebug() <<  __PRETTY_FUNCTION__ << "Icon left of Title Bar: " + line.mid(16 ,2);

    // Icon type?
    qDebug() <<  __PRETTY_FUNCTION__ << "Icon Icon type?: " + line.mid(18 ,2);

    // Icon right of Title Bar
    qDebug() <<  __PRETTY_FUNCTION__ << "Icon right of Title Bar: " + line.mid(20 ,2);

    // Icon type?
    qDebug() <<  __PRETTY_FUNCTION__ << "Icon Icon type?: " + line.mid(22 ,2);

    // Status info
    qDebug() <<  __PRETTY_FUNCTION__ << "Status info: " + line.mid(24 ,2);

    // Status info type?
    qDebug() <<  __PRETTY_FUNCTION__ << "Status info type?: " + line.mid(26 ,2);
    
    // Character of Title Bar 
    qDebug() <<  __PRETTY_FUNCTION__ << "Character of Title Bar: " + line.mid(28);
  }
  // SiriusXM
  else if (line.startsWith("03")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "SiriusXM";
  }
  // Pandora
  else if (line.startsWith("04")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Pandora";
  }
  // Rhapsody
  else if (line.startsWith("05")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Rhapsody";
  }
  // Last.fm
  else if (line.startsWith("06")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Last.fm";
  }
  // Napster
  else if (line.startsWith("07")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Napster";
  }
  // Slacker
  else if (line.startsWith("08")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Slacker";
  }
  // Mediafly
  else if (line.startsWith("09")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Mediafly";
  }
  // Spotify
  else if (line.startsWith("0A")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Spotify";
  }
  // AUPEO!
  else if (line.startsWith("0B")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "AUPEO!";
  }
  // radiko
  else if (line.startsWith("0C")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "radiko";
  }
  // e-onkyo
  else if (line.startsWith("0D")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "e-onkyo";
  }
  // TuneIn Radio
  else if (line.startsWith("0E")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "TunIn Radio";
  }
  // MP3 Tunes
  else if (line.startsWith("0F")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "MP3 Tunes";
  }
  // Simfy
  else if (line.startsWith("10")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "Simfy";
  }
  // HomeMedia
  else if (line.startsWith("11")) {
    qDebug() <<  __PRETTY_FUNCTION__ << "HomeMedia";
  }
  else {
    qWarning() <<  __PRETTY_FUNCTION__ << "Unsupported service type";
  }
}

void OnkyoRemote::handleList(QString line) {
  qDebug() <<  __PRETTY_FUNCTION__ << line;
  if (line.startsWith("A") || line.startsWith("U")) {
    int position = QStringRef(&line, 1, 2).string()->toInt();
    QString listText = line.right(3);
    if (listText.contains("ģÃû")) {
      listText = listText.replace("ģÃû", "-");
    }
    listViewdata[position] = listText;
    wholePlayList.SongTitles[wholePlayList.currentTenStartsAt + position] = (wholePlayList.currentTenStartsAt + position + 1) + ". " + listText;
  } else if (line.startsWith("C")) {
    if (line.at(1) !=  '-') {
      listCursor = QStringRef(&line, 1, 2).string()->toInt();
    } else {
      listCursor = 0;
    }
    if (line.endsWith("P")) {
      initListView();
    }
  }
}

void OnkyoRemote::handlePlayList(QString string) {
  //numbers 0,1 is network source type in ascii (Byte 1)
  //numbers 2,3 is menu depth in hex (byte 2)
  //numbers 4,5,6,7 are the cursor position in hex (byte3,4)
  //numbers 8,9,10,11 are the total items in the list in hex(byte 5,6)
  qDebug() <<  __PRETTY_FUNCTION__ << string;
  if (string.length() > 22) {
    bool ok;
    QString name = string.right(22);
    int listlength = QStringRef(&string, 8, 12).string()->toInt(&ok, 16);
    if (listlength == 0) {
      ;
    }

    int listcursorpos = QStringRef(&string, 4, 8).string()->toInt(&ok, 16);
    int listStartAt = listcursorpos / 10 * 10;
    int menuDepth = QStringRef(&string, 2, 3).string()->toInt();

    if (wholePlayList.title == name && wholePlayList.menuDepth == menuDepth && wholePlayList.length == listlength) {
      // if at end of list say it is looped and go back to list start.
      wholePlayList.currentTenStartsAt = listStartAt;
      if (listStartAt == wholePlayList.loopStartFlag && wholePlayList.looped == false) {
        wholePlayList.looped = true;
      }

      if (wholePlayList.looped == false) {
        network->command("NTCRIGHT");
      }
    } else {
      wholePlayList = PlayList(name, listlength, menuDepth, listStartAt);
      network->command("NTCRIGHT");
    }
  }
}

void OnkyoRemote::initListView() {
  qDebug() <<  __PRETTY_FUNCTION__;
  listViewdata.clear();
  listViewdata.append("-");
  listViewdata.append("-");
  listViewdata.append("-");
  listViewdata.append("-");
  listViewdata.append("-");
  listViewdata.append("-");
  listViewdata.append("-");
  listViewdata.append("-");
  listViewdata.append("-");
  listViewdata.append("-");
}

void OnkyoRemote::makeJacketArt(QString jacketArtStr) {
  qDebug() <<  __PRETTY_FUNCTION__ + jacketArtStr;
}