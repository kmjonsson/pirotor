/*
 * Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
 * Released under GPL. See LICENSE
 */

#ifndef ROTOR_H
#define ROTOR_H

#include <QWidget>
#include <QPushButton>
#include <QSettings>
#include <QMutex>
#include "mmq.h"
#include "rotorbuttons.h"

class Rotor : public QWidget
{
    Q_OBJECT

public:
    Rotor(QSettings *settings, RotorButtons *buttons, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent ( QMouseEvent * event );
	 MMQ *mmq;
	 QMutex mutex;
	 QSettings *settings;
    int dir,aim;
    QImage rotorImg;
	 RotorButtons *buttons;

public slots:
	void connected();
	void authenticated();
	void packet(QString queue,QJsonValue data);
	void disconnected();
	void fetchStatus();
	void error(qint64 id);

	// Buttons :-)
	void goAntenna(int deg);
	void aimAntenna(int deg);
	void stopAntenna();

private:
	void initRotor();
};

#endif
