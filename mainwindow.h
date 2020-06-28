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
    StreamClient *client;
    QMediaPlayer *playerA;
    QMediaPlayer *playerB;
    QVideoWidget *video;
    QThread *client_thread;
    QBuffer *bufferA;
    QBuffer *bufferB;
    char playing_buffer = 'B';
    bool playing = false;
    bool muted = false;

    void set_up_videoplayer();

private slots:
    void connection_change();
    void video_control_change();
    void audio_control_change();
    void volume_change(int value);
    void chunk_downloaded(QByteArray *chunk);
    void player_state_changed(QMediaPlayer::State state);
    void server_connected();
    void server_disconnected();

signals:
    void connect_client(const QString&, const int) const;
    void disconnect_client();
    void request_chunk();

};
#endif // MAINWINDOW_H
