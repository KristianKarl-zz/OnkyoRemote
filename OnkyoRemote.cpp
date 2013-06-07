#include "OnkyoRemote.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

#include "Network.h"

#define CONNECT_STRING "Connect"
#define CONNECTING_STRING "Connecting"
#define DISCONNECTING_STRING "Disconnecting"
#define CONNECTED_STRING "Connected"

OnkyoRemote::OnkyoRemote() {
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

  QPushButton *volumeDown = new QPushButton("Volume Down");
  connect(volumeDown, SIGNAL(clicked()), this, SLOT(volumeDown()));

  QHBoxLayout *volumeLayout = new QHBoxLayout;
  volumeLayout->addWidget(volumeDown);
  volumeLayout->addWidget(volumeUp);

  /*
   *  Input selectors
   */
  QPushButton *cblSat = new QPushButton("CBL/SAT");
  connect(cblSat, SIGNAL(clicked()), this, SLOT(cblSat()));

  QPushButton *game = new QPushButton("Game/TV");
  connect(game, SIGNAL(clicked()), this, SLOT(game()));

  QPushButton *radio = new QPushButton("Radio");
  connect(radio, SIGNAL(clicked()), this, SLOT(radio()));

  QPushButton *spotify = new QPushButton("Spotify");
  connect(spotify, SIGNAL(clicked()), this, SLOT(spotify()));

  QHBoxLayout *inputSelectorLayout = new QHBoxLayout;
  inputSelectorLayout->addWidget(game);
  inputSelectorLayout->addWidget(cblSat);
  inputSelectorLayout->addWidget(radio);
  inputSelectorLayout->addWidget(spotify);

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
  displayText = new QLabel("Standby");
  displayText->setAlignment(Qt::AlignCenter);

  QHBoxLayout *displayLayout = new QHBoxLayout;
  displayLayout->addWidget(displayText);

  /*
   *  Layouts
   */

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(connectedLayout);
  layout->addLayout(volumeLayout);
  layout->addLayout(inputSelectorLayout);
  layout->addLayout(radioLayout);
  layout->addLayout(displayLayout);

  setCentralWidget(new QWidget);
  centralWidget()->setLayout(layout);

  network = new Network(this);
  connect(network, SIGNAL(setDisplay(const QString &)), this, SLOT(setDisplay(const QString &)));
  network->discover();
}

OnkyoRemote::~OnkyoRemote() {
  delete network;
}

void OnkyoRemote::volumeUp() {
  network->command("MVLUP");
}

void OnkyoRemote::volumeDown() {
  network->command("MVLDOWN");
}

void OnkyoRemote::radioUp() {
  network->command("PRSUP");
}

void OnkyoRemote::radioDown() {
  network->command("PRSDOWN");
}

void OnkyoRemote::connected() {
  connectBtn->setText(CONNECTED_STRING);
  connectBtn->setPalette(QPalette(Qt::green));
  querySelector();
}

void OnkyoRemote::querySelector() {
  network->command("SLIQSTN");
}

void OnkyoRemote::disconnected() {
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
  network->command("SLI01");
  querySelector();
}

void OnkyoRemote::game() {
  network->command("SLI02");
  querySelector();
}

void OnkyoRemote::radio() {
  network->command("SLI24");
  querySelector();
  network->command("TUNQSTN");
}

void OnkyoRemote::spotify() {
  network->command("SLI28");
  QTime dieTime = QTime::currentTime().addSecs(10);
  while (QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  network->command("NSV0A");
  querySelector();
}

void OnkyoRemote::setDisplay(const QString& text) {
  displayText->setText(text);
}

#include "OnkyoRemote.moc"
