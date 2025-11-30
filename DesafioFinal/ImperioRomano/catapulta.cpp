#include "Catapulta.h"
#include <QtMath>

Catapulta::Catapulta(double x, double y)
    : posX(x),
    posY(y),
    anguloActual(45.0),
    anguloMinimo(30.0),
    anguloMaximo(80.0),
    longitudBrazo(80.0)
{
    QPixmap sprite(":/images/catapulta_1.png");

    if (sprite.isNull()) {
        qDebug() << "Error: No se pudo cargar catapulta.png";
        sprite = QPixmap(120, 100);
        sprite.fill(QColor(139, 69, 19));
    }

    sprite = sprite.scaled(120, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(sprite);

    setTransformOriginPoint(pixmap().width() / 2, pixmap().height() * 0.8);

    setPos(x - pixmap().width() / 2, y - pixmap().height());
    setZValue(10);
}

void Catapulta::setAngulo(double angulo)
{
    anguloActual = qBound(anguloMinimo, angulo, anguloMaximo);
    setRotation(-anguloActual);
}

void Catapulta::animarDisparo()
{
    setRotation(-10.0);
}

void Catapulta::restaurarPosicion()
{
    setRotation(-anguloActual);
}

void Catapulta::getPuntoLanzamiento(double &x, double &y) const
{
    double anguloRad = anguloActual * M_PI / 180.0;
    double centroX = posX;
    double centroY = posY - pixmap().height() * 0.2;

    x = centroX + longitudBrazo * qCos(anguloRad);
    y = centroY - longitudBrazo * qSin(anguloRad);
}
