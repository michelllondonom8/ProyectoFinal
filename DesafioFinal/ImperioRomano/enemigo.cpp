#include "enemigo.h"
#include <QtMath>
#include <QTransform>
#include <QDebug>

Enemigo::Enemigo(bool fuerte, QObject *parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    esFuerte(fuerte),
    velocidad(2.0),
    rangoDeteccion(350.0),
    rangoAtaque(80.0),
    atacando(false),
    contadorAtaque(0),
    frameActual(0),
    contadorFrame(0),
    framesPorAnimacion(10),
    mirandoIzquierda(true),
    estadoActual(IDLE)
{
    vidaMaxima = esFuerte ? 200 : 100;
    vida = vidaMaxima;
    velocidad = esFuerte ? 1.4 : 2.2;

    setPixmap(crearPlaceholder());
    setOffset(-pixmap().width() / 2, -pixmap().height());
}

QPixmap Enemigo::crearPlaceholder() const
{
    int w = esFuerte ? 100 : 80;
    int h = esFuerte ? 140 : 120;

    QPixmap p(w, h);
    QColor color = esFuerte ? QColor(200, 50, 50)  // rojo = fuerte
                            : QColor(50, 80, 200); // azul = normal

    p.fill(color);
    return p;
}

void Enemigo::actualizar(const QPointF &posJugador)
{
    if (!estaVivo()) return;

    qreal dx = posJugador.x() - x();
    qreal distancia = qAbs(dx);

    mirandoIzquierda = (dx < 0);

    if (distancia <= rangoAtaque)
    {
        if (!atacando) {
            atacando = true;
            contadorAtaque = 0;
            estadoActual = ATACANDO;
            frameActual = 0;
        }
    }
    else if (distancia <= rangoDeteccion)
    {
        if (!atacando) {
            estadoActual = CAMINANDO;
            qreal paso = (dx > 0) ? velocidad : -velocidad;
            setPos(x() + paso, y());
        }
    }
    else
    {
        if (!atacando)
            estadoActual = IDLE;
    }
    if (atacando)
    {
        contadorAtaque++;

        if (contadorAtaque > 30) {
            atacando = false;
            contadorAtaque = 0;
            estadoActual = IDLE;
            frameActual = 0;
        }
    }

    actualizarAnimacion();
    actualizarSprite();
}

void Enemigo::actualizarAnimacion()
{
    contadorFrame++;

    if (contadorFrame >= framesPorAnimacion) {
        contadorFrame = 0;
        frameActual++;

        if (frameActual >= 4)
            frameActual = 0;
    }
}

void Enemigo::actualizarSprite()
{
    QPixmap p = crearPlaceholder();
    if (mirandoIzquierda)
        p = p.transformed(QTransform().scale(-1, 1));

    setPixmap(p);
    setOffset(-pixmap().width() / 2, -pixmap().height());
}

void Enemigo::recibirDanio(int danio)
{
    vida -= danio;
    if (vida < 0) vida = 0;

    if (vida == 0)
        emit murio();
}

QRectF Enemigo::getBoundingBox() const
{
    qreal w = pixmap().width();
    qreal h = pixmap().height();
    return QRectF(x() - w/2, y() - h, w, h);
}
