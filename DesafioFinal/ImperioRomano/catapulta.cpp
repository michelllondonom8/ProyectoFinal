#include "Catapulta.h"
#include <QtMath>
#include <QDebug>

Catapulta::Catapulta(double x, double y)
    : posX(x),
    posY(y),
    anguloActual(45.0),
    anguloMinimo(30.0),
    anguloMaximo(80.0)
{
    QPixmap sprite(":/images/catapulta_1.png");

    if (sprite.isNull()) {
        qDebug() << "Error: No se pudo cargar catapulta.png";
    }

    sprite = sprite.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(sprite);

    setPos(x - pixmap().width() / 2, 570);

    setZValue(10);

    qDebug() << "Catapulta creada en:" << pos() << "Suelo Y:" << y;
}

void Catapulta::setAngulo(double angulo)
{
    anguloActual = qBound(anguloMinimo, angulo, anguloMaximo);
}

void Catapulta::getPuntoLanzamiento(double &x, double &y) const
{
    double anguloRad = anguloActual * M_PI / 180.0;

    double baseX = this->pos().x() + pixmap().width() / 2;
    double baseY = this->pos().y() + 70;

    double distanciaLanzamiento = 60;

    x = baseX + distanciaLanzamiento * qCos(anguloRad);
    y = baseY - distanciaLanzamiento * qSin(anguloRad);

    qDebug() << "Punto lanzamiento real:" << x << y;
}

