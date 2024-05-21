#include "communicate.h"
#include "control.h"
#include "mainwindow.h"
#include <iostream>

void Communicate::broadcast( const QString& message, const QHostAddress& address, const quint16 port, QList< QHostAddress >& ipList ) {
    QUdpSocket udpSocket;
    QByteArray datagram = message.toUtf8();

    // UDP 广播
    udpSocket.writeDatagram( datagram, address, port );
    // 如果接收到回复，将回复者的IP地址添加到列表中
    if ( udpSocket.hasPendingDatagrams() ) {
        datagram.resize( udpSocket.pendingDatagramSize() );
        QHostAddress senderIP;
        quint16      senderPort;

        udpSocket.readDatagram( datagram.data(), datagram.size(), &senderIP, &senderPort );

        ipList.append( senderIP );

        std::cout << "Received from: " << senderIP.toString().toStdString() << std::endl;
    }
}

int Communicate::setControlMode( const ControlMode& controlMode, FSA_CONNECT::FSA& fsa, const std::string& ip ) {
    fsa.init( ip );
    switch ( controlMode ) {
    case ControlMode::POSITION:
        fsa.EnablePosControl();
        throw std::runtime_error( "Not implemented yet" );
        break;
    case ControlMode::VELOCITY:
        fsa.EnableVelControl();
        throw std::runtime_error( "Not implemented yet" );
        break;
    case ControlMode::CURRENT:
        fsa.EnableCurControl();
        throw std::runtime_error( "Not implemented yet" );
        break;
    case ControlMode::PD:
        fsa.EnablePDControl();
        throw std::runtime_error( "Not implemented yet" );
        break;
    default:
        break;
    }
    return 0;
}

void Communicate::sendControlData( const float& controlPeriod, const std::vector< float >& controlData, FSA_CONNECT::FSA& fsa, const std::string& ip ) {}