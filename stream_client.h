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
    void request_chunk() const; //Requests a new chunk from the server

private:
    QTcpSocket *client_socket;
    QByteArray *chunk_buffer = nullptr; //Used to temporarily store the incomplete buffer data

    void send_data(QTcpSocket * const connection, const QByteArray&) const; //Sends a data to the server

private slots:
    void on_connected(); //Called when client gets connected to the server
    void on_disconnected(); //Called when client gets disconnected from the server
    void on_data_read(); //Called when the data comes in and processes the data


signals:
    void chunk_downloaded(QByteArray *chunk);
    void server_connected();
    void server_disconnected();

};

#endif // STREAM_CLIENT_H
