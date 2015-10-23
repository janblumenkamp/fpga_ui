#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fpgacomm.h"
#include "qtcolortriangle.h"
#include "colorviewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	Ui::MainWindow *ui;
	FPGA_Comm *comm;
	double temperature; // Aktuelle temperatur

	#define TEMP_VALUES 300 // Temperaturwerte
	QTimer *timer_tempGraph;
	QVector<double> temperature_history, temperature_time; // Letzte Temperaturwerte (y Achse) und Zeitwerte (x Achse)
private slots:
	void refreshPortList(); // Aktualisiert Portliste
	void commOpenPort(); // Öffnet comport
	void commClosePort(); // schließt comport

	void led0_commRefresh(int val); // Änderung der Werte in den LEDs
	void led1_commRefresh(int val);
	void led2_commRefresh(int val);
	void led3_commRefresh(int val);

	void newPackage(FPGA_Comm::Package *p); // Neues Paket von der FPGA_Comm Klasse empfangen

	void updateTempGraph(void); // Aktualisieren des Temperaturgraphen

	void rgb1ColorChanged(const QColor &col); // Farbe geändert
};

#endif // MAINWINDOW_H
