#ifndef FPGACOMM_H
#define FPGACOMM_H

#include <QMainWindow>

#include <QSerialPort>

class FPGA_Comm: public QObject
{
	Q_OBJECT

public:
	FPGA_Comm();
	~FPGA_Comm();
	bool openPort(const QString port);
	bool isConnected(void);
	void setReg(unsigned char reg, unsigned char val);
	void queryReg(unsigned char reg);
	void closePort(void);

	struct Package {
		unsigned char data; // Daten
		unsigned char reg; // Register
		unsigned char chk_rec; // Empfangene Checksumme
		unsigned char chk_calc; // Berechnete Checksumme
		bool rw; // Schreib oder Lesezugriff
		bool valid; // Daten gültig?
	};

	enum REGISTER {
		LED0, LED1, LED2, LED3,
		BUTTONS,
		TEMP_INT, TEMP_DEZI // Ganze Gradzahlen und Nachkommastellen
	};

private:
	QSerialPort *serial; // Serielle Schnittstelle
	enum PARS_STATE_E {IDLE, REG, DATA, CHK};
	unsigned char pars_state; // Statemachine für den Parser der seriellen Empfangsschnittstelle
	Package rec; // Empfangenes Paket

private slots:
	void readPort(void); // Wird von der seriellen Schnittstelle aufgerufen, wenn neue Daten zur Verfügung stehen

signals:
	void receivedPackage(FPGA_Comm::Package *p); // Wird gesendet, wenn ein neues, korrektes Paket empfangen wurde
};

#endif // FPGACOMM_H
