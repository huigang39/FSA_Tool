#include "mainwindow.h"
#include <iostream>
#include <qhostaddress.h>
#include <qlist.h>

MainWindow::MainWindow( QWidget* parent ) : QMainWindow( parent ) {
    init();
    setupUI();
}

MainWindow::~MainWindow() = default;

void MainWindow::init() {
    connect( &timer, &QTimer::timeout, [ & ]() {
        QList< QHostAddress > lastIpList = ipList;
        communicate.broadcast( "Hello", QHostAddress( "192.168.137.255" ), 2333, ipList );
        if ( ipList != lastIpList ) {
            updateUI();
        }
    } );
    timer.start( 1000 );
}

void MainWindow::setupUI() {
    ui.setupUi( this );
    resize( 800, 600 );
}

void MainWindow::updateUI() {
    ui.comboBox_ipList->clear();
    for ( auto& ip : ipList ) {
        ui.comboBox_ipList->addItem( ip.toString() );
    }
}

void MainWindow::on_comboBox_controlMode_textActivated( const QString& currentControlMode ) {
    auto it = communicate.controlModeMap.find( currentControlMode.toStdString() );
    if ( it != communicate.controlModeMap.end() ) {
        controlMode = it->second;
        for ( auto& fsa : fsaList ) {
            for ( auto& ip : ipList ) {
                communicate.setControlMode( controlMode, fsa, ip.toString().toStdString() );
            }
        }
    }
    else {
        throw std::runtime_error( "Unknown control mode" );
    }
}

void MainWindow::on_comboBox_functionMode_textActivated( const QString& currentFunctionMode ) {
    auto it = control.functionModeMap.find( currentFunctionMode.toStdString() );
    if ( it != control.functionModeMap.end() ) {
        functionMode = it->second;
        control.setControlData( functionMode );
    }
    else {
        throw std::runtime_error( "Unknown function mode" );
    }
}