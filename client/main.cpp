/*
 * Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
 * Released under GPL. See LICENSE
 */

#include <QApplication>
#include <QHBoxLayout>

#include "rotor.h"
#include "rotorbuttons.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Rotor rotor;
    rotor.setFixedSize(QSize(480,480));
    RotorButtons rotorButtons(&rotor);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(&rotor);	
    layout->addWidget(&rotorButtons);	
    QWidget *window = new QWidget;
    window->resize(800,480);
    window->setLayout(layout);
    window->show();
    return app.exec();
}
