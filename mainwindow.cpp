#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(const QString& version, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Custom Twitch Transcode Client v" + version);

    set_up_videoplayer();

    client = new StreamClient();
    client_thread = new QThread();

    client->moveToThread(client_thread);
    client_thread->start();

    ui->portLineEdit->setValidator(new QIntValidator(1, 65536, this));

    connect(this, SIGNAL(connect_client(const QString&, const int)), client, SLOT(connect_to_server(const QString&, const int)));
    connect(this, SIGNAL(disconnect_client()), client, SLOT(disconnect_from_server()));
    connect(this, SIGNAL(request_chunk()), client, SLOT(request_chunk()));

    connect(ui->ipLineEdit, SIGNAL(returnPressed()), this, SLOT(connection_change()));
    connect(ui->portLineEdit, SIGNAL(returnPressed()), this, SLOT(connection_change()));
    connect(ui->connectionButton, SIGNAL(clicked()), this, SLOT(connection_change()));
    connect(ui->controlButton, SIGNAL(clicked()), this, SLOT(video_control_change()));
    connect(ui->audioControlButton, SIGNAL(clicked()), this, SLOT(audio_control_change()));
    connect(ui->volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(volume_change(int)));
    connect(client, SIGNAL(server_connected()), this, SLOT(server_connected()));
    connect(client, SIGNAL(server_disconnected()), this, SLOT(server_disconnected()));
    connect(client, SIGNAL(chunk_downloaded(QByteArray*)), this, SLOT(chunk_downloaded(QByteArray*)));
    connect(playerA, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(player_state_changed(QMediaPlayer::State)));
    connect(playerB, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(player_state_changed(QMediaPlayer::State)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connection_change()
{
    if (client->connected)
    {
        emit disconnect_client();
        if (playing) video_control_change();
        return;
    }

    int port = ui->portLineEdit->text().toInt();
    if (port < 1 || port > 65536) return;

    emit connect_client(ui->ipLineEdit->text(), port);
}

void MainWindow::video_control_change()
{
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
    playerA->setVolume(value);
    playerB->setVolume(value);

    ui->volumeLabel->setText(QString::number(value) + "%");
}

void MainWindow::set_up_videoplayer()
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

void MainWindow::chunk_downloaded(QByteArray *chunk)
{
    if (playing_buffer == 'A')
    {
        bufferB->buffer().append(*chunk);
        if (bufferB->buffer().size() >= minimum_required_bytes) ui->controlButton->setEnabled(true);
    } else {
        bufferA->buffer().append(*chunk);
        if (bufferA->buffer().size() >= minimum_required_bytes) ui->controlButton->setEnabled(true);
    }

    qDebug() << "chunk size: " << chunk->size();
    qDebug() << "buffer A size: " << bufferA->size();
    qDebug() << "buffer B size: " << bufferB->size();
    qDebug() << "player A position: " << playerA->position();
    qDebug() << "player B position: " << playerB->position();

    emit request_chunk();

    delete chunk;
}

void MainWindow::player_state_changed(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState)
    {
        if (playing_buffer == 'A')
        {
            playing_buffer = 'B';
            playerB->setVideoOutput(video);
            playerB->play();
            bufferA->buffer().clear();
            playerA->setMedia(QMediaContent(), bufferA);
        } else {
            playing_buffer = 'A';
            playerA->setVideoOutput(video);
            playerA->play();
            bufferB->buffer().clear();
            playerB->setMedia(QMediaContent(), bufferB);
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
