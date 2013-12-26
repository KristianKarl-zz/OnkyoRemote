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

#define CONNECT_STRING "Connect"
#define CONNECTING_STRING "Connecting"
#define DISCONNECTING_STRING "Disconnecting"
#define CONNECTED_STRING "Connected"

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
  connectBtn = new QPushButton(CONNECT_STRING);
  connect(connectBtn, SIGNAL(clicked()), this, SLOT(changeConnectionStatus()));
  connectBtn->setPalette(QPalette(Qt::red));
  QHBoxLayout *connectedLayout = new QHBoxLayout;
  connectedLayout->addWidget(connectBtn);

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

  QButtonGroup *btnGroup = new QButtonGroup();
  btnGroup->setExclusive(true);
  btnGroup->addButton(gameBtn);
  btnGroup->addButton(cblSatBtn);
  btnGroup->addButton(radioBtn);
  btnGroup->addButton(spotifyBtn);
  btnGroup->addButton(pcBtn);

  QHBoxLayout *inputSelectorLayout = new QHBoxLayout;
  inputSelectorLayout->addWidget(gameBtn);
  inputSelectorLayout->addWidget(cblSatBtn);
  inputSelectorLayout->addWidget(radioBtn);
  inputSelectorLayout->addWidget(spotifyBtn);
  inputSelectorLayout->addWidget(pcBtn);

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
  connectBtn->setText(QString("Connected to %1, at %2").arg(network->getDevice()->model).arg(network->getDevice()->addr.toString()));
  connectBtn->setPalette(QPalette(Qt::green));
  getInitialStatus();
}

void OnkyoRemote::disconnected() {
  qDebug() <<  __PRETTY_FUNCTION__;
  connectBtn->setText(CONNECT_STRING);
  connectBtn->setPalette(QPalette(Qt::red));
}

void OnkyoRemote::changeConnectionStatus() {
  qDebug() << __PRETTY_FUNCTION__ << "Text: " << connectBtn->text();

  if (!network->isConnected()) {
    network->discover();
    connectBtn->setText(CONNECTING_STRING);
    connectBtn->setPalette(QPalette(Qt::yellow));
  } else {
    network->disconnect();
    connectBtn->setText(DISCONNECTING_STRING);
    connectBtn->setPalette(QPalette(Qt::yellow));
  }
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
    handleList(message.split("NLS")[1]);
  } else if (message.startsWith("NKY")) {
    //waitInput(message.split("NKY")[1]);
  } else if (message.startsWith("NJA")) {
    //mainForm.netPlay.makeJacketArt(Arrays.copyOfRange(statusBytes, 3, statusBytes.length));
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
    handlePlayList(message.split("NLT")[1]);
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
