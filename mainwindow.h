#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QSerialPortInfo>

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
	QSerialPort *serial;

private slots:
	void refreshPortList(); // Aktualisiert Portliste
	void serialOpenPort(); // Öffnet comport
	void serialClosePort(); //Schließt Comport
	void serialWrite(int val); // Schreibt integer auf den Port
	void serialRead(void); // Holt neue Daten vom seriellen Port ab, wenn verfügbar
};

#endif // MAINWINDOW_H
