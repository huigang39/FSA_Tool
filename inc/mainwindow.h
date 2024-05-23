#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "control.h"
#include "dataGenerater.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <map>
#include <string>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class FSA_Tool;
}
QT_END_NAMESPACE

class FSA_Tool : public QMainWindow {
    Q_OBJECT

public:
    FSA_Tool( QWidget* parent = nullptr );
    ~FSA_Tool();

    Ui::FSA_Tool  ui;
    QThread       uiUpdateThread;
    QTimer        boardcastTimer;
    QTimer        getPvcTimer;
    Control       control;
    DataGenerater dataGenerater;

    QMap< QString, FSA_CONNECT::FSA > fsaMap;

    Control::ControlMode        controlMode{ Control::ControlMode::POSITION };
    DataGenerater::FunctionMode functionMode{ DataGenerater::FunctionMode::SineWave };

private:
    void setupUI();
    void init();
    void updateUI();

    void setControlDataVariable( std::map< DataGenerater::ControlDataVariable, double >& controlDataVariable );

    void test();
    void plotVector( const std::vector< double >& data );

private slots:
    const QString on_comboBox_fsaList_textActivated( const QString& currentIP );
    void          on_comboBox_controlMode_textActivated( const QString& currentControlMode );
    void          on_comboBox_functionMode_textActivated( const QString& currentFunctionMode );

    void on_pushButton_enableFSA_clicked();
    void on_pushButton_setControlMode_clicked();
    void on_pushButton_setFunctionMode_clicked();
};
#endif  // MAINWINDOW_H