#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "Fsa.h"
#include <QHostAddress>
#include <QString>
#include <QUdpSocket>
#include <string>
#include <vector>

class Communicate : public QObject {
    Q_OBJECT
public:
    Communicate()  = default;
    ~Communicate() = default;

    enum class ControlMode { POSITION, VELOCITY, CURRENT, PD };
    const std::map< const std::string, const ControlMode > controlModeMap = { { std::string( "POSITION" ), ControlMode::POSITION },
                                                                              { std::string( "VELOCITY" ), ControlMode::VELOCITY },
                                                                              { std::string( "CURRENT" ), ControlMode::CURRENT },
                                                                              { std::string( "PD" ), ControlMode::PD } };

    void broadcast( const QString& message, const QHostAddress& address, const quint16 port, QList< QHostAddress >& ipList );
    int  setControlMode( const ControlMode& controlMode, FSA_CONNECT::FSA& fsa, const std::string& ip );
    void sendControlData( const float& controlPeriod, const std::vector< float >& controlData, FSA_CONNECT::FSA& fsa, const std::string& ip );
};

#endif  // COMMUNICATE_H