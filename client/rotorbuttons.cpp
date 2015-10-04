/*
 * Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
 * Released under GPL. See LICENSE
 */


#include <QtWidgets>

#include "rotorbuttons.h"

RotorButtons::RotorButtons(Rotor *rotor, QWidget *parent)
    : QWidget(parent)
{
    resize(320, 480);

    // Buttons
	plus10Button = new QPushButton("+10", this);
	plus10Button->setGeometry(QRect(QPoint(645-480, 10), QSize(145, 60)));
	connect(plus10Button, SIGNAL (released()), rotor, SLOT (plus10ButtonPressed()));

	sub10Button = new QPushButton("-10", this);
	sub10Button->setGeometry(QRect(QPoint(490-480, 10), QSize(145, 60)));
	connect(sub10Button, SIGNAL (released()), rotor, SLOT (sub10ButtonPressed()));

	stopButton = new QPushButton("Stop", this);
	stopButton->setGeometry(QRect(QPoint(490-480, 370), QSize(300, 100)));
	connect(stopButton, SIGNAL (released()), rotor, SLOT (stopButtonPressed()));
}

// vim: set ts=4:sw=4
