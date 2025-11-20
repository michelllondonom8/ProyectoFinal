#include "proyectil.h"
#include <QtMath>

Proyectil::Proyectil(qreal x, qreal y, qreal velX, qreal velY, QObject *parent)
    : QObject(parent),
    QGraphicsEllipseItem(0, 0, 20, 20),
    velocidadX(velX),
    velocidadY(velY),
    angulo(0)
{
    setPos(x, y);
    setBrush(QBrush(Qt::gray));
    setPen(QPen(Qt::darkGray, 2));
}

void Proyectil::actualizar()
{
    aplicarFisica();

    // Mover el proyectil
    setPos(x() + velocidadX, y() + velocidadY);

    // Verificar si está fuera de límites
    if (!estaDentroDelLimite()) {
        emit fueraDeLimites();
    }
}

void Proyectil::aplicarFisica()
{
    // Aplicar gravedad (tiro parabólico)
    velocidadY += gravedad;

    // Calcular ángulo actual de la trayectoria
    if (velocidadX != 0) {
        angulo = qRadiansToDegrees(qAtan2(velocidadY, velocidadX));
    }
}

bool Proyectil::estaDentroDelLimite() const
{
    // Verificar si el proyectil está dentro de los límites de la escena
    qreal posX = x();
    qreal posY = y();

    return (posX >= -50 && posX <= 1250 && posY >= -50 && posY <= 650);
}
