/*
 * Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
 * Released under GPL. See LICENSE
 */


#include <QtWidgets>

#include "rotor.h"

#include <QMouseEvent>

Rotor::Rotor(QWidget *parent)
    : QWidget(parent)
{
	settings = new QSettings( QString("rotor.ini"), QSettings::IniFormat );

	// Update timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fetchStatus()));
    timer->start(1000*settings->value("System/statusInterval",300).toInt());

    setWindowTitle(tr("Rotor"));
    resize(800, 480);

    dir = -1;
    aim = -1;

	// MMQ Init
    mmq = new MMQ(this);
    connect(mmq, SIGNAL(connected()),this, SLOT(connected()));
    connect(mmq, SIGNAL(authenticated()),this, SLOT(authenticated()));
    connect(mmq, SIGNAL(packet(QString,QJsonValue)),this, SLOT(packet(QString,QJsonValue)));
    connect(mmq, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(mmq, SIGNAL(error(qint64)),this, SLOT(error(qint64)));

    mmq->setKey(settings->value("System/password").toString());
    mmq->doConnect(
			settings->value("System/hostname",QString("localhost")).toString(),
			settings->value("System/port",4343).toInt());
 
	// Load rotor Image
    rotorImg = QImage(settings->value("System/backgroundImage",QString("rotor.png")).toString());
    Q_ASSERT(!img.isNull());
}

void Rotor::error(qint64 id) {
	qDebug() << "Failure on packet:" << id;
    QCoreApplication::quit();
}

void Rotor::stopButtonPressed() {
	mmq->send(QString("stop"),QJsonValue(QString("stop")));
}

void Rotor::plus10ButtonPressed() {
	mutex.lock();
	int a = aim;
	mutex.unlock();
	if(a < settings->value("System/max",360).toInt()-10) {
		a += 10;
		a /= 10;
		a *= 10;
		a %= 360;
		aimAntenna(a);
	}
}

void Rotor::sub10ButtonPressed() {
	mutex.lock();
	int a = aim;
	mutex.unlock();
	if(a > settings->value("System/min",0).toInt()+10) {
		a -= 10;
		a /= 10;
		a *= 10;
		a %= 360;
		aimAntenna(a);
	}
}

void Rotor::mousePressEvent ( QMouseEvent * event ) {
    int x = height()/2-event->x();
    int y = height()/2-event->y();

	double dist = sqrt(x*x+y*y);

	// Some strange bug in the touch panel of PiDisplay
	if(dist > height()/2*settings->value("System/beamSize",0.97).toDouble()) {
		return;
	}

    int a = -atan2(x,y) * 180 / 3.1415;
    a += 360;
    a %= 360;

	mutex.lock();
    aimAntenna(a);
	mutex.unlock();
}

void Rotor::aimAntenna(int aim) {
	mmq->send(QString("aim"),QJsonValue(aim));
}

void Rotor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(QRect(0, 0, height(), height()), rotorImg);
    painter.translate(height() / 2, height() / 2);
    painter.scale(height() / 200.0, height() / 200.0);

	mutex.lock();
	int a=aim;
	int d=dir;
	mutex.unlock();

    if(a < 0 || d < 0) {
        return;
    }

	static const double beamSize = settings->value("System/beamSize",0.97).toDouble();
	static const int beamWidth=settings->value("System/beamWidth",68).toInt();

    if(a != d && settings->value("System/showAim",true).toBool()) {
		static const QColor penColor(settings->value("System/aimBorder",QString("#80000000")).toString());
		painter.setPen(penColor);
		static const QColor backgroundColor(settings->value("System/aimBackground",QString("#60008080")).toString());
		painter.setBrush(backgroundColor);

		static const QPoint arrow[4] = {
			QPoint(10, 8),
			QPoint(0,0),
			QPoint(-10, 8),
			QPoint(0, -100*beamSize)
		};

        painter.save();
        painter.rotate(a % 360);
        painter.drawConvexPolygon(arrow, 4);
        painter.restore();
    }

    static const QColor penColor(settings->value("System/beamBorder",QString("#80000000")).toString());
    static const QColor backgroundColor(settings->value("System/beamBackground",QString("#60008080")).toString());
	static const bool showCenterLine=settings->value("System/showBeamCenterLine",true).toBool();

    painter.setPen(penColor);
    painter.setBrush(backgroundColor);
    painter.save();
    painter.rotate(d % 360);
	if(showCenterLine) {
    	painter.drawLine(0,0,0,-100*beamSize);
	}
	painter.drawPie(-100*beamSize,-100*beamSize,200*beamSize,200*beamSize,(90-beamWidth/2)*16,beamWidth*16);
    painter.restore();

	// Show text?
    painter.drawText(QPoint(-98,-87),QString("Aim"));
    painter.drawText(QPoint(-80,-87),QString::number(a));
    painter.drawText(QPoint(-98,-74),QString("Dir"));
    painter.drawText(QPoint(-80,-74),QString::number(d));
}

// MMQstuff
void Rotor::connected()
{
        qDebug() << "Test::connected... :-)";
}

void Rotor::authenticated()
{
	qDebug() << "Test::authenticated...";
	mmq->subscribe(QString("status"));
	fetchStatus();
}

// Packet from MMQ
void Rotor::packet(QString queue, QJsonValue data)
{
	if(queue == "status") {
		QJsonObject msg = data.toObject();
		mutex.lock();
		aim = msg["Aim"].toString().toInt() % 360;
		dir = msg["Pos"].toString().toInt() % 360;
		mutex.unlock();
		update();
	}
}

void Rotor::disconnected()
{
        qDebug() << "Test::disconnected...";
        QCoreApplication::quit();
}

void Rotor::fetchStatus() {
		mmq->send(QString("getStatus"),QJsonObject());
}

// vim: set ts=4:sw=4
