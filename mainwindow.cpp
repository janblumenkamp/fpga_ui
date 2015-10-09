#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QMessageBox>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	// Serieller Port
	serial = new QSerialPort(this);
	connect(serial, SIGNAL(readyRead()), this, SLOT(serialRead()));

    // Verbinde Spinfelder mit Slider:
    connect(ui->spn_led1, SIGNAL(valueChanged(int)), ui->sld_led1, SLOT(setValue(int)));
    connect(ui->spn_led2, SIGNAL(valueChanged(int)), ui->sld_led2, SLOT(setValue(int)));
    connect(ui->spn_led3, SIGNAL(valueChanged(int)), ui->sld_led3, SLOT(setValue(int)));
    connect(ui->spn_led4, SIGNAL(valueChanged(int)), ui->sld_led4, SLOT(setValue(int)));

    // Verbinde Slider mit Spinfelder:
    connect(ui->sld_led1, SIGNAL(valueChanged(int)), ui->spn_led1, SLOT(setValue(int)));
    connect(ui->sld_led2, SIGNAL(valueChanged(int)), ui->spn_led2, SLOT(setValue(int)));
    connect(ui->sld_led3, SIGNAL(valueChanged(int)), ui->spn_led3, SLOT(setValue(int)));
    connect(ui->sld_led4, SIGNAL(valueChanged(int)), ui->spn_led4, SLOT(setValue(int)));

	// Buttons
	connect(ui->btn_refreshPortList, SIGNAL(clicked()), this, SLOT(refreshPortList())); //Refresh Button (Liste der seriellen Ports)
	connect(ui->btn_connect, SIGNAL(clicked()), this, SLOT(serialOpenPort())); //Connect Button (Liste der seriellen Ports)
	connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(serialClosePort())); //Disconnect Button

	// Spinfeld change event
	connect(ui->spn_led1, SIGNAL(valueChanged(int)), this, SLOT(serialWrite(int)));
}

/*
 *
 * Destruktor
 */
MainWindow::~MainWindow()
{
	serialClosePort();
	delete ui;
}

/*
 * Verbindung zum Refresh Button, lädt die Portliste der verfügbaren Comports
 *
 */
void MainWindow::refreshPortList()
{
	Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
	{
	   ui->cmb_serialPorts->addItem(port.portName());
	}
}

/*
 * Connection zum Open Button, verbindet mit dem seriellen Port
 *
 */
void MainWindow::serialOpenPort()
{
	if(ui->cmb_serialPorts->currentText() != "")
	{
		serial->setPortName("/dev/" + ui->cmb_serialPorts->currentText());
		serial->setBaudRate(QSerialPort::Baud19200);
		serial->setDataBits(QSerialPort::Data8);
		serial->setParity(QSerialPort::NoParity);
		serial->setStopBits(QSerialPort::OneStop);
		serial->setFlowControl(QSerialPort::NoFlowControl);
		if (serial->open(QIODevice::ReadWrite)) {
			qDebug() << "Opened sucessfully";
		} else {
			qDebug() << "Opening port failed!";
		}
	}
}

/*
 * connection zum close Button, Schließt Verbindung mit dem seriellen Port
 *
 */
void MainWindow::serialClosePort()
{
	if(serial->isOpen())
	{
		serial->close();
		qDebug() << "Serial port closed!";
	}
}

/*
 * sendet ein byte auf den serial port (temporär)
 *
 */
void MainWindow::serialWrite(int val)
{
	if(serial->isOpen())
	{
		serial->write((char *)&val, 1);
		qDebug()<<"Sent" << val;
	}
}

/*
 * Slot, Wird von serialport connection aufgerufen, holt neue Daten vom Somport ab
 *
 */

void MainWindow::serialRead(void)
{
	QByteArray data = serial->readAll();
	qDebug()<<data;
}
