#include "gladiador.h"
#include "fisica.h"
#include <QGraphicsScene>
#include <QTransform>
#include <QPixmap>

Gladiador::Gladiador(QObject *parent)
    : QObject(parent)
    , QGraphicsPixmapItem()
    , vida(100)
    , velocidadX(0)
    , velocidadY(0)
    , velocidadMaxima(Fisica::VELOCIDAD_HORIZONTAL)
    , aceleracion(Fisica::ACELERACION)
    , fuerzaSalto(Fisica::FUERZA_SALTO)
    , gravedad(Fisica::GRAVEDAD)
    , enSuelo(true)
    , saltando(false)
    , atacando(false)
    , mirandoDerecha(true)
    , frameActual(0)
    , contadorFrame(0)
    , framesPorAnimacion(Fisica::FRAMES_POR_ANIMACION)
    , estadoActual(PARADO)
    , ataqueContador(0)
{
    cargarSpritesIndividuales();

    QPixmap startPx;
    if (!animIdle.isEmpty()) startPx = animIdle.at(0);
    if (startPx.isNull()) {
        startPx = QPixmap(110, 140);
        startPx.fill(Qt::transparent);
    }
    startPx = startPx.scaled(110, 140, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    setPixmap(startPx);
    setOffset(-pixmap().width() / 2, -pixmap().height());
}

void Gladiador::cargarSpritesIndividuales()
{
    animIdle.clear();
    animIdle.append(QPixmap(":/images/idle_01.jpeg"));
    animIdle.append(QPixmap(":/images/idle_02.jpeg"));

    animWalk.clear();
    animWalk.append(QPixmap(":/images/player_walk_01.png"));
    animWalk.append(QPixmap(":/images/player_walk_02.png"));
    animWalk.append(QPixmap(":/images/player_walk_03.png"));

    animJump.clear();
    animJump.append(QPixmap(":/images/player_jump.png"));

    animAttack.clear();
    animAttack.append(QPixmap(":/images/player_attack_01.png"));
    animAttack.append(QPixmap(":/images/player_attack_02.png"));

    animDeath.clear();
    animDeath.append(QPixmap(":/images/player_death_01.png"));
    animDeath.append(QPixmap(":/images/player_death_02.png"));
}

void Gladiador::moverIzquierda()
{
    velocidadX = -velocidadMaxima;
    mirandoDerecha = false;
    if (enSuelo && !atacando) estadoActual = CAMINANDO;
}

void Gladiador::moverDerecha()
{
    velocidadX = velocidadMaxima;
    mirandoDerecha = true;
    if (enSuelo && !atacando) estadoActual = CAMINANDO;
}

void Gladiador::saltar()
{
    if (enSuelo && !saltando && !atacando) {
        velocidadY = fuerzaSalto;
        enSuelo = false;
        saltando = true;
        estadoActual = SALTANDO;
        frameActual = 0;
        contadorFrame = 0;
    }
}

void Gladiador::atacar()
{
    if (!atacando && estadoActual != MUERTO) {
        atacando = true;
        ataqueContador = framesPorAnimacion; // duración corta del ataque
        estadoActual = ATACANDO;
        frameActual = 0;
        contadorFrame = 0;
    }
}

void Gladiador::detener()
{
    velocidadX = 0;
    if (enSuelo && !atacando) estadoActual = PARADO;
}

void Gladiador::aplicarGravedad()
{
    if (!enSuelo) {
        velocidadY += gravedad;
        if (velocidadY > Fisica::MAX_CAIDA) velocidadY = Fisica::MAX_CAIDA;
    }
}

void Gladiador::actualizar()
{
    aplicarGravedad();
    setPos(x() + velocidadX, y() + velocidadY);

    if (!scene()) return;

    qreal suelo = scene()->height();

    if (y() >= suelo) {
        setPos(x(), suelo);
        velocidadY = 0;
        enSuelo = true;
        saltando = false;
        if (!atacando) estadoActual = (velocidadX == 0) ? PARADO : CAMINANDO;
    }

    QRectF lim = scene()->sceneRect();
    qreal w = pixmap().width();

    if (x() - w/2 < lim.left()) setPos(w/2, y());
    if (x() + w/2 > lim.right()) setPos(lim.right() - w/2, y());

    if (atacando) {
        ataqueContador--;
        if (ataqueContador <= 0) {
            atacando = false;
            estadoActual = enSuelo ? PARADO : SALTANDO;
            frameActual = 0;
            contadorFrame = 0;
        }
    }

    actualizarAnimacion();
    actualizarSprite();
}

void Gladiador::actualizarAnimacion()
{
    contadorFrame++;
    if (contadorFrame < framesPorAnimacion) return;
    contadorFrame = 0;
    frameActual++;

    int max = 1;
    switch (estadoActual) {
    case PARADO: max = animIdle.size(); break;
    case CAMINANDO: max = animWalk.size(); break;
    case SALTANDO: max = animJump.size(); break;
    case ATACANDO: max = animAttack.size(); break;
    case MUERTO: max = animDeath.size(); break;
    }

    if (max <= 0) { frameActual = 0; return; }
    if (frameActual >= max) frameActual = 0;
}

QPixmap Gladiador::obtenerSpriteActual()
{
    QVector<QPixmap>* v = nullptr;
    switch (estadoActual) {
    case PARADO: v = &animIdle; break;
    case CAMINANDO: v = &animWalk; break;
    case SALTANDO: v = &animJump; break;
    case ATACANDO: v = &animAttack; break;
    case MUERTO: v = &animDeath; break;
    }

    if (!v || v->isEmpty()) {
        QPixmap ph(110, 140);
        ph.fill(Qt::transparent);
        return ph;
    }

    if (frameActual < 0 || frameActual >= v->size()) frameActual = 0;

    QPixmap px = v->at(frameActual);
    if (px.isNull()) {
        QPixmap ph(110, 140);
        ph.fill(Qt::transparent);
        return ph;
    }

    px = px.scaled(110, 140, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if (!mirandoDerecha) px = px.transformed(QTransform().scale(-1, 1));

    return px;
}

void Gladiador::actualizarSprite()
{
    QPixmap px = obtenerSpriteActual();
    if (px.isNull()) return;
    setPixmap(px);
    setOffset(-px.width() / 2, -px.height());
}

QRectF Gladiador::getBoundingBox() const
{
    qreal w = pixmap().width();
    qreal h = pixmap().height();
    return QRectF(x() - w/2, y() - h, w, h);
}

void Gladiador::setVida(int nuevaVida)
{
    vida = qBound(0, nuevaVida, 100);
    emit vidaCambiada(vida);
    if (vida == 0) {
        estadoActual = MUERTO;
        frameActual = 0;
        emit murio();
    }
}

void Gladiador::recibirDanio(int danio)
{
    setVida(vida - danio);
}

