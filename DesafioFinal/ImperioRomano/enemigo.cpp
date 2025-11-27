#include "enemigo.h"
#include <QtMath>
#include <QTransform>
#include <QDebug>
#include <QBrush>
#include <QPen>

Enemigo::Enemigo(bool fuerte, QObject *parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    esFuerte(fuerte),
    velocidad(2.5),
    rangoDeteccion(450.0),
    rangoAtaque(130.0),
    atacando(false),
    contadorAtaque(0),
    cooldownAtaque(0),
    cooldownAtaqueMax(80),
    yaGolpeo(false),
    frameActual(0),
    contadorFrame(0),
    framesPorAnimacion(8),
    mirandoIzquierda(false),
    estadoActual(IDLE),
    barraVidaFondo(nullptr),
    barraVida(nullptr)
{
    vidaMaxima = esFuerte ? 300 : 150;
    vida = vidaMaxima;
    velocidad = esFuerte ? 1.8 : 2.5;
    cooldownAtaqueMax = esFuerte ? 100 : 80;

    cargarSprites();

    QPixmap inicial = obtenerSpriteActual();
    if (inicial.isNull()) {
        inicial = crearPlaceholder();
    }

    setPixmap(inicial);
    setOffset(-pixmap().width() / 2, -pixmap().height());

    qreal anchoBarraVida = esFuerte ? 90 : 70;
    qreal altoBarraVida = 8;

    barraVidaFondo = new QGraphicsRectItem(-anchoBarraVida/2, -pixmap().height() - 15,
                                           anchoBarraVida, altoBarraVida, this);
    barraVidaFondo->setBrush(QBrush(QColor(100, 0, 0)));
    barraVidaFondo->setPen(QPen(Qt::black, 2));

    QColor colorVida = esFuerte ? QColor(200, 50, 50) : QColor(50, 200, 50);
    barraVida = new QGraphicsRectItem(-anchoBarraVida/2, -pixmap().height() - 15,
                                      anchoBarraVida, altoBarraVida, this);
    barraVida->setBrush(QBrush(colorVida));
    barraVida->setPen(QPen(Qt::white, 2));
}

void Enemigo::cargarSprites()
{
    if (esFuerte) {
        animIdle.append(QPixmap(":/images/enemigo_idle1.png"));

        animWalk.append(QPixmap(":/images/enemigo_walk1.png"));
        animWalk.append(QPixmap(":/images/enemigo_walk2.png"));

        animAttack.append(QPixmap(":/images/enemigo_ataque.png"));

        animDeath.append(QPixmap(":/images/enemigo_muerte1.png"));
        animDeath.append(QPixmap(":/images/enemigo_muerte2.png"));
        animDeath.append(QPixmap(":/images/enemigo_muerte3.png"));
        animDeath.append(QPixmap(":/images/enemigo_muerte4.png"));
    } else {
        animIdle.append(QPixmap(":/images/enemigo_normal_idle_01.png"));

        animWalk.append(QPixmap(":/images/enemigo_normal_walk_01.png"));
        animWalk.append(QPixmap(":/images/enemigo_normal_walk_02.png"));

        animAttack.append(QPixmap(":/images/enemigo_normal_attack_01.png"));
        animAttack.append(QPixmap(":/images/enemigo_normal_attack_02.png"));

        animDeath.append(QPixmap(":/images/enemy_death_01.png"));
        animDeath.append(QPixmap(":/images/enemy_death_02.png"));
    }
}

QPixmap Enemigo::crearPlaceholder() const
{
    int w = esFuerte ? 120 : 100;
    int h = esFuerte ? 160 : 140;
    QPixmap p(w, h);
    QColor color = esFuerte ? QColor(200, 50, 50) : QColor(50, 80, 200);
    p.fill(color);
    return p;
}

void Enemigo::actualizar(const QPointF &posJugador)
{
    if (!estaVivo()) {
        if (estadoActual != MUERTO) {
            estadoActual = MUERTO;
            frameActual = 0;
            contadorFrame = 0;
        }
        actualizarAnimacion();
        actualizarSprite();
        return;
    }

    if (cooldownAtaque > 0) {
        cooldownAtaque--;
    }

    qreal dx = posJugador.x() - x();
    qreal distancia = qAbs(dx);

    mirandoIzquierda = (dx < 0);

    if (distancia <= rangoAtaque && cooldownAtaque <= 0)
    {
        if (!atacando) {
            atacando = true;
            contadorAtaque = 0;
            yaGolpeo = false;
            estadoActual = ATACANDO;
            frameActual = 0;
            contadorFrame = 0;
        }
    }
    else if (distancia <= rangoDeteccion && !atacando)
    {
        estadoActual = CAMINANDO;
        qreal paso = (dx > 0) ? velocidad : -velocidad;
        setPos(x() + paso, y());
    }
    else if (!atacando)
    {
        estadoActual = IDLE;
    }

    if (atacando)
    {
        contadorAtaque++;

        if (contadorAtaque == 12 && !yaGolpeo) {
            yaGolpeo = true;
            emit ataque();
        }

        if (contadorAtaque >= 30) {
            atacando = false;
            contadorAtaque = 0;
            cooldownAtaque = cooldownAtaqueMax;
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

        int maxFrames = 1;
        switch (estadoActual) {
        case IDLE: maxFrames = animIdle.size(); break;
        case CAMINANDO: maxFrames = animWalk.size(); break;
        case ATACANDO: maxFrames = animAttack.size(); break;
        case MUERTO: maxFrames = animDeath.size(); break;
        }

        if (maxFrames <= 0) maxFrames = 1;

        if (frameActual >= maxFrames) {
            if (estadoActual == MUERTO) {
                frameActual = maxFrames - 1;
            } else {
                frameActual = 0;
            }
        }
    }
}

QPixmap Enemigo::obtenerSpriteActual()
{
    QVector<QPixmap>* vectorSprites = nullptr;

    switch (estadoActual) {
    case IDLE: vectorSprites = &animIdle; break;
    case CAMINANDO: vectorSprites = &animWalk; break;
    case ATACANDO: vectorSprites = &animAttack; break;
    case MUERTO: vectorSprites = &animDeath; break;
    }

    if (!vectorSprites || vectorSprites->isEmpty()) {
        return crearPlaceholder();
    }

    if (frameActual < 0 || frameActual >= vectorSprites->size()) {
        frameActual = 0;
    }

    QPixmap sprite = vectorSprites->at(frameActual);

    if (sprite.isNull()) {
        return crearPlaceholder();
    }

    int w = esFuerte ? 140 : 110;
    int h = esFuerte ? 180 : 150;
    sprite = sprite.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    return sprite;
}

void Enemigo::actualizarSprite()
{
    QPixmap sprite = obtenerSpriteActual();

    if (mirandoIzquierda) {
        sprite = sprite.transformed(QTransform().scale(-1, 1));
    }

    setPixmap(sprite);
    setOffset(-pixmap().width() / 2, -pixmap().height());
}

void Enemigo::recibirDanio(int danio)
{
    vida -= danio;
    if (vida < 0) vida = 0;

    actualizarBarraVida();

    if (vida == 0) {
        estadoActual = MUERTO;

        if (barraVida) barraVida->setVisible(false);
        if (barraVidaFondo) barraVidaFondo->setVisible(false);

        emit murio();
    }
}

void Enemigo::actualizarBarraVida()
{
    if (!barraVida || !barraVidaFondo) return;

    qreal anchoTotal = barraVidaFondo->rect().width();
    qreal porcentajeVida = static_cast<qreal>(vida) / vidaMaxima;
    qreal anchoActual = anchoTotal * porcentajeVida;

    QRectF rect = barraVida->rect();
    rect.setWidth(anchoActual);
    barraVida->setRect(rect);

    QColor colorVida;
    if (porcentajeVida > 0.6) {
        colorVida = QColor(50, 200, 50);
    } else if (porcentajeVida > 0.3) {
        colorVida = QColor(255, 165, 0);
    } else {
        colorVida = QColor(200, 50, 50);
    }

    barraVida->setBrush(QBrush(colorVida));
}

QRectF Enemigo::getBoundingBox() const
{
    qreal w = pixmap().width() * 0.6;
    qreal h = pixmap().height() * 0.75;
    qreal offsetX = (pixmap().width() - w) / 2;
    qreal offsetY = (pixmap().height() - h);

    return QRectF(x() - pixmap().width()/2 + offsetX,
                  y() - pixmap().height() + offsetY,
                  w, h);
}

QRectF Enemigo::getRangoAtaque() const
{
    qreal anchoAtaque = 80;
    qreal altoAtaque = pixmap().height() * 0.6;

    qreal offsetX;
    if (mirandoIzquierda) {
        offsetX = -pixmap().width()/2 - anchoAtaque;
    } else {
        offsetX = pixmap().width()/2;
    }

    return QRectF(x() + offsetX,
                  y() - pixmap().height(),
                  anchoAtaque,
                  altoAtaque);
}
