#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef ANDROID
#include <QtAndroid>
#include <QtAndroidExtras>
#endif

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QString s0;
    QString s1;
    QString s2;
    QString s3;
    QString s4;

    ui->setupUi(this);
    pConfig = new CONFIG("");

    pDevice = new UNITLIN(pConfig);
    //
    drawCDULocalIPAddress();
    drawCDULocalPort();
    drawCDURemoteIPAddress();
    drawCDURemotePort();
    drawPCLocalIPAddress();
    drawPCLocalPort();
    drawPCRemoteIPAddress();
    drawPCRemotePort();

    ui->lineEdit_27->setText(pDevice->getPathToObjectsFiles());
    ui->checkBox->setChecked(pDevice->getRestorePCConnectionFlagState());
    //
    ui->lineEdit_2->setEnabled(false);
    ui->lineEdit_3->setEnabled(false);
    ui->lineEdit_4->setEnabled(false);
    ui->lineEdit_5->setEnabled(false);
    //
    ui->lineEdit_18->setEnabled(false);
    //
    ui->lineEdit_10->setEnabled(false);
    ui->lineEdit_11->setEnabled(false);
    ui->lineEdit_12->setEnabled(false);
    ui->lineEdit_13->setEnabled(false);

    ui->lineEdit_19->setEnabled(false);
    //
    ui->lineEdit_14->setEnabled(false);
    ui->lineEdit_15->setEnabled(false);
    ui->lineEdit_16->setEnabled(false);
    ui->lineEdit_17->setEnabled(false);
    //
    ui->lineEdit_24->setEnabled(false);
    //
    ui->lineEdit_6->setEnabled(false);
    ui->lineEdit_7->setEnabled(false);
    ui->lineEdit_8->setEnabled(false);
    ui->lineEdit_9->setEnabled(false);
    //
    ui->lineEdit_25->setEnabled(false);
    ui->lineEdit_27->setEnabled(false);
    //
    ui->pushButton_2->setEnabled(false);
    ui->checkBox->setEnabled(false);
    ui->lineEdit->setEnabled(true);

    pDevice->start();
    ui->startButtonwidget->show();
    ui->settingsWidget->hide();

    QTimer::singleShot(1000, this, &MainWindow::on_startCduButton_released);
}

MainWindow::~MainWindow()
{
    pDevice->stop();
    delete pDevice;
    delete pConfig;
    delete ui;
}

void MainWindow::on_lineEdit_textChanged(const QString& arg1)
{
    //
    pDevice->testPassword(arg1);
    //
    ui->lineEdit_2->setEnabled(true);
    ui->lineEdit_3->setEnabled(true);
    ui->lineEdit_4->setEnabled(true);
    ui->lineEdit_5->setEnabled(true);
    //
    ui->lineEdit_18->setEnabled(true);
    //
    ui->lineEdit_10->setEnabled(true);
    ui->lineEdit_11->setEnabled(true);
    ui->lineEdit_12->setEnabled(true);
    ui->lineEdit_13->setEnabled(true);

    ui->lineEdit_19->setEnabled(true);
    //
    ui->lineEdit_14->setEnabled(true);
    ui->lineEdit_15->setEnabled(true);
    ui->lineEdit_16->setEnabled(true);
    ui->lineEdit_17->setEnabled(true);
    //
    ui->lineEdit_24->setEnabled(true);
    //
    ui->lineEdit_6->setEnabled(true);
    ui->lineEdit_7->setEnabled(true);
    ui->lineEdit_8->setEnabled(true);
    ui->lineEdit_9->setEnabled(true);
    //
    ui->lineEdit_25->setEnabled(true);
    ui->lineEdit_27->setEnabled(true);
    //
    ui->pushButton_2->setEnabled(true);
    ui->checkBox->setEnabled(true);
    //
}

void MainWindow::drawCDULocalIPAddress()
{
    QString s0;
    QString s1;
    QString s2;
    QString s3;
    QString s4;
    s0 = pDevice->getCDULocalIPAddress();
    pDevice->scatterIPAddress(s4, s3, s2, s1, s0);
    ui->lineEdit_2->setText(s4);
    ui->lineEdit_3->setText(s3);
    ui->lineEdit_4->setText(s2);
    ui->lineEdit_5->setText(s1);
}

void MainWindow::drawCDULocalPort()
{
    QString s0;
    s0.setNum(pDevice->getCDULocalPort());
    ui->lineEdit_18->setText(s0);
}

void MainWindow::drawCDURemoteIPAddress()
{
    QString s0;
    QString s1;
    QString s2;
    QString s3;
    QString s4;

    s0 = pDevice->getCDURemoteIPAddress();
    pDevice->scatterIPAddress(s4, s3, s2, s1, s0);
    ui->lineEdit_10->setText(s4);
    ui->lineEdit_11->setText(s3);
    ui->lineEdit_12->setText(s2);
    ui->lineEdit_13->setText(s1);
}
//
void MainWindow::drawCDURemotePort()
{
    QString s0;

    s0.setNum(pDevice->getCDURemotePort());
    ui->lineEdit_19->setText(s0);
}

void MainWindow::drawPCLocalIPAddress()
{
    QString s0;
    QString s1;
    QString s2;
    QString s3;
    QString s4;

    s0 = pDevice->getPCLocalIPAddress();
    pDevice->scatterIPAddress(s4, s3, s2, s1, s0);
    ui->lineEdit_14->setText(s4);
    ui->lineEdit_15->setText(s3);
    ui->lineEdit_16->setText(s2);
    ui->lineEdit_17->setText(s1);
}
//
void MainWindow::drawPCLocalPort()
{
    QString s0;
    s0.setNum(pConfig->getPCLocalPort());
    ui->lineEdit_24->setText(s0);
}
//
void MainWindow::drawPCRemoteIPAddress()
{
    QString s0;
    QString s1;
    QString s2;
    QString s3;
    QString s4;

    s0 = pDevice->getPCRemoteIPAddress();
    pDevice->scatterIPAddress(s4, s3, s2, s1, s0);
    ui->lineEdit_6->setText(s4);
    ui->lineEdit_7->setText(s3);
    ui->lineEdit_8->setText(s2);
    ui->lineEdit_9->setText(s1);
}
//
void MainWindow::drawPCRemotePort()
{
    QString s0;
    s0.setNum(pDevice->getPCRemotePort());
    ui->lineEdit_25->setText(s0);
}
//


bool MainWindow::testCDULocalIPAddress()
{
    return pDevice->setCDULocalIPAddress(ui->lineEdit_2->text(), ui->lineEdit_3->text(), ui->lineEdit_4->text(), ui->lineEdit_5->text());
}

bool MainWindow::testCDURemoteIPAddress()
{
    return pDevice->setCDURemoteIPAddress(ui->lineEdit_10->text(), ui->lineEdit_11->text(), ui->lineEdit_12->text(), ui->lineEdit_13->text());
}

bool MainWindow::testPCLocalIPAddress()
{
    return pDevice->setPCLocalIPAddress(ui->lineEdit_14->text(), ui->lineEdit_15->text(), ui->lineEdit_16->text(), ui->lineEdit_17->text());
}

bool MainWindow::testPCRemoteIPAddress()
{
    return pDevice->setPCRemoteIPAddress(ui->lineEdit_6->text(), ui->lineEdit_7->text(), ui->lineEdit_8->text(), ui->lineEdit_9->text());
}

bool MainWindow::testCDULocalPort()
{
    return pDevice->setCDULocalPort(ui->lineEdit_18->text());
}

bool MainWindow::testCDURemotePort()
{
    return pDevice->setCDURemotePort(ui->lineEdit_19->text());
}

bool MainWindow::testPCLocalPort()
{
    return pDevice->setPCLocalPort(ui->lineEdit_24->text());
}

bool MainWindow::testPCRemotePort()
{
    return pDevice->setPCRemotePort(ui->lineEdit_25->text());
}

void MainWindow::on_lineEdit_2_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDULocalIPAddress()) drawCDULocalIPAddress();
}

void MainWindow::on_lineEdit_3_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDULocalIPAddress()) drawCDULocalIPAddress();
}

void MainWindow::on_lineEdit_4_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDULocalIPAddress()) drawCDULocalIPAddress();
}

void MainWindow::on_lineEdit_5_cursorPositionChanged(int arg1, int arg2)
{
    (void) arg1;
    if (!testCDULocalIPAddress()) drawCDULocalIPAddress();
}

void MainWindow::on_lineEdit_18_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDULocalPort()) drawCDULocalPort();
}

void MainWindow::on_lineEdit_19_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDURemotePort()) drawCDURemotePort();
}

void MainWindow::on_lineEdit_24_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCLocalPort()) drawPCLocalPort();
}

void MainWindow::on_lineEdit_25_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCRemotePort()) drawPCRemotePort();
}

// ���������� ���������� ���������� � �����
void MainWindow::on_pushButton_2_released()
{
    pDevice->save();
}

void MainWindow::on_lineEdit_27_textChanged(const QString& arg1)
{
    pDevice->setPathToObjectsFiles(arg1);
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        pDevice->setRestorePCConnectionFlag(true);
    }
    else
        pDevice->setRestorePCConnectionFlag(false);
}


void MainWindow::on_lineEdit_10_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDURemoteIPAddress()) drawCDURemoteIPAddress();
}

void MainWindow::on_lineEdit_11_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDURemoteIPAddress()) drawCDURemoteIPAddress();
}


void MainWindow::on_lineEdit_12_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDURemoteIPAddress()) drawCDURemoteIPAddress();
}

void MainWindow::on_lineEdit_13_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testCDURemoteIPAddress()) drawCDURemoteIPAddress();
}

void MainWindow::on_lineEdit_14_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCLocalIPAddress()) drawPCLocalIPAddress();
}

void MainWindow::on_lineEdit_15_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCLocalIPAddress()) drawPCLocalIPAddress();
}

void MainWindow::on_lineEdit_16_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCLocalIPAddress()) drawPCLocalIPAddress();
}

void MainWindow::on_lineEdit_17_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCLocalIPAddress()) drawPCLocalIPAddress();
}

void MainWindow::on_lineEdit_6_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCRemoteIPAddress()) drawPCRemoteIPAddress();
}

void MainWindow::on_lineEdit_7_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCRemoteIPAddress()) drawPCRemoteIPAddress();
}

void MainWindow::on_lineEdit_8_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCRemoteIPAddress()) drawPCRemoteIPAddress();
}

void MainWindow::on_lineEdit_9_textChanged(const QString& arg1)
{
    (void) arg1;
    if (!testPCRemoteIPAddress()) drawPCRemoteIPAddress();
}
void MainWindow::on_hideSettingsButton_released()
{
    ui->settingsWidget->hide();
    ui->startButtonwidget->show();
}

void MainWindow::on_showSettingsButton_released()
{
    ui->startButtonwidget->hide();
    ui->settingsWidget->show();
}

void MainWindow::on_startCduButton_released()
{
#ifdef ANDROID
    QAndroidJniObject::callStaticMethod<void>("com/radioavionica/UmuEmulator/MyService", "startApplication", "(Landroid/content/Context;)V", QtAndroid::androidContext().object());
#endif
}
