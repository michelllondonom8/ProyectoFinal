#ifndef CATAPULTA_H
#define CATAPULTA_H

#include <QGraphicsPixmapItem>
#include <QPixmap>

class Catapulta : public QGraphicsPixmapItem
{
public:
    Catapulta(double x, double y);

    void setAngulo(double angulo);
    double getAngulo() const { return anguloActual; }

    void animarDisparo();
    void restaurarPosicion();

    double getPosX() const { return posX; }
    double getPosY() const { return posY; }
    void getPuntoLanzamiento(double &x, double &y) const;

private:
    double posX;
    double posY;
    double anguloActual;
    double anguloMinimo;
    double anguloMaximo;
    double longitudBrazo;
};

#endif // CATAPULTA_H
