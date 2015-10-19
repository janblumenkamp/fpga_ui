#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QMessageBox>
#include <QLabel>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	comm = new FPGA_Comm();

    // Verbinde Spinfelder mit Slider:
	connect(ui->spn_led0, SIGNAL(valueChanged(int)), ui->sld_led0, SLOT(setValue(int)));
	connect(ui->spn_led1, SIGNAL(valueChanged(int)), ui->sld_led1, SLOT(setValue(int)));
	connect(ui->spn_led2, SIGNAL(valueChanged(int)), ui->sld_led2, SLOT(setValue(int)));
	connect(ui->spn_led3, SIGNAL(valueChanged(int)), ui->sld_led3, SLOT(setValue(int)));

    // Verbinde Slider mit Spinfelder:
	connect(ui->sld_led0, SIGNAL(valueChanged(int)), ui->spn_led0, SLOT(setValue(int)));
	connect(ui->sld_led1, SIGNAL(valueChanged(int)), ui->spn_led1, SLOT(setValue(int)));
	connect(ui->sld_led2, SIGNAL(valueChanged(int)), ui->spn_led2, SLOT(setValue(int)));
	connect(ui->sld_led3, SIGNAL(valueChanged(int)), ui->spn_led3, SLOT(setValue(int)));

	// Buttons
	connect(ui->btn_refreshPortList, SIGNAL(clicked()), this, SLOT(refreshPortList())); //Refresh Button (Liste der seriellen Ports)
	connect(ui->btn_connect, SIGNAL(clicked()), this, SLOT(commOpenPort())); //Connect Button (Liste der seriellen Ports)
	connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(commClosePort())); //Disconnect Button

	// Spinfeld change event
	connect(ui->spn_led0, SIGNAL(valueChanged(int)), this, SLOT(led0_commRefresh(int)));
	connect(ui->spn_led1, SIGNAL(valueChanged(int)), this, SLOT(led1_commRefresh(int)));
	connect(ui->spn_led2, SIGNAL(valueChanged(int)), this, SLOT(led2_commRefresh(int)));
	connect(ui->spn_led3, SIGNAL(valueChanged(int)), this, SLOT(led3_commRefresh(int)));

	// Neues Paket von der FPGA Klasse
	connect(comm, SIGNAL(receivedPackage(FPGA_Comm::Package*)), this, SLOT(newPackage(FPGA_Comm::Package*)));
}

/*
 *
 * Destruktor
 */
MainWindow::~MainWindow()
{
	delete comm;
	delete ui;
}

/*
 * Verbindung zum Refresh Button, lädt die Portliste der verfügbaren Comports
 *
 */
void MainWindow::refreshPortList()
{
	ui->cmb_serialPorts->clear();
	Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
	{
	   ui->cmb_serialPorts->addItem(port.portName());
	}
}

/*
 * Connection zum Open Button, verbindet mit dem seriellen Port
 *
 */
void MainWindow::commOpenPort()
{
	if(ui->cmb_serialPorts->currentText() != "")
	{
		if(comm->openPort("/dev/" + ui->cmb_serialPorts->currentText()) == true) // Vebindung konnte erfolgeich hergestellt werden
		{
			ui->lbl_current_state->setText("Verbunden");
			ui->btn_connect->setEnabled(false);
			ui->btn_disconnect->setEnabled(true);

			// Lese nun die Werte ua sdem FPGA Board für die LEDs
			comm->queryReg(FPGA_Comm::LED0);
			comm->queryReg(FPGA_Comm::LED1);
			comm->queryReg(FPGA_Comm::LED2);
			comm->queryReg(FPGA_Comm::LED3);
			comm->queryReg(FPGA_Comm::BUTTONS);
			comm->queryReg(FPGA_Comm::TEMP_LSB);
			comm->queryReg(FPGA_Comm::TEMP_MSB);
		}
		else
		{
			ui->lbl_current_state->setText("Fehlgeschlagen");
		}
	}
}

/*
 * Connection zum Close Button, schließt den seriellen Port
 *
 */
void MainWindow::commClosePort()
{
	comm->closePort();
	ui->btn_connect->setEnabled(true);
	ui->btn_disconnect->setEnabled(false);
	ui->lbl_current_state->setText("Nicht verbunden");
}

/*
 * LED 1 Änderung - aktualisiere Register
 *
 */
void MainWindow::led0_commRefresh(int val)
{
	comm->setReg(0, (unsigned char)val);
}

/*
 * LED 2 Änderung - aktualisiere Register
 *
 */
void MainWindow::led1_commRefresh(int val)
{
	comm->setReg(1, (unsigned char)val);
}

/*
 * LED 3 Änderung - aktualisiere Register
 *
 */
void MainWindow::led2_commRefresh(int val)
{
	comm->setReg(2, (unsigned char)val);
}

/*
 * LED 4 Änderung - aktualisiere Register
 *
 */
void MainWindow::led3_commRefresh(int val)
{
	comm->setReg(3, (unsigned char)val);
}

/*
 * Neues Paket von der FPGA_Comm Klasse empfangen
 *
 */
void MainWindow::newPackage(FPGA_Comm::Package *p)
{
	qDebug() << "MainWindow RecPack" << p->reg << ", " << p->rw << ", " << p->data;

	if(p->rw == true)
	{
		switch (p->reg) {
		case FPGA_Comm::LED0:
			ui->spn_led0->setValue(p->data);
			break;
		case FPGA_Comm::LED1:
			ui->spn_led1->setValue(p->data);
			break;
		case FPGA_Comm::LED2:
			ui->spn_led2->setValue(p->data);
			break;
		case FPGA_Comm::LED3:
			ui->spn_led3->setValue(p->data);
			break;
		case FPGA_Comm::BUTTONS:
			ui->chk_btnA->setChecked(p->data & (1<<0));
			ui->chk_btnB->setChecked(p->data & (1<<1));
			ui->chk_btnC->setChecked(p->data & (1<<2));
			break;

		// Temperatur: To do...
		default:
			break;
		}

	}
}
