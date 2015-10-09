/*
 * Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
 * Released under GPL. See LICENSE
 */

#ifndef ROTORBUTTONS_H
#define ROTORBUTTONS_H

#include <QMap>
#include <QWidget>
#include <QPushButton>
#include <QSettings>


class Action {
public:
	enum Actions { STOP, GO, GOTO, QUIT };
	Actions action;
	int deg;
};

class RotorButtons : public QWidget
{
    Q_OBJECT

public:
    explicit RotorButtons(QSettings *settings, QWidget *parent = 0);

protected:

public slots:
	void buttonPressed();

signals:
	void goButtonPressed(int deg);
	void gotoButtonPressed(int deg);
	void stopButtonPressed();

private:
	QMap<QObject *, Action *> buttons;
};

#endif
