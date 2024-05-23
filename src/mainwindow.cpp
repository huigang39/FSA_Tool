#include "mainwindow.h"
#include "Fsa.h"
#include "control.h"
#include "dataGenerater.h"
#include <iostream>
#include <qtimer.h>
#include <string>
#include <sys/socket.h>
#include <vector>

FSA_Tool::FSA_Tool( QWidget* parent ) : QMainWindow( parent ) {
    init();
    setupUI();
}

FSA_Tool::~FSA_Tool() = default;

void FSA_Tool::init() {
    static QMap< QString, FSA_CONNECT::FSA > lastFsaMap = fsaMap;
    connect( &boardcastTimer, &QTimer::timeout, [ & ]() {
        control.broadcast( "Is any fourier smart server here?", QHostAddress( "192.168.137.255" ), 2334, fsaMap );

        for ( auto& ip : fsaMap.keys() ) {
            std::cout << "IP: " << ip.toStdString() << std::endl;
        }

        for ( auto& ip : lastFsaMap.keys() ) {
            std::cout << "lastIP: " << ip.toStdString() << std::endl;
        }

        if ( lastFsaMap.keys() != fsaMap.keys() ) {
            lastFsaMap = fsaMap;
            updateUI();
        }
    } );

    connect( &getPvcTimer, &QTimer::timeout, [ & ]() {
        for ( auto& fsa : fsaMap ) {
            fsa.GetPVC( control.pvcNow.at( Control::ControlMode::POSITION ), control.pvcNow.at( Control::ControlMode::VELOCITY ), control.pvcNow.at( Control::ControlMode::CURRENT ) );
            ui.lcdNumber_position->display( control.pvcNow.at( Control::ControlMode::POSITION ) );
            ui.lcdNumber_velocity->display( control.pvcNow.at( Control::ControlMode::VELOCITY ) );
            ui.lcdNumber_current->display( control.pvcNow.at( Control::ControlMode::CURRENT ) );
        }
    } );

    boardcastTimer.moveToThread( &uiUpdateThread );
    getPvcTimer.moveToThread( &uiUpdateThread );

    connect( &uiUpdateThread, &QThread::started, [ & ]() { boardcastTimer.start( 3000 ); } );
    connect( &uiUpdateThread, &QThread::started, [ & ]() { getPvcTimer.start( 500 ); } );
}

void FSA_Tool::setupUI() {
    ui.setupUi( this );
    resize( 800, 600 );
}

void FSA_Tool::updateUI() {
    ui.comboBox_fsaList->clear();
    for ( auto& ip : fsaMap.keys() ) {
        ui.comboBox_fsaList->addItem( ip );
    }
}

const QString FSA_Tool::on_comboBox_fsaList_textActivated( const QString& currentIP ) {
    return currentIP;
}

void FSA_Tool::on_comboBox_controlMode_textActivated( const QString& currentControlMode ) {
    auto it = control.controlModeMap.find( currentControlMode.toStdString() );
    if ( it != control.controlModeMap.end() ) {
        controlMode = it->second;
    }
    else {
        throw std::runtime_error( "Unknown Control Mode" );
    }
}

void FSA_Tool::on_comboBox_functionMode_textActivated( const QString& currentFunctionMode ) {
    auto it = dataGenerater.functionModeMap.find( currentFunctionMode.toStdString() );
    if ( it != dataGenerater.functionModeMap.end() ) {
        functionMode = it->second;
        ui.stackedWidget_configControlData->setCurrentIndex( static_cast< int >( functionMode ) );
    }
    else {
        throw std::runtime_error( "Unknown Function Mode" );
    }
}

void FSA_Tool::on_pushButton_enableFSA_clicked() {
    if ( !ui.comboBox_fsaList->currentText().isEmpty() ) {
        fsaMap.find( on_comboBox_fsaList_textActivated( ui.comboBox_fsaList->currentText() ) ).value();
        control.enableFSA( fsaMap.find( on_comboBox_fsaList_textActivated( ui.comboBox_fsaList->currentText() ) ).value() );
    }
}

void FSA_Tool::on_pushButton_setControlMode_clicked() {
    if ( !ui.comboBox_fsaList->currentText().isEmpty() ) {
        control.setControlMode( controlMode, fsaMap.find( on_comboBox_fsaList_textActivated( ui.comboBox_fsaList->currentText() ) ).value() );
    }
}

void FSA_Tool::on_pushButton_setFunctionMode_clicked() {
    // test();
    if ( !ui.comboBox_fsaList->currentText().isEmpty() ) {
        setControlDataVariable( dataGenerater.controlDataVariable );

        for ( auto& innerMap : control.controlData ) {
            for ( auto& vector : innerMap.second ) {
                vector.second.clear();
            }
        }

        std::vector< double > pos;
        std::vector< double > vel;
        std::vector< double > cur;

        switch ( controlMode ) {
        case Control::ControlMode::POSITION:
            pos = dataGenerater.generateControlData( functionMode, dataGenerater.controlDataVariable );
            vel = std::vector< double >( pos.size(), 0.0 );
            cur = std::vector< double >( pos.size(), 0.0 );

            control.controlData.at( Control::ControlMode::POSITION ).at( "POSITION" ) = pos;
            control.controlData.at( Control::ControlMode::POSITION ).at( "VELOCITY" ) = vel;
            control.controlData.at( Control::ControlMode::POSITION ).at( "CURRENT" )  = cur;

            break;
        case Control::ControlMode::VELOCITY:
            vel = dataGenerater.generateControlData( functionMode, dataGenerater.controlDataVariable );
            cur = std::vector< double >( vel.size(), 0.0 );

            control.controlData.at( Control::ControlMode::VELOCITY ).at( "VELOCITY" ) = vel;
            control.controlData.at( Control::ControlMode::VELOCITY ).at( "CURRENT" )  = cur;

            break;
        case Control::ControlMode::CURRENT:
            cur = dataGenerater.generateControlData( functionMode, dataGenerater.controlDataVariable );

            control.controlData.at( Control::ControlMode::CURRENT ).at( "CURRENT" ) = cur;
            break;
        case Control::ControlMode::PD:
            pos = dataGenerater.generateControlData( functionMode, dataGenerater.controlDataVariable );
            vel = std::vector< double >( pos.size(), 0.0 );
            cur = std::vector< double >( pos.size(), 0.0 );

            control.controlData.at( Control::ControlMode::PD ).at( "POSITION" ) = pos;
            control.controlData.at( Control::ControlMode::PD ).at( "VELOCITY" ) = vel;
            control.controlData.at( Control::ControlMode::PD ).at( "CURRENT" )  = cur;

            break;
        default:
            throw std::runtime_error( "Unknown Control Mode" );
            break;
        }

        control.sendControlData( controlMode, control.controlData, fsaMap.find( on_comboBox_fsaList_textActivated( ui.comboBox_fsaList->currentText() ) ).value(), 0.002 );
    }
}

void FSA_Tool::setControlDataVariable( std::map< DataGenerater::ControlDataVariable, double >& controlDataVariable ) {
    controlDataVariable.at( DataGenerater::ControlDataVariable::Amplitude )      = ui.lineEdit_sinwWave_amplitude->text().toDouble();
    controlDataVariable.at( DataGenerater::ControlDataVariable::Frequency )      = ui.lineEdit_sinwWave_frequency->text().toDouble();
    controlDataVariable.at( DataGenerater::ControlDataVariable::StartFrequency ) = ui.lineEdit_sinwWave_phase->text().toDouble();
    controlDataVariable.at( DataGenerater::ControlDataVariable::EndFrequency )   = ui.lineEdit_sinwWave_duration->text().toDouble();
    controlDataVariable.at( DataGenerater::ControlDataVariable::Phase )          = ui.lineEdit_sinwWave_phase->text().toDouble();
    controlDataVariable.at( DataGenerater::ControlDataVariable::Duration )       = ui.lineEdit_sinwWave_duration->text().toDouble();
    controlDataVariable.at( DataGenerater::ControlDataVariable::SampleRate )     = ui.lineEdit_sinwWave_sampleRate->text().toDouble();
}

void FSA_Tool::test() {
    setControlDataVariable( dataGenerater.controlDataVariable );
    plotVector( dataGenerater.generateControlData( functionMode, dataGenerater.controlDataVariable ) );
}

void FSA_Tool::plotVector( const std::vector< double >& data ) {
    FILE* pipe = popen( "gnuplot -persist", "w" );

    if ( pipe ) {
        fprintf( pipe, "plot '-' with lines\n" );

        for ( size_t i = 0; i < data.size(); ++i ) {
            fprintf( pipe, "%zu %lf\n", i, data[ i ] );
        }

        fprintf( pipe, "e\n" );
        fflush( pipe );

        pclose( pipe );
    }
}
