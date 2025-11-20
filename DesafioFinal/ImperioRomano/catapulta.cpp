// ==================== catapulta.h ====================
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

// ==================== catapulta.cpp ====================
#include "catapulta.h"
#include <QtMath>

Catapulta::Catapulta(QObject *parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    posX(50),
    posY(450),
    angulo(45.0),
    velocidadAngulo(1.0),
    activo(true),
    subiendo(true)
{
    cargarSprite();
    setPos(posX, posY);
}

void Catapulta::cargarSprite()
{
    // Placeholder - rectángulo marrón simulando catapulta
    spriteCatapulta = QPixmap(80, 100);
    spriteCatapulta.fill(QColor(101, 67, 33)); // Color madera

    // TODO: Cargar sprite real
    // spriteCatapulta.load(":/sprites/catapulta.png");

    setPixmap(spriteCatapulta);
}

void Catapulta::actualizar()
{
    if (!activo) return;

    oscilar();
    actualizarSprite();
}

void Catapulta::oscilar()
{
    // Movimiento oscilante del ángulo de disparo
    if (subiendo) {
        angulo += velocidadAngulo;
        if (angulo >= 80.0) {
            subiendo = false;
        }
    } else {
        angulo -= velocidadAngulo;
        if (angulo <= 10.0) {
            subiendo = true;
        }
    }
}

Proyectil* Catapulta::disparar()
{
    if (!activo) return nullptr;

    // Calcular posición inicial del proyectil
    qreal proyectilX = posX + 40; // Centro de la catapulta
    qreal proyectilY = posY;

    // Calcular velocidad inicial basada en el ángulo actual
    qreal anguloRad = qDegreesToRadians(angulo);
    qreal velocidadInicial = 15.0;

    qreal velX = velocidadInicial * qCos(anguloRad);
    qreal velY = -velocidadInicial * qSin(anguloRad);

    // Crear y retornar el proyectil
    Proyectil *proyectil = new Proyectil(proyectilX, proyectilY, velX, velY);

    emit proyectilDisparado(proyectil);

    return proyectil;
}

void Catapulta::actualizarSprite()
{
    // Aquí podrías rotar el sprite según el ángulo
    // Por ahora mantenemos el sprite fijo
    setPixmap(spriteCatapulta);
}
