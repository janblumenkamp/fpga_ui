#include <QLabel>
#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include "fpgacomm.h"

FPGA_Comm::FPGA_Comm()
{
	// Serieller Port
	serial = new QSerialPort();

	// Wenn neue Daten zur Verfügung stehen
	connect(serial, SIGNAL(readyRead()), this, SLOT(readPort()));

	pars_state = IDLE;
}

FPGA_Comm::~FPGA_Comm() // Destruktor
{
	closePort();
}

bool FPGA_Comm::openPort(const QString port)
{
	serial->setPortName(port);
	serial->setBaudRate(QSerialPort::Baud19200);
	serial->setDataBits(QSerialPort::Data8);
	serial->setParity(QSerialPort::NoParity);
	serial->setStopBits(QSerialPort::OneStop);
	serial->setFlowControl(QSerialPort::NoFlowControl);
	if (serial->open(QIODevice::ReadWrite)) {
		qDebug() << "Opened sucessfully";
		return true;
	} else {
		qDebug() << "Opening port failed!";
		return false;
	}
}

bool FPGA_Comm::isConnected(void)
{
	return serial->isOpen();
}

void FPGA_Comm::closePort(void)
{
	if(serial->isOpen())
	{
		serial->close();
		qDebug() << "Serial port closed!";
	}
}

/*
 * setzt Wert in Register
 * reg: 0 - 127 (letztes bit für r/w reserviert), val: 0 - 255
 */
void FPGA_Comm::setReg(unsigned char reg, unsigned char val)
{
	if(serial->isOpen())
	{
		unsigned char data[4]; // 4 byte: start, register + rw, data, checksum
		data[0] = 0x55; // b01010101
		data[1] = reg | (1<<7); //Setze register und letztes bit (Schreibzugriff)
		data[2] = val;
		data[3] = data[0] + data[1] + data[2];

		serial->write((const char *)data, 4);
		qDebug()<<"Sent package (set register" << reg << " with data " << val << ")";
	}
}

/*
 * Frage ein Byte vom Slave an (Slave antwortet dann ggf. mit Schreibzugriff)
 * reg: 0 - 127 (letztes bit für r/w reserviert)
 */
void FPGA_Comm::queryReg(unsigned char reg)
{
	if(serial->isOpen())
	{
		unsigned char data[4]; // 4 byte: start, register + rw, data, checksum
		data[0] = 0x55; // b01010101
		data[1] = reg & 0x7f; //Letztes bit darf nicht gesetzt sein (Lesezugriff)
		data[2] = 0; // Keine Daten
		data[3] = data[0] + data[1];

		serial->write((const char *)data, 4);
		qDebug()<<"Sent package (query register" << reg << " (chk:" << data[3] <<"))";
	}
}

/*
 * Slot - wird aufgerufen, wenn neue Daten im seriellen Port zur Verfügung stehen
 *
 */

void FPGA_Comm::readPort(void)
{
	QByteArray ser_raw = serial->readAll(); // Speichere Daten in QBytearray

	std::vector<unsigned char> data( // Übertrage Daten in unsigned char vektor
		ser_raw.begin(), ser_raw.end());

	for(std::vector<unsigned char>::iterator i = data.begin(); i != data.end(); ++i)
	{
		switch (pars_state)
		{
		case IDLE:
			if(*i == 0x55)
			{
				rec.chk_calc = 0x55;
				pars_state = REG;
			}
			break;
		case REG:
			rec.rw = (bool)(*i & (1<<7));
			rec.reg = *i & 0x7F;
			rec.chk_calc += *i;
			pars_state = DATA;
			break;
		case DATA:
			rec.data = *i;
			rec.chk_calc += *i;
			pars_state = CHK;
			break;
		case CHK:

			rec.chk_rec = *i;
			qDebug() << "Received package: reg " << rec.reg << ", rw: " << rec.rw << ", data: " << rec.data << ", checksum: " << rec.chk_rec;
			if(rec.chk_calc == rec.chk_rec)
			{
				rec.valid = true;
				// Nun die empfangenen Daten verarbeiten (muss die Hauptklasse erledigen über Signal)
				emit receivedPackage(&rec);
			}
			else
			{
				rec.valid = false;
				qDebug() << "wrong checksum calc:" << rec.chk_calc;
			}
			pars_state = IDLE;
			break;
		default:
			break;
		}
	}
}
