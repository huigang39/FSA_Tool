#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Fsa.h"
#include "communicate.h"
#include "control.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTimer>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow( QWidget* parent = nullptr );
    ~MainWindow();
    Ui::MainWindow                  ui;
    QTimer                          timer;
    Communicate                     communicate;
    Control                         control;
    QList< QHostAddress >           ipList;
    std::vector< FSA_CONNECT::FSA > fsaList;

    Communicate::ControlMode controlMode;
    Control::FunctionMode    functionMode;

private:
    void setupUI();
    void init();
    void updateUI();

private slots:
    void on_comboBox_controlMode_textActivated( const QString& currentControlMode );
    void on_comboBox_functionMode_textActivated( const QString& currentFunctionMode );
};
#endif  // MAINWINDOW_H
