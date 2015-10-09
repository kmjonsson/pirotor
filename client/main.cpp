/*
 * Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
 * Released under GPL. See LICENSE
 */

#include <QApplication>
#include <QHBoxLayout>
#include <QSettings>

#include "rotor.h"
#include "rotorbuttons.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSettings *settings = new QSettings( QString("rotor.ini"), QSettings::IniFormat );
    RotorButtons rotorButtons(settings);
    rotorButtons.setFixedSize(
			QSize(
				settings->value("System/screenWidth",800).toInt()
				-settings->value("System/screenHeight",480).toInt(),
				settings->value("System/screenHeight",480).toInt()
				)
	       		);

    Rotor rotor(settings,&rotorButtons);
;
    rotor.setFixedSize(
			QSize(
				settings->value("System/screenHeight",480).toInt(),
				settings->value("System/screenHeight",480).toInt()
				)
	       		);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(&rotor);	
    layout->addWidget(&rotorButtons);	

    QWidget window;
    window.setFixedSize(
                        QSize(
                                settings->value("System/screenWidth",800).toInt(),
                                settings->value("System/screenHeight",480).toInt()
                                )
                        );
    window.setLayout(layout);
    window.show();
    return app.exec();
}
