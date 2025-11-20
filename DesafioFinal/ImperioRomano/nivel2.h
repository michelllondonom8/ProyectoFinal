#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include <QGraphicsPixmapItem>
#include <QList>
#include <QTimer>
#include <QGraphicsRectItem>
#include <QSoundEffect>

class Nivel2 : public Nivel
{
    Q_OBJECT

public:
    explicit Nivel2(QWidget *parent = nullptr);
    ~Nivel2();

protected:
    void inicializarNivel() override;
    void cargarFondo() override;
    void actualizarJuego() override;
    void verificarColisiones() override;

private:
    QList<QGraphicsPixmapItem*> fondos;
    QList<QGraphicsPixmapItem*> obstaculos;

    qreal desplazamientoFondo;
    qreal distanciaRecorrida;
    qreal distanciaObjetivo;
    int contadorGeneracion;
    int repeticionesIntro;
    bool explosionActiva;

    QGraphicsRectItem *puerta;
    QGraphicsPixmapItem *spriteExplosion;
    QTimer *timerExplosion;
    int frameExplosion;

    QSoundEffect fxRock;
    QSoundEffect fxPillar;

    void crearFondos();
    void generarObstaculos();
    void actualizarObstaculos();
    void iniciarExplosion(const QPointF &p);
    void iniciarMuerteJugador();
};

#endif
