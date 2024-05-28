#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "Fsa.h"
#include "FsaConfig.h"
#include "FsaStatus.h"
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

    const std::map< const std::string, const FSA_CONNECT::Status::FSAControlWord > controlWordMap{
        { "Enable", FSA_CONNECT::Status::FSAControlWord::SERVO_ON },
        { "Disable", FSA_CONNECT::Status::FSAControlWord::SERVO_OFF },
        { "Calibrate Motor", FSA_CONNECT::Status::FSAControlWord::CALIBRATE_MOTOR },
        { "Calibrate ADC", FSA_CONNECT::Status::FSAControlWord::CALIBRATE_ADC },
        { "Clear Fault", FSA_CONNECT::Status::FSAControlWord::CLEAR_FAULT },
    };

    const std::map< const std::string, const FSA_CONNECT::Status::FSAModeOfOperation > controlModeMap{
        { "POSITION", FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL },
        { "VELOCITY", FSA_CONNECT::Status::FSAModeOfOperation::VELOCITY_CONTROL },
        { "CURRENT", FSA_CONNECT::Status::FSAModeOfOperation::CURRENT_CLOSE_LOOP_CONTROL },
        { "PD", FSA_CONNECT::Status::FSAModeOfOperation::PD_CONTROL },
    };

    typedef std::map< FSA_CONNECT::Status::FSAModeOfOperation, std::map< std::string, std::vector< double > > > ControlData_t;

    ControlData_t controlData{
        {
            FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL,
            {
                { "POSITION", { 0.0 } },
                { "VELOCITY", { 0.0 } },
                { "CURRENT", { 0.0 } },
            },
        },
        {
            FSA_CONNECT::Status::FSAModeOfOperation::VELOCITY_CONTROL,
            {
                { "VELOCITY", { 0.0 } },
                { "CURRENT", { 0.0 } },
            },
        },
        {
            FSA_CONNECT::Status::FSAModeOfOperation::CURRENT_CLOSE_LOOP_CONTROL,
            {

                { "CURRENT", { 0.0 } },

            },
        },
        {
            FSA_CONNECT::Status::FSAModeOfOperation::PD_CONTROL,
            {
                { "POSITION", { 0.0 } },
                { "VELOCITY", { 0.0 } },
                { "CURRENT", { 0.0 } },
            },
        },
    };

    std::map< FSA_CONNECT::Status::FSAModeOfOperation, double > pvcNow = {
        { FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL, 0.0 },
        { FSA_CONNECT::Status::FSAModeOfOperation::VELOCITY_CONTROL, 0.0 },
        { FSA_CONNECT::Status::FSAModeOfOperation::CURRENT_CLOSE_LOOP_CONTROL, 0.0 },
    };

    FSA_CONNECT::FSAConfig::FSAPIDParams pidParameter;

    void broadcast( const QString& message, const QHostAddress& address, const quint16 port, QMap< QString, FSA_CONNECT::FSA >& fsaMap );
    int  setPidParamter( FSA_CONNECT::FSAConfig::FSAPIDParams& pidParameter, FSA_CONNECT::FSA& fsa );
    int  setControlWord( const FSA_CONNECT::Status::FSAControlWord& controlWord, FSA_CONNECT::FSA& fsa );
    int  setControlMode( const FSA_CONNECT::Status::FSAModeOfOperation& controlMode, FSA_CONNECT::FSA& fsa );
    void sendControlData( const FSA_CONNECT::Status::FSAModeOfOperation& controlMode, ControlData_t& controlData, FSA_CONNECT::FSA& fsa, const double& controlPeriod );
};

#endif  // COMMUNICATE_H