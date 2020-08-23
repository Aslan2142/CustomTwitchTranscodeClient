#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>
#include <QBuffer>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QGraphicsVideoItem>
#include <QtConcurrent/QtConcurrent>
#include "stream_client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& version, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    StreamClient *client; //Communiactes with the server
    QMediaPlayer *playerA; //Used with bufferA
    QMediaPlayer *playerB; //Used with bufferB
    QVideoWidget *video; //Video object used by both players
    QThread *client_thread; //Thread on which client object is running on
    QBuffer *bufferA; //Chunk buffer
    QBuffer *bufferB; //Chunk buffer
    char playing_buffer = 'B'; //Used to indicate what buffer is currently playing
    bool playing = false; //Used to indicate in the playing playing or paused
    bool muted = false; //Used to indicate if the player is muted or not
    int chunk_counter = 0; //Used to track the number of chunks in the memory

    void set_up_videoplayer(); //Sets up the video player (called when window launches)

private slots:
    void connection_change(); //Called when user click on connect/disconnect button
    void video_control_change(); //Called when user clicks on play/pause button
    void audio_control_change(); //Called when user mutes/unmutes
    void volume_change(int value); //Called when user changes volume
    void chunk_downloaded(QByteArray *chunk); //Called when whule chunk gets successfully downloaded
    void player_state_changed(QMediaPlayer::State state); //Called when player changes state (gets paused or unpaused)
    void server_connected(); //Called when client gets connected to the server
    void server_disconnected(); //Called when client gets disconnected from the server

signals:
    void connect_client(const QString&, const int) const;
    void disconnect_client();
    void request_chunk();

};
#endif // MAINWINDOW_H
