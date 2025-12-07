#include "Proyectil.h"
#include <QtMath>
#include <QDebug>

Proyectil::Proyectil(double x0, double y0, double angulo,
                     double velocidadInicial, double g)
    : x(x0),
    y(y0),
    gravedad(g),
    activo(true)
{
    double anguloRad = angulo * M_PI / 180.0;

    vx = velocidadInicial * qCos(anguloRad);
    vy = -velocidadInicial * qSin(anguloRad);

    QPixmap sprite(":/images/piedra_de_caida.png");

    if (sprite.isNull()) {
        qDebug() << "Error: No se pudo cargar proyectil.png";
        sprite = QPixmap(35, 35);
        sprite.fill(QColor(100, 100, 100));
    }

    sprite = sprite.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(sprite);

    setOffset(-pixmap().width() / 2, -pixmap().height() / 2);

    setPos(x, y);
    setZValue(15);
}

void Proyectil::actualizar(double dt)
{
    if (!activo) return;

    vy += gravedad * dt;

    x += vx * dt;
    y += vy * dt;

    setPos(x, y);
}

double Proyectil::getVelocidadActual() const
{
    return qSqrt(vx * vx + vy * vy);
}
