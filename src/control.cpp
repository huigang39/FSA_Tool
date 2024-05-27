#include "control.h"
#include "Fsa.h"
#include "FsaConfig.h"
#include "FsaStatus.h"
#include "mainwindow.h"
#include <iostream>
#include <qhostaddress.h>
#include <qmap.h>
#include <qthread.h>
#include <vector>

void Control::broadcast( const QString& message, const QHostAddress& address, const quint16 port, QMap< QString, FSA_CONNECT::FSA >& fsaMap ) {
    QUdpSocket udpSocket;
    QByteArray datagram = message.toUtf8();

    // UDP 广播
    udpSocket.writeDatagram( datagram, address, port );

    // 如果接收到回复，将回复者的 IPv4 地址添加到 Map 中
    if ( udpSocket.waitForReadyRead( 100 ) && udpSocket.hasPendingDatagrams() ) {
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

int Control::setPidParamter( FSA_CONNECT::FSAConfig::FSAPIDParams& pidParameter, FSA_CONNECT::FSA& fsa ) {
    fsa.SetPIDParams( pidParameter );
    return 0;
}

int Control::setControlMode( const FSA_CONNECT::Status::FSAModeOfOperation& controlMode, FSA_CONNECT::FSA& fsa ) {
    fsa.GetPVC( pvcNow.at( FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL ), pvcNow.at( FSA_CONNECT::Status::FSAModeOfOperation::VELOCITY_CONTROL ),
                pvcNow.at( FSA_CONNECT::Status::FSAModeOfOperation::CURRENT_CLOSE_LOOP_CONTROL ) );

    switch ( controlMode ) {
    case FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL:
        fsa.SetPosition( pvcNow.at( controlMode ), 0.0, 0.0 );
        fsa.EnablePosControl();
        break;
    case FSA_CONNECT::Status::FSAModeOfOperation::VELOCITY_CONTROL:
        fsa.SetVelocity( pvcNow.at( controlMode ), 0.0 );
        fsa.EnableVelControl();
        break;
    case FSA_CONNECT::Status::FSAModeOfOperation::CURRENT_CLOSE_LOOP_CONTROL:
        fsa.SetCurrent( pvcNow.at( controlMode ) );
        fsa.EnableCurControl();
        break;
    case FSA_CONNECT::Status::FSAModeOfOperation::PD_CONTROL:
        fsa.SetPosition( pvcNow.at( FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL ), 0.0, 0.0 );
        fsa.EnablePDControl();
        break;
    default:
        throw std::runtime_error( "Unknown Control Mode" );
        break;
    }
    return 0;
}

void Control::sendControlData( const FSA_CONNECT::Status::FSAModeOfOperation& controlMode, ControlData_t& controlData, FSA_CONNECT::FSA& fsa, const double& controlPeriod ) {
    std::vector< double > pos{};
    std::vector< double > vel{};
    std::vector< double > cur{};
    int                   size{};

    switch ( controlMode ) {
    case FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL:
        pos  = controlData.at( controlMode ).at( "POSITION" );
        vel  = controlData.at( controlMode ).at( "VELOCITY" );
        cur  = controlData.at( controlMode ).at( "CURRENT" );
        size = pos.size();
        break;
    case FSA_CONNECT::Status::FSAModeOfOperation::VELOCITY_CONTROL:
        vel  = controlData.at( controlMode ).at( "VELOCITY" );
        cur  = controlData.at( controlMode ).at( "CURRENT" );
        size = vel.size();
        break;
    case FSA_CONNECT::Status::FSAModeOfOperation::CURRENT_CLOSE_LOOP_CONTROL:
        cur  = controlData.at( controlMode ).at( "CURRENT" );
        size = cur.size();
        break;
    case FSA_CONNECT::Status::FSAModeOfOperation::PD_CONTROL:
        pos  = controlData.at( controlMode ).at( "POSITION" );
        vel  = controlData.at( controlMode ).at( "VELOCITY" );
        cur  = controlData.at( controlMode ).at( "CURRENT" );
        size = pos.size();
        break;
    default:
        throw std::runtime_error( "Unknown Control Mode" );
        break;
    }

    struct timespec ts {};
    struct timespec now {};

    for ( int i = 0; i < size; i++ ) {

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
        case FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL:
            try {
                fsa.SetPosition( pos.at( i ), vel.at( i ), cur.at( 1 ) );
            }
            catch ( const std::exception& e ) {
                std::cerr << e.what() << '\n';
            }
            break;
        case FSA_CONNECT::Status::FSAModeOfOperation::VELOCITY_CONTROL:
            try {
                fsa.SetVelocity( vel.at( i ), cur.at( i ) );
            }
            catch ( const std::exception& e ) {
                std::cerr << e.what() << '\n';
            }
            break;
        case FSA_CONNECT::Status::FSAModeOfOperation::CURRENT_CLOSE_LOOP_CONTROL:
            try {
                fsa.SetCurrent( cur.at( i ) );
            }
            catch ( const std::exception& e ) {
                std::cerr << e.what() << '\n';
            }
            break;
        case FSA_CONNECT::Status::FSAModeOfOperation::PD_CONTROL:
            try {
                fsa.SetPosition( pos.at( i ), vel.at( i ), cur.at( i ) );
            }
            catch ( const std::exception& e ) {
                std::cerr << e.what() << '\n';
            }
            break;
        default:
            throw std::runtime_error( "Unknown Control Mode" );
            break;
        }

        clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &nextPeriod, NULL );

        auto                            end     = std::chrono::high_resolution_clock::now();
        std::chrono::duration< double > elapsed = end - start;

        std::cout << "Position: " << pos.at( i ) << " Velocity: " << vel.at( i ) << " Current: " << cur.at( i ) << std::endl;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    }
}