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

class Rotor : public QWidget
{
    Q_OBJECT

public:
    Rotor(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent ( QMouseEvent * event );
	 void aimAntenna(int aim);
	 MMQ *mmq;
	 QMutex mutex;
	 QSettings *settings;
    int dir,aim;
    QImage rotorImg;

public slots:
	void connected();
	void authenticated();
	void packet(QString queue,QJsonValue data);
	void disconnected();
	void fetchStatus();

	// Buttons :-)
	void plus10ButtonPressed();
	void sub10ButtonPressed();
	void stopButtonPressed();

private:
};

#endif
