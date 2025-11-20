#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QGraphicsEllipseItem>
#include <QObject>
#include <QBrush>      // ← FALTABA
#include <QPen>

class Proyectil : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    explicit Proyectil(qreal x, qreal y, qreal velX, qreal velY, QObject *parent = nullptr);

    // Métodos principales
    void actualizar();
    bool estaDentroDelLimite() const;

    // Getters
    qreal getVelocidadX() const { return velocidadX; }
    qreal getVelocidadY() const { return velocidadY; }
    qreal getAngulo() const { return angulo; }

    // Setters
    void setVelocidadX(qreal vel) { velocidadX = vel; }
    void setVelocidadY(qreal vel) { velocidadY = vel; }

signals:
    void fueraDeLimites();

private:
    qreal velocidadX;
    qreal velocidadY;
    qreal angulo;
    const qreal gravedad = 0.5;

    void aplicarFisica();
};

#endif // PROYECTIL_H
