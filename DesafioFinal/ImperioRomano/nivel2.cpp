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
    frameExplosion(0),
    timerSegundo(nullptr),
    tiempoObjetivo(30)
{
    fxRock.setSource(QUrl("qrc:/sounds/rock_fall_1.wav"));
    fxRock.setLoopCount(1);
    fxRock.setVolume(0.8);

    fxPillar.setSource(QUrl("qrc:/sounds/pillar_crash_1.wav"));
    fxPillar.setLoopCount(1);
    fxPillar.setVolume(0.8);

    tiempoRestante = 60;
    inicializarNivel();
}

Nivel2::~Nivel2()
{
    qDeleteAll(obstaculos);
    if (timerSegundo) {
        timerSegundo->stop();
        delete timerSegundo;
    }
}

void Nivel2::inicializarNivel()
{
    cargarFondo();
    crearFondos();

    jugador = new Gladiador();
    jugador->setPos(200, escena->height());
    escena->addItem(jugador);

    connect(jugador, &Gladiador::vidaCambiada, this, [this](int vida) {
        barraVida->setValue(vida);
        if (vida <= 0) {
            nivelActivo = false;
            timerJuego->stop();
            if (timerSegundo) timerSegundo->stop();

            QTimer::singleShot(2000, this, [this]() {
                finalizarNivel(false);
            });
        }
    });
    puerta = new QGraphicsPixmapItem();

    QPixmap spritePuerta(":/images/Puerta.png");
    puerta->setPixmap(spritePuerta.scaled(150, 220, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    qreal distanciaPuerta = 8500;
    puerta->setPos(distanciaPuerta, escena->height() - 190);
    puerta->setZValue(100);
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
    timerSegundo = new QTimer(this);
    connect(timerSegundo, &QTimer::timeout, this, &Nivel2::actualizarTemporizador);
    timerSegundo->start(1000);
    tiempoRestante = 60;
    labelTiempo->setText(QString("Tiempo: 1:00"));

    nivelActivo = true;
}

void Nivel2::cargarFondo()
{
    (void) QPixmap(":/images/Nivel2_fondo.jpeg");
}

void Nivel2::crearFondos()
{
    QPixmap f(":/images/Nivel2_fondo.jpeg");
    f = f.scaled(escena->width(), escena->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    for (int i = 0; i < 5; i++) {
        QGraphicsPixmapItem *fp = new QGraphicsPixmapItem(f);
        fp->setPos(i * escena->width(), 0);
        fp->setZValue(-10);
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
    nivelActivo = false;
    timerJuego->stop();
    if (timerSegundo) timerSegundo->stop();

    // Usar el sistema de animación del gladiador en vez de cambiar pixmap directamente
    jugador->setVida(0);  // Esto activa el estado MUERTO en gladiador

    // Esperar a que se vea la animación de muerte del gladiador
    QTimer::singleShot(2000, this, [this]() {
        finalizarNivel(false);
    });
}

void Nivel2::actualizarTemporizador()
{
    if (!nivelActivo) return;

    tiempoRestante--;

    int minutos = tiempoRestante / 60;
    int segundos = tiempoRestante % 60;

    labelTiempo->setText(
        QString("Tiempo: %1:%2")
            .arg(minutos)
            .arg(segundos, 2, 10, QChar('0')));
    if (tiempoRestante <= 0) {
        nivelActivo = false;
        timerJuego->stop();
        timerSegundo->stop();

        finalizarNivel(false);
    }

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
    if (puerta && rj.intersects(puerta->sceneBoundingRect())) {
        nivelActivo = false;
        timerJuego->stop();
        if (timerSegundo) timerSegundo->stop();

        QTimer::singleShot(500, this, [this]() {
            finalizarNivel(true);
        });
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
    qreal velocidadScroll = 2.5;

    for (QGraphicsPixmapItem *f : fondos) {
        f->setX(f->x() - velocidadScroll);
        if (f->x() <= -escena->width()){
            qreal maxX = -999999;
            for (QGraphicsPixmapItem *fb : fondos) {
                if (fb->x() > maxX) {
                    maxX = fb->x();
                }
            }
            f->setX(maxX + escena->width());
        }
    }
    if (puerta) {
        puerta->setX(puerta->x() - velocidadScroll);
    }

    distanciaRecorrida += velocidadScroll;

    generarObstaculos();
    actualizarObstaculos();
    verificarColisiones();
}

