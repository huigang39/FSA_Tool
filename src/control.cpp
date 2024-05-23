#include "control.h"
#include "Fsa.h"
#include "mainwindow.h"
#include <iostream>
#include <qhostaddress.h>
#include <qmap.h>
#include <vector>

void Control::broadcast( const QString& message, const QHostAddress& address, const quint16 port, QMap< QString, FSA_CONNECT::FSA > fsaMap ) {
    QUdpSocket udpSocket;
    QByteArray datagram = message.toUtf8();

    // UDP 广播
    udpSocket.writeDatagram( datagram, address, port );

    // 如果接收到回复，将回复者的 IPv4 地址添加到 Map 中
    if ( udpSocket.waitForReadyRead( 500 ) && udpSocket.hasPendingDatagrams() ) {
        datagram.resize( udpSocket.pendingDatagramSize() );
        QHostAddress senderIP;
        quint16      senderPort;
        QString      key;

        udpSocket.readDatagram( datagram.data(), datagram.size(), &senderIP, &senderPort );

        key = senderIP.toString().remove( "::ffff:" );

        if ( !fsaMap.contains( key ) ) {
            fsaMap.insert( key, FSA_CONNECT::FSA() );
            fsaMap.find( key )->init( key.toStdString() );
        }
    }
}

int Control::enableFSA( FSA_CONNECT::FSA& fsa ) {
    fsa.Enable();
    return 0;
}

int Control::setControlMode( const ControlMode& controlMode, FSA_CONNECT::FSA& fsa ) {
    switch ( controlMode ) {
    case ControlMode::POSITION:
        fsa.EnablePosControl();
        break;
    case ControlMode::VELOCITY:
        fsa.EnableVelControl();
        break;
    case ControlMode::CURRENT:
        fsa.EnableCurControl();
        break;
    case ControlMode::PD:
        fsa.EnablePDControl();
        break;
    default:
        throw std::runtime_error( "Unknown Control Mode" );
        break;
    }
    return 0;
}

void Control::sendControlData( const ControlMode& controlMode, ControlData_t& controlData, FSA_CONNECT::FSA& fsa, const float& controlPeriod ) {
    std::vector< double > pos;
    std::vector< double > vel;
    std::vector< double > cur;

    switch ( controlMode ) {
    case ControlMode::POSITION:
        pos = controlData.at( ControlMode::POSITION ).at( "POSITION" );
        vel = controlData.at( ControlMode::POSITION ).at( "VELOCITY" );
        cur = controlData.at( ControlMode::POSITION ).at( "CURRENT" );
        break;
    case ControlMode::VELOCITY:
        vel = controlData.at( ControlMode::VELOCITY ).at( "VELOCITY" );
        cur = controlData.at( ControlMode::VELOCITY ).at( "CURRENT" );
        break;
    case ControlMode::CURRENT:
        cur = controlData.at( ControlMode::CURRENT ).at( "CURRENT" );
        break;
    case ControlMode::PD:
        pos = controlData.at( ControlMode::PD ).at( "POSITION" );
        vel = controlData.at( ControlMode::PD ).at( "VELOCITY" );
        cur = controlData.at( ControlMode::PD ).at( "CURRENT" );
        break;
    default:
        throw std::runtime_error( "Unknown Control Mode" );
        break;
    }

    struct timespec ts;
    struct timespec now;

    for ( int i = 0; i < pos.size(); i++ ) {

        ts.tv_sec  = static_cast< time_t >( controlPeriod );
        ts.tv_nsec = static_cast< long long >( ( controlPeriod - ts.tv_sec ) * 1e9 );

        clock_gettime( CLOCK_MONOTONIC, &now );

        struct timespec nextPeriod = { now.tv_sec + ts.tv_sec, now.tv_nsec + ts.tv_nsec };
        while ( nextPeriod.tv_nsec >= 1000000000 ) {
            nextPeriod.tv_sec++;
            nextPeriod.tv_nsec -= 1000000000;
        }

        auto start = std::chrono::high_resolution_clock::now();

        // 发送数据
        switch ( controlMode ) {
        case ControlMode::POSITION:
            fsa.SetPosition( pos.at( i ), vel.at( i ), cur.at( 1 ) );
            break;
        case ControlMode::VELOCITY:
            fsa.SetVelocity( vel.at( i ), cur.at( i ) );
            break;
        case ControlMode::CURRENT:
            fsa.SetCurrent( cur.at( i ) );
            break;
        case ControlMode::PD:
            fsa.SetPosition( pos.at( i ), vel.at( i ), cur.at( i ) );
            break;
        default:
            throw std::runtime_error( "Unknown Control Mode" );
            break;
        }

        clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &nextPeriod, NULL );

        auto                            end     = std::chrono::high_resolution_clock::now();
        std::chrono::duration< double > elapsed = end - start;

        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    }
}