#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "Fsa.h"
#include <QHostAddress>
#include <QString>
#include <QUdpSocket>
#include <qlist.h>
#include <string>
#include <vector>

class Control {
public:
    Control()  = default;
    ~Control() = default;

    enum class ControlMode { POSITION, VELOCITY, CURRENT, PD };

    const std::map< const std::string, const ControlMode > controlModeMap{
        { "POSITION", ControlMode::POSITION },
        { "VELOCITY", ControlMode::VELOCITY },
        { "CURRENT", ControlMode::CURRENT },
        { "PD", ControlMode::PD },
    };

    typedef std::map< Control::ControlMode, std::map< std::string, std::vector< double > > > ControlData_t;

    ControlData_t controlData{
        {
            Control::ControlMode::POSITION,
            {
                { "POSITION", { 0.0 } },
                { "VELOCITY", { 0.0 } },
                { "CURRENT", { 0.0 } },
            },
        },
        {
            Control::ControlMode::VELOCITY,
            {
                { "VELOCITY", { 0.0 } },
                { "CURRENT", { 0.0 } },
            },
        },
        {
            Control::ControlMode::CURRENT,
            {

                { "CURRENT", { 0.0 } },

            },
        },
        {
            Control::ControlMode::PD,
            {
                { "POSITION", { 0.0 } },
                // { "VELOCITY", {} },
                { "CURRENT", { 0.0 } },
            },
        },
    };

    void broadcast( const QString& message, const QHostAddress& address, const quint16 port, QMap< QString, FSA_CONNECT::FSA >& fsaMap );
    int  enableFSA( FSA_CONNECT::FSA& fsa );
    int  setControlMode( const ControlMode& controlMode, FSA_CONNECT::FSA& fsa );
    void sendControlData( const ControlMode& controlMode, ControlData_t& controlData, FSA_CONNECT::FSA& fsa, const float& controlPeriod );
};

#endif  // COMMUNICATE_H