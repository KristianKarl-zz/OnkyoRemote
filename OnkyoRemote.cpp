#include "OnkyoRemote.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

#include "Network.h"
#include "ISCP.h"

#define CONNECT_STRING "Connect"
#define CONNECTING_STRING "Connecting"
#define DISCONNECTING_STRING "Disconnecting"
#define CONNECTED_STRING "Connected"

OnkyoRemote::OnkyoRemote()
{
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
     *  Layouts
     */

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(connectedLayout);
    layout->addLayout(volumeLayout);
    layout->addLayout(inputSelectorLayout);

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    network = new Network( this );
    network->start("192.168.1.17", ISCP_PORT);
}

OnkyoRemote::~OnkyoRemote()
{
    delete network;
}

void OnkyoRemote::volumeUp()
{
    network->transfer("MVL", "UP");
}

void OnkyoRemote::volumeDown()
{
    network->transfer("MVL", "DOWN");
}

void OnkyoRemote::connected()
{
    connectBtn->setText(CONNECTED_STRING);
    connectBtn->setPalette(QPalette(Qt::green));
}

void OnkyoRemote::disconnected()
{
    connectBtn->setText(CONNECT_STRING);
    connectBtn->setPalette(QPalette(Qt::red));
}

void OnkyoRemote::changeConnectionStatus()
{
    qDebug() << "Text: " << connectBtn->text();
    if (!network->isConnected()) {
        network->start("192.168.1.17", ISCP_PORT);
        connectBtn->setText(CONNECTING_STRING);
        connectBtn->setPalette(QPalette(Qt::yellow));
    } else {
        network->disconnect();
        connectBtn->setText(DISCONNECTING_STRING);
        connectBtn->setPalette(QPalette(Qt::yellow));
    }
}

void OnkyoRemote::cblSat()
{
    network->transfer("SLI", "01");
}

void OnkyoRemote::game()
{
    network->transfer("SLI", "02");
}

void OnkyoRemote::radio()
{
    network->transfer("SLI", "24");
}

void OnkyoRemote::spotify()
{
    network->transfer("SLI", "28");
    QTime dieTime= QTime::currentTime().addSecs(10);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    network->transfer("NSV", "0A");
}

#include "OnkyoRemote.moc"
