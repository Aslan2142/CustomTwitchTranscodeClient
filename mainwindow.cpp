#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(const QString& version, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Setup UI
    ui->setupUi(this);
    setWindowTitle("Custom Twitch Transcode Client v" + version);
    set_up_videoplayer();

    //Setup Client
    client = new StreamClient();
    client_thread = new QThread();
    client->moveToThread(client_thread);
    client_thread->start();

    //Allow only valid ports in UI
    ui->portLineEdit->setValidator(new QIntValidator(1, 65536, this));

    //Conenct signals to the client object
    connect(this, SIGNAL(connect_client(const QString&, const int)), client, SLOT(connect_to_server(const QString&, const int)));
    connect(this, SIGNAL(disconnect_client()), client, SLOT(disconnect_from_server()));
    connect(this, SIGNAL(request_chunk()), client, SLOT(request_chunk()));

    //Connect UI signals
    connect(ui->ipLineEdit, SIGNAL(returnPressed()), this, SLOT(connection_change()));
    connect(ui->portLineEdit, SIGNAL(returnPressed()), this, SLOT(connection_change()));
    connect(ui->connectionButton, SIGNAL(clicked()), this, SLOT(connection_change()));
    connect(ui->controlButton, SIGNAL(clicked()), this, SLOT(video_control_change()));
    connect(ui->audioControlButton, SIGNAL(clicked()), this, SLOT(audio_control_change()));
    connect(ui->volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(volume_change(int)));

    //Connect client object signals
    connect(client, SIGNAL(server_connected()), this, SLOT(server_connected()));
    connect(client, SIGNAL(server_disconnected()), this, SLOT(server_disconnected()));
    connect(client, SIGNAL(chunk_downloaded(QByteArray*)), this, SLOT(chunk_downloaded(QByteArray*)));

    //Connect player object signals
    connect(playerA, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(player_state_changed(QMediaPlayer::State)));
    connect(playerB, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(player_state_changed(QMediaPlayer::State)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_up_videoplayer() //Sets up buffers and video players
{
    playerA = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    playerB = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    video = new QVideoWidget(this);

    ui->graphicsView->show();
    ui->graphicsView->setViewport(video);

    bufferA = new QBuffer();
    bufferA->open(QIODevice::ReadWrite);
    playerA->setMedia(QMediaContent(), bufferA);

    bufferB = new QBuffer();
    bufferB->open(QIODevice::ReadWrite);
    playerB->setMedia(QMediaContent(), bufferB);
}

void MainWindow::connection_change()
{
    //If client is connected, disconnect it
    if (client->connected)
    {
        emit disconnect_client();
        if (playing) video_control_change();
        return;
    }

    //Check is port is valid
    int port = ui->portLineEdit->text().toInt();
    if (port < 1 || port > 65536) return;

    //Try to connect to the server
    emit connect_client(ui->ipLineEdit->text(), port);
}

void MainWindow::video_control_change()
{
    //Stop if the video is playing, otherwise play it
    if (playing)
    {
        playing = false;
        ui->controlButton->setText("Play");
        if (playing_buffer == 'A') playerA->pause();
        else playerB->pause();
    } else {
        playing = true;
        ui->controlButton->setText("Pause");
        if (playing_buffer == 'A') playerA->play();
        else playerB->play();
    }
}

void MainWindow::audio_control_change()
{
    //Unmute if the sound is muted,
    if (muted)
    {
        muted = false;
        ui->audioControlButton->setText("Mute");
        playerA->setMuted(false);
        playerB->setMuted(false);
    } else {
        muted = true;
        ui->audioControlButton->setText("Unmute");
        playerA->setMuted(true);
        playerB->setMuted(true);
    }
}

void MainWindow::volume_change(int value)
{
    //Set the volume
    playerA->setVolume(value);
    playerB->setVolume(value);

    ui->volumeLabel->setText(QString::number(value) + "%");
}

void MainWindow::chunk_downloaded(QByteArray *chunk)
{
    chunk_counter++;

    ui->controlButton->setEnabled(true); //Enable play/pause button

    //Switch buffers and copy new chunk data to the buffer
    //Remove old buffer data from the memory
    if (playing_buffer == 'A')
    {
        delete bufferB;
        bufferB = new QBuffer();
        bufferB->setData(*chunk);
        bufferB->open(QIODevice::ReadWrite);
    } else {
        delete bufferA;
        bufferA = new QBuffer();
        bufferA->setData(*chunk);
        bufferA->open(QIODevice::ReadWrite);
    }

    //Start playing the chunk if the player is stopped
    if (playerA->state() == QMediaPlayer::StoppedState && playerB->state() == QMediaPlayer::StoppedState)
    {
        playing = true;
        ui->controlButton->setText("Pause");
        player_state_changed(QMediaPlayer::StoppedState);
    }

    qDebug() << "chunk size: " << chunk->size();
    qDebug() << "buffer A size: " << bufferA->size();
    qDebug() << "buffer B size: " << bufferB->size();
    qDebug() << "player A position: " << playerA->position();
    qDebug() << "player B position: " << playerB->position();

    emit request_chunk(); //Request an another chunk from the server

    delete chunk; //Remove temporary chunk from the memory
}

void MainWindow::player_state_changed(QMediaPlayer::State state)
{
    //If the played is stopped,
    if (state == QMediaPlayer::StoppedState)
    {
        //Request a new chunk if there is no chunk in the memory
        if (--chunk_counter == 0)
        {
            emit request_chunk();
        }

        if (playing_buffer == 'A')
        {
            playing_buffer = 'B';
            playerB->setMedia(QMediaContent(), bufferB);
            playerB->setVideoOutput(video);
            playerB->play();
        } else {
            playing_buffer = 'A';
            playerA->setMedia(QMediaContent(), bufferA);
            playerA->setVideoOutput(video);
            playerA->play();
        }
    }
}

void MainWindow::server_connected()
{
    ui->connectionButton->setText("Disconnect");
    ui->connectionLabel->setText("Connected to " + client->host + ":" + QString::number(client->port));

    ui->controlButton->setEnabled(false);
}

void MainWindow::server_disconnected()
{
    ui->connectionButton->setText("Connect");
    ui->connectionLabel->setText("Disconnected");

    ui->controlButton->setEnabled(false);
}
