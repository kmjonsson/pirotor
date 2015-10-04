/*
 * Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
 * Released under GPL. See LICENSE
 */

#ifndef ROTORBUTTONS_H
#define ROTORBUTTONS_H

#include <QWidget>
#include <QPushButton>

#include "rotor.h"

class RotorButtons : public QWidget
{
    Q_OBJECT

public:
    explicit RotorButtons(Rotor *rotor, QWidget *parent = 0);

protected:

public slots:

private:
	QPushButton *plus10Button;
	QPushButton *sub10Button;
	QPushButton *stopButton;
};

#endif
