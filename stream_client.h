#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QList>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

class StreamClient : public QObject
{
    Q_OBJECT
public:
    explicit StreamClient(QObject *parent = nullptr);

    bool connected = false;
    QString host = "";
    int port = 0;

public slots:
    void connect_to_server(const QString& host, const int port);
    void disconnect_from_server() const;
    void request_chunk() const;

private:
    QTcpSocket *client_socket;
    QByteArray *chunk_buffer = nullptr;

    void send_data(QTcpSocket * const connection, const QByteArray&) const;
    void process_data(const QByteArray data);

private slots:
    void on_connected();
    void on_disconnected();
    void on_data_read();

signals:
    void chunk_downloaded(QByteArray *chunk);
    void server_connected();
    void server_disconnected();

};

#endif // STREAM_CLIENT_H
