#ifndef CATAPULTA_H
#define CATAPULTA_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPixmap>
#include <QList>
#include "proyectil.h"

class Catapulta : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Catapulta(QObject *parent = nullptr);

    // Métodos principales
    void actualizar();
    Proyectil* disparar();

    // Getters
    qreal getAngulo() const { return angulo; }
    qreal getPosX() const { return posX; }
    qreal getPosY() const { return posY; }
    qreal getVelocidadAngulo() const { return velocidadAngulo; }
    bool estaActivo() const { return activo; }

    // Setters
    void setAngulo(qreal nuevoAngulo) { angulo = nuevoAngulo; }
    void setPosX(qreal x) { posX = x; }
    void setPosY(qreal y) { posY = y; }
    void setActivo(bool estado) { activo = estado; }

signals:
    void proyectilDisparado(Proyectil *proyectil);

private:
    // Atributos
    qreal posX;
    qreal posY;
    qreal angulo;
    qreal velocidadAngulo;
    bool activo;
    bool subiendo;

    // Sprites
    QPixmap spriteCatapulta;

    void cargarSprite();
    void actualizarSprite();
    void oscilar();
};

#endif // CATAPULTA_H
