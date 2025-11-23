#include "nivel1.h"
#include "gladiador.h"
#include "enemigo.h"
#include <QMessageBox>
#include <QGraphicsPixmapItem>

Nivel1::Nivel1(QWidget *parent)
    : Nivel(1, parent),
    timerSegundo(nullptr),
    enemigosEliminados(0)
{
    setWindowTitle("Nivel 1 - Coliseo Romano");
    inicializarNivel();
}

Nivel1::~Nivel1()
{
    if (timerSegundo) {
        timerSegundo->stop();
        delete timerSegundo;
    }
    qDeleteAll(enemigos);
}

void Nivel1::inicializarNivel()
{
    cargarFondo();

    jugador = new Gladiador();
    escena->addItem(jugador);
    jugador->setPos(150, escena->height());

    connect(jugador, &Gladiador::vidaCambiada, this, [this](int vida) {
        barraVida->setValue(vida);
        if (vida <= 0) {
            finalizarNivel(false);
            QMessageBox::critical(this, "Derrota", "Has sido derrotado en el Coliseo.");
        }
    });
    crearEnemigos();

    timerSegundo = new QTimer(this);
    connect(timerSegundo, &QTimer::timeout, this, &Nivel1::actualizarTemporizador);
    timerSegundo->start(1000);

    nivelActivo = true;
}

void Nivel1::cargarFondo()
{
    QPixmap fondo(":/images/Fondo1.jpg");

    if (!fondo.isNull()) {
        QPixmap scaled = fondo.scaled(
            escena->width(),
            escena->height(),
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );

        escena->setBackgroundBrush(scaled);
    } else {
        escena->setBackgroundBrush(Qt::black);
    }
}

void Nivel1::crearEnemigos()
{
    int escenaAlto = escena->height();
    Enemigo *e1 = new Enemigo(false);
    escena->addItem(e1);
    e1->setPos(800, escenaAlto);
    enemigos.append(e1);

    Enemigo *e2 = new Enemigo(false);
    escena->addItem(e2);
    e2->setPos(950, escenaAlto);
    enemigos.append(e2);

    Enemigo *f = new Enemigo(true);
    escena->addItem(f);
    f->setPos(1100, escenaAlto);
    enemigos.append(f);
}

void Nivel1::actualizarJuego()
{
    if (!nivelActivo) return;

    if (teclaIzquierda) {
        jugador->moverIzquierda();
    } else if (teclaDerecha) {
        jugador->moverDerecha();
    } else {
        jugador->detener();
    }

    if (teclaSalto) {
        jugador->saltar();
        teclaSalto = false;
    }

    if (teclaAtaque) {
        jugador->atacar();
        teclaAtaque = false;
    }

    jugador->actualizar();

    for (Enemigo *e : enemigos) {
        if (e) {
            e->actualizar(jugador->pos());
        }
    }
    verificarColisiones();
}

void Nivel1::verificarColisiones()
{
    if(!jugador) return;
    QRectF rectJugador = jugador->getBoundingBox();
    QList<Enemigo*> muertos;

    for (Enemigo *enemigo : enemigos){
        if (!enemigo) continue;

        QRectF rectEnemigo = enemigo->getBoundingBox();

        if (rectJugador.intersects(rectEnemigo)) {
            if (jugador->estaAtacando()) {
                enemigo->recibirDanio(10);
                if (!enemigo->estaVivo()) {
                    muertos.append(enemigo);
                    enemigosEliminados++;
                }
            }
            else if (enemigo->estaAtacando()) {
                jugador->recibirDanio(5);
            }
        }
    }
    for(Enemigo *m : muertos){
        escena->removeItem(m);
        enemigos.removeOne(m);
        delete m;
}
}

void Nivel1::actualizarTemporizador()
{
    tiempoRestante--;

    int minutos = tiempoRestante / 60;
    int segundos = tiempoRestante % 60;

    labelTiempo->setText(QString("Tiempo: %1:%2")
                             .arg(minutos)
                             .arg(segundos, 2, 10, QChar('0')));

    if (tiempoRestante <= 0) {
        finalizarNivel(false);
        QMessageBox::critical(this, "Tiempo agotado", "El tiempo terminó.");
    }
}
