#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "unitlin.h"
#include "config.h"

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
    void drawCDULocalIPAddress();
    void drawCDULocalPort();
    void drawCDURemoteIPAddress();
    void drawCDURemotePort();
    void drawPCLocalIPAddress();
    void drawPCLocalPort();
    void drawPCRemoteIPAddress();
    void drawPCRemotePort();

    bool testCDULocalIPAddress();
    bool testCDURemoteIPAddress();
    bool testPCLocalIPAddress();
    bool testPCRemoteIPAddress();

    bool testCDULocalPort();
    bool testCDURemotePort();
    bool testPCLocalPort();
    bool testPCRemotePort();

private slots:
    void on_lineEdit_textChanged(const QString& arg1);

    void on_lineEdit_2_textChanged(const QString& arg1);

    void on_lineEdit_3_textChanged(const QString& arg1);

    void on_lineEdit_4_textChanged(const QString& arg1);

    void on_lineEdit_5_cursorPositionChanged(int arg1, int arg2);

    void on_lineEdit_18_textChanged(const QString& arg1);

    void on_lineEdit_19_textChanged(const QString& arg1);

    void on_lineEdit_24_textChanged(const QString& arg1);

    void on_lineEdit_25_textChanged(const QString& arg1);

    void on_pushButton_2_released();

    void on_lineEdit_27_textChanged(const QString& arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_released();

private:
    Ui::MainWindow* ui;
    UNITLIN* pDevice;
    CONFIG* pConfig;
};

#endif  // MAINWINDOW_H
