#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Fsa.h"
#include "FsaStatus.h"
#include "control.h"
#include "dataGenerater.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <map>
#include <qglobal.h>
#include <qtmetamacros.h>
#include <string>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class FSA_Tool;
}
QT_END_NAMESPACE

class ControlWorker : public QObject {
    Q_OBJECT
public:
    ControlWorker()  = default;
    ~ControlWorker() = default;

public slots:
    void sendControlData( Control& control, const FSA_CONNECT::Status::FSAModeOfOperation& controlMode, Control::ControlData_t& controlData, FSA_CONNECT::FSA& fsa, const double& controlPeriod ) {
        control.sendControlData( controlMode, controlData, fsa, controlPeriod );
    };
};

class FSA_Tool : public QMainWindow {
    Q_OBJECT

public:
    FSA_Tool( QWidget* parent = nullptr );
    ~FSA_Tool();

    Ui::FSA_Tool  ui;
    QThread       uiUpdateThread;
    QThread       dataSendThread;
    QTimer        boardcastTimer;
    QTimer        getPvcTimer;
    Control       control;
    DataGenerater dataGenerater;

    ControlWorker controlWorker;

    QMap< QString, FSA_CONNECT::FSA > fsaMap;

    FSA_CONNECT::Status::FSAControlWord     controlWord{ FSA_CONNECT::Status::FSAControlWord::SERVO_OFF };
    FSA_CONNECT::Status::FSAModeOfOperation controlMode{ FSA_CONNECT::Status::FSAModeOfOperation::POSITION_CONTROL };
    DataGenerater::FunctionMode             functionMode{ DataGenerater::FunctionMode::SineWave };

private:
    void setupUI();
    void init();
    void updateUI();

    void setControlDataVariable( std::map< DataGenerater::ControlDataVariable, double >& controlDataVariable );
    void setPidParamter( FSA_CONNECT::FSAConfig::FSAPIDParams& pidParameter );

    void test();
    void plotVector( const std::vector< double >& data );

private slots:
    const QString on_comboBox_ipList_textActivated( const QString& currentIP );
    void          on_comboBox_controlWord_textActivated( const QString& currentControlWord );
    void          on_comboBox_controlMode_textActivated( const QString& currentControlMode );
    void          on_comboBox_functionMode_textActivated( const QString& currentFunctionMode );

    void on_pushButton_setPidParamter_clicked();
    void on_pushButton_setControlWord_clicked();
    void on_pushButton_setControlMode_clicked();
    void on_pushButton_setFunctionMode_clicked();

signals:
    void dataSendThreadStart( Control& control, const FSA_CONNECT::Status::FSAModeOfOperation& controlMode, Control::ControlData_t& controlData, FSA_CONNECT::FSA& fsa, const double& controlPeriod );
};

#endif  // MAINWINDOW_H