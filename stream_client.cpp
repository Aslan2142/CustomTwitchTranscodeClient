#include "stream_client.h"

StreamClient::StreamClient(QObject *parent) : QObject(parent)
{
    client_socket = new QTcpSocket(this);

    connect(client_socket, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(client_socket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    connect(client_socket, SIGNAL(readyRead()), this, SLOT(on_data_read()));
}

void StreamClient::connect_to_server(const QString& host, const int port)
{
    client_socket->connectToHost(host, port);
}

void StreamClient::disconnect_from_server() const
{
    client_socket->disconnectFromHost();
}

void StreamClient::request_chunk() const
{
    QThread::msleep(500);
    QString request = "request_chunk";
    send_data(client_socket, request.toUtf8());
}

void StreamClient::send_data(QTcpSocket * const connection, const QByteArray& data) const
{
    connection->write(data);
    connection->flush();
    connection->waitForBytesWritten();
}

void StreamClient::on_connected()
{
    connected = true;
    host = client_socket->peerAddress().toString();
    port = client_socket->peerPort();
    emit server_connected();

    QThread::sleep(1);
    request_chunk();
}

void StreamClient::on_disconnected()
{
    connected = false;
    host = "";
    port = 0;
    emit server_disconnected();
}

void StreamClient::on_data_read()
{
    QByteArray data_all = client_socket->readAll();
    QList<QByteArray> data = data_all.split('-');

    if (data.length() > 0 && data[0] == "error")
    {
        disconnect_from_server();
        return;
    }

    if (chunk_buffer == nullptr) chunk_buffer = new QByteArray();

    if (data.length() > 1 && data_all.startsWith("start_of_chunk-"))
    {
        *chunk_buffer = "";
    }

    chunk_buffer->append(data_all);

    if (data_all.endsWith("-end_of_chunk"))
    {
        chunk_buffer->remove(0, 15);
        chunk_buffer->remove(chunk_buffer->length() - 13, 13);

        emit chunk_downloaded(chunk_buffer);
        chunk_buffer = nullptr;
    }
}
