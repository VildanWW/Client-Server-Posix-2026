#include "client.h"
#include "Settings.h"
#include <iostream>
#include <QSettings>

Client::Client(QObject *parent) : QObject{parent} {
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

void Client::onReadyRead() {
    while (socket->bytesAvailable() >= sizeof(PacketData)) {
        PacketData packet{};
        socket->peek(reinterpret_cast<char*>(&packet), sizeof(PacketData));

        int fullPacketSize = sizeof(PacketData) + packet.dataSize;

        if (socket->bytesAvailable() < fullPacketSize) {
            break;
        }

        socket->read(reinterpret_cast<char*>(&packet), sizeof(PacketData));
        QByteArray bufferRecived = socket->read(packet.dataSize);

        packet.senderName[sizeof(packet.senderName) - 1] = '\0';
        QString authorName = QString::fromUtf8(packet.senderName);

        emit messageReceived(packet.packetType, authorName, bufferRecived);
    }
}

bool Client::connectToServer(const std::string& ip, const QString& userName) {
    if(!socket) {
        return false;
    }

    socket->connectToHost(QString::fromStdString(ip), ServerConfig::port);

    if(socket->waitForConnected(ClientConfig::timeWaitingConnected)) {
        std::cout << "Connected successfully\n";
        clientName = userName;
        return true;
    }

    std::cerr << "Don't to connected to the server\n";
    return false;
}

bool Client::sendToData(DataType type, const QByteArray &sendBuffer) {
    if(!socket || socket->state() != QAbstractSocket::ConnectedState) {
        return false;
    }

    PacketData packet;
    memset(&packet, 0, sizeof(PacketData));

    packet.packetType = type;
    packet.dataSize = sendBuffer.size();

    std::string stdName = clientName.toStdString();

    size_t copiedBytes = stdName.copy(packet.senderName, sizeof(packet.senderName) - 1);
    packet.senderName[copiedBytes] = '\0';

    QByteArray block;
    block.reserve(sizeof(PacketData) + sendBuffer.size());

    block.append(reinterpret_cast<const char*>(&packet), sizeof(PacketData));
    block.append(sendBuffer);

    socket->write(block);
    return socket->flush();
}

bool Client::stop() {
    if(socket && socket->state()!=QAbstractSocket::UnconnectedState) {
        socket->disconnectFromHost();
        socket->waitForDisconnected(ClientConfig::timeWaitingDisconnected);
    }
    return true;
}

Client::~Client() {
    stop();
    std::cout<<"Destructor worked!\n";
}

