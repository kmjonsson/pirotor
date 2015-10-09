/*
 * Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
 * Released under GPL. See LICENSE
 */


#include <QtWidgets>

#include "rotorbuttons.h"

RotorButtons::RotorButtons(QSettings *settings, QWidget *parent)
    : QWidget(parent)
{
	foreach(QString g, settings->childGroups()) {
		if(g.startsWith("Button#")) {
			settings->beginGroup(g);

			QPushButton *button = new QPushButton(g.midRef(7).toString(), this);

			int x = settings->value("x",0).toInt();
			int y = settings->value("y",0).toInt();
			int w = settings->value("width",50).toInt();
			int h = settings->value("height",50).toInt();
			button->setGeometry(QRect(QPoint(x, y), QSize(w, h)));

			Action *a = new Action();
			a->action = Action::STOP; 
			a->deg = settings->value("deg",0).toInt();

			QString action = settings->value("action",QString("stop")).toString();
			if(action == "go")   { a->action = Action::GO; }
			if(action == "goto") { a->action = Action::GOTO; }
			if(action == "quit") { a->action = Action::QUIT; }

			buttons.insert(button,a);
			connect(button, SIGNAL (released()), this, SLOT (buttonPressed()));
			settings->endGroup();
		}
	}
}

void RotorButtons::buttonPressed() {
	Action *ba = buttons.value(sender());
	switch(ba->action) {
		case Action::GO:
			emit goButtonPressed(ba->deg);
			break;
		case Action::GOTO:
			emit gotoButtonPressed(ba->deg);
			break;
	    case Action::STOP:
			emit stopButtonPressed();
			break;
	    case Action::QUIT:
			QCoreApplication::quit();
			break;
	}
}

// vim: set ts=4:sw=4
