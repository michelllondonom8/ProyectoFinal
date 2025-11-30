#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QGraphicsPixmapItem>

class Proyectil : public QObject, public QGraphicsPixmapItem
{
public:
    Proyectil(double x0, double y0, double angulo, double velocidadInicial, double gravedad);

    void actualizar(double dt);

    double getX() const { return x; }
    double getY() const { return y; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }

    bool estaActivo() const { return activo; }
    void desactivar() { activo = false; }

    double getVelocidadActual() const;

private:
    double x, y;
    double vx, vy;
    double gravedad;
    bool activo;
};
#endif // PROYECTIL_H
