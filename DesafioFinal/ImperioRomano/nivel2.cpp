#include "nivel2.h"
#include "gladiador.h"
#include <QRandomGenerator>
#include <QGraphicsScene>

Nivel2::Nivel2(QWidget *parent)
    : Nivel(2, parent),
    desplazamientoFondo(0),
    distanciaRecorrida(0),
    distanciaObjetivo(3000),
    contadorGeneracion(0),
    repeticionesIntro(0),
    explosionActiva(false),
    puerta(nullptr),
    spriteExplosion(nullptr),
    timerExplosion(new QTimer(this)),
    frameExplosion(0)
{
    fxRock.setSource(QUrl("qrc:/sounds/rock_fall_1.wav"));
    fxRock.setLoopCount(1);
    fxRock.setVolume(0.8);

    fxPillar.setSource(QUrl("qrc:/sounds/pillar_crash_1.wav"));
    fxPillar.setLoopCount(1);
    fxPillar.setVolume(0.8);

    inicializarNivel();
}

Nivel2::~Nivel2()
{
    qDeleteAll(obstaculos);
}

void Nivel2::inicializarNivel()
{
    cargarFondo();
    crearFondos();

    jugador = new Gladiador();
    jugador->setPos(200, escena->height());
    escena->addItem(jugador);

    puerta = new QGraphicsRectItem(0,0,140,240);
    puerta->setBrush(Qt::yellow);
    puerta->setPos(distanciaObjetivo, escena->height() - 240);
    escena->addItem(puerta);

    spriteExplosion = new QGraphicsPixmapItem();
    spriteExplosion->setVisible(false);
    escena->addItem(spriteExplosion);

    connect(timerExplosion, &QTimer::timeout, this, [this]() {
        frameExplosion++;

        if (frameExplosion == 1)
            spriteExplosion->setPixmap(QPixmap(":/images/explosion_1.png"));
        else if (frameExplosion == 2)
            spriteExplosion->setPixmap(QPixmap(":/images/explosion_2.png"));
        else {
            timerExplosion->stop();
            spriteExplosion->setVisible(false);
            iniciarMuerteJugador();
        }
    });

    nivelActivo = true;
}

void Nivel2::cargarFondo()
{
    (void) QPixmap(":/images/Nivel2_fondo.jpeg");
}

void Nivel2::crearFondos()
{
    QPixmap f(":/images/Nivel2_fondo.jpeg");
    f = f.scaled(escena->width(), escena->height(), Qt::IgnoreAspectRatio);

    for (int i = 0; i < 3; i++) {
        QGraphicsPixmapItem *fp = new QGraphicsPixmapItem(f);
        fp->setPos(i * escena->width(), 0);
        escena->addItem(fp);
        fondos.append(fp);
    }
}

void Nivel2::generarObstaculos()
{
    contadorGeneracion++;
    if (contadorGeneracion % 40 != 0) return;

    bool piedra = QRandomGenerator::global()->bounded(100) < 70;
    int x = QRandomGenerator::global()->bounded(100, int(escena->width() - 100));

    QGraphicsPixmapItem *obj = new QGraphicsPixmapItem();

    if (piedra) {
        int f = QRandomGenerator::global()->bounded(0, 3);
        obj->setPixmap(QPixmap(QString(":/images/rock_%1.png").arg(f)).scaled(48,48));
        obj->setData(0, "piedra");
    } else {
        obj->setPixmap(QPixmap(":/images/pillar_anim_0.png").scaled(60,120));
        obj->setData(0, "pilar");
    }

    obj->setPos(x, -60);
    escena->addItem(obj);
    obstaculos.append(obj);
}

void Nivel2::actualizarObstaculos()
{
    if (explosionActiva) return;

    for (int i = 0; i < obstaculos.size(); i++) {
        QGraphicsPixmapItem *o = obstaculos[i];
        QString tipo = o->data(0).toString();

        if (tipo == "piedra") o->moveBy(0, 6);
        else o->moveBy(0, 4);

        if (o->y() > escena->height()) {
            escena->removeItem(o);
            delete o;
            obstaculos.removeAt(i);
            i--;
        }
    }
}

void Nivel2::iniciarExplosion(const QPointF &p)
{
    explosionActiva = true;
    spriteExplosion->setPixmap(QPixmap(":/images/explosion_0.png"));
    spriteExplosion->setPos(p);
    spriteExplosion->setVisible(true);

    frameExplosion = 0;
    timerExplosion->start(120);
}

void Nivel2::iniciarMuerteJugador()
{
    jugador->setPixmap(QPixmap(":/images/player_death_01.png"));

    QTimer::singleShot(300, this, [this]() {
        jugador->setPixmap(QPixmap(":/images/player_death_02.png"));
    });

    QTimer::singleShot(1500, this, [this]() {
        finalizarNivel(false);
    });
}

void Nivel2::verificarColisiones()
{
    if (explosionActiva) return;

    QRectF rj = jugador->getBoundingBox();

    for (QGraphicsPixmapItem *o : obstaculos) {

        if (rj.intersects(o->sceneBoundingRect())) {

            QString tipo = o->data(0).toString();

            if (tipo == "piedra") {
                fxRock.play();
            } else {
                fxPillar.play();
            }

            iniciarExplosion(o->pos());
            jugador->setVida(0);
            return;
        }
    }

    if (rj.intersects(puerta->sceneBoundingRect())) {

        if (repeticionesIntro < 2) {
            repeticionesIntro++;
            jugador->setPos(200, escena->height());
            distanciaRecorrida = 0;
            return;
        }

        finalizarNivel(true);
    }
}

void Nivel2::actualizarJuego()
{
    if (!nivelActivo || explosionActiva)
        return;

    if (teclaIzquierda) jugador->moverIzquierda();
    else if (teclaDerecha) jugador->moverDerecha();
    else jugador->detener();

    if (teclaSalto) { jugador->saltar(); teclaSalto = false; }

    jugador->actualizar();

    for (QGraphicsPixmapItem *f : fondos) {
        f->setX(f->x() - 2.5);
        if (f->x() <= -escena->width())
            f->setX(f->x() + escena->width() * 3);
    }

    distanciaRecorrida += 2;

    generarObstaculos();
    actualizarObstaculos();
    verificarColisiones();
}

