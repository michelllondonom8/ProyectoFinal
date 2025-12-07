#include "nivel1.h"
#include "gladiador.h"
#include "enemigo.h"
#include <QMessageBox>
#include <QGraphicsPixmapItem>

Nivel1::Nivel1(QWidget *parent)
    : Nivel(1, parent),
<<<<<<< HEAD
    timerSegundo(nullptr),
    enemigosEliminados(0),
    tiempoTranscurrido(0),
    musicaNivel(nullptr),
    audioOutput(nullptr)
=======
      timerSegundo(nullptr),
      enemigosEliminados(0),
      tiempoTranscurrido(0),
      musicaNivel(nullptr),
      audioOutput(nullptr)
>>>>>>> f323b5d112ae3d7b56a44b2e4a0e6c38e4912fa4
{
    setWindowTitle("Nivel 1 - Coliseo Romano");
    inicializarNivel();
}

Nivel1::~Nivel1()
{
    if (musicaNivel) {
        musicaNivel->stop();
        delete musicaNivel;
        musicaNivel = nullptr;
    }
    if (audioOutput) {
        delete audioOutput;
        audioOutput = nullptr;
    }
    if (timerSegundo) {
        timerSegundo->stop();
        delete timerSegundo;
        timerSegundo = nullptr;
    }
    for (Enemigo *e : enemigos) {
        if (e) {
            escena->removeItem(e);
            delete e;
        }
    }
    enemigos.clear();
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
            nivelActivo = false;
            timerJuego->stop();
            if (timerSegundo) timerSegundo->stop();
            QTimer::singleShot(2000, this, [this]() {
                finalizarNivel(false);
            });
        }
    });
    generarEnemigo(0);

    timerSegundo = new QTimer(this);
    connect(timerSegundo, &QTimer::timeout, this, &Nivel1::actualizarTemporizador);
    timerSegundo->start(1000);
    musicaNivel = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicaNivel->setAudioOutput(audioOutput);

    QUrl url = QUrl("qrc:/sounds/Gladiator Arena.mp3");
    musicaNivel->setSource(url);
    audioOutput->setVolume(0.5);
    musicaNivel->setLoops(QMediaPlayer::Infinite);
    musicaNivel->play();

    musicaNivel = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicaNivel->setAudioOutput(audioOutput);

    QUrl url = QUrl("qrc:/sounds/Gladiator Arena.mp3");
    musicaNivel->setSource(url);
    audioOutput->setVolume(0.5);
    musicaNivel->setLoops(QMediaPlayer::Infinite);
    musicaNivel->play();

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

void Nivel1::generarEnemigo(int tipo)
{
    int escenaAlto = escena->height();
    Enemigo *enemigo = nullptr;

    if (tipo == 0) {
        enemigo = new Enemigo(false);
        enemigo->setPos(800, escenaAlto);
    } else if (tipo == 1) {
        enemigo = new Enemigo(false);
        enemigo->setPos(1000, escenaAlto);
    } else if (tipo == 2) {
        enemigo = new Enemigo(true);
        enemigo->setPos(1150, escenaAlto);
    }

    if (enemigo) {
        escena->addItem(enemigo);
        enemigos.append(enemigo);

        connect(enemigo, &Enemigo::ataque, this, [this, enemigo]() {
            if (jugador && enemigo && jugador->estaVivo() && enemigo->estaVivo()) {
<<<<<<< HEAD
                QRectF rectJugador = jugador->getBoundingBox();
                QRectF rangoAtaqueEnemigo = enemigo->getRangoAtaque();

=======

                QRectF rectJugador = jugador->getBoundingBox();
                QRectF rangoAtaqueEnemigo = enemigo->getRangoAtaque();

>>>>>>> f323b5d112ae3d7b56a44b2e4a0e6c38e4912fa4
                if (rangoAtaqueEnemigo.intersects(rectJugador)) {
                    int danio = enemigo->esFuerteEnemigo() ? 15 : 12;
                    jugador->recibirDanio(danio);
                }
            }
        });

        connect(enemigo, &Enemigo::murio, this, [this, enemigo]() {
            if (enemigos.contains(enemigo)) {
                escena->removeItem(enemigo);
                enemigos.removeOne(enemigo);
                enemigo->deleteLater();
            }
        });
    }
}

void Nivel1::actualizarJuego()
{
    if (!nivelActivo) return;

    if (teclaIzquierda) jugador->moverIzquierda();
    else if (teclaDerecha) jugador->moverDerecha();
    else jugador->detener();

    if (teclaSalto) { jugador->saltar(); teclaSalto = false; }
    if (teclaAtaque) { jugador->atacar(); teclaAtaque = false; }

    jugador->actualizar();

    for (Enemigo *e : enemigos) {
<<<<<<< HEAD
        if (e){
            e->actualizar(jugador->pos());
        }
=======
        if (e) e->actualizar(jugador->pos());
>>>>>>> f323b5d112ae3d7b56a44b2e4a0e6c38e4912fa4
    }

    verificarColisiones();

    bool todosGenerados = (tiempoTranscurrido >= 10);
    if (todosGenerados) {

        int enemigosVivos = 0;
        bool hayEnemigosDesapareciendo = false;
<<<<<<< HEAD
        for (Enemigo *e : enemigos) {
            if (e) {
                if (e && e->estaVivo()) {
                    enemigosVivos++;
                }else{
                    hayEnemigosDesapareciendo = true;
                }
            }
        }
=======

        for (Enemigo *e : enemigos) {
            if (e) {
                if (e->estaVivo()) enemigosVivos++;
                else hayEnemigosDesapareciendo = true;
            }
        }

>>>>>>> f323b5d112ae3d7b56a44b2e4a0e6c38e4912fa4
        if (enemigosVivos == 0 && !hayEnemigosDesapareciendo && enemigos.isEmpty()) {
            nivelActivo = false;
            timerJuego->stop();
            if (timerSegundo) timerSegundo->stop();

            QTimer::singleShot(1000, this, [this]() {
                finalizarNivel(true);
            });
        }
    }
}

void Nivel1::verificarColisiones()
{
    if (!jugador || !jugador->estaVivo()) return;

    QRectF rectJugador = jugador->getBoundingBox();

    for (Enemigo *enemigo : enemigos) {

        if (!enemigo || !enemigo->estaVivo()) continue;

        QRectF rectEnemigo = enemigo->getBoundingBox();

        if (rectJugador.intersects(rectEnemigo)) {
            jugador->resolverColision(rectEnemigo);
        }
<<<<<<< HEAD
        if (jugador->estaAtacando()) {
            QRectF rangoAtaqueJugador = jugador->getRangoAtaque();

            if (rangoAtaqueJugador.intersects(rectEnemigo)) {
=======

        if (jugador->estaAtacando()) {

            QRectF rangoAtaqueJugador = jugador->getRangoAtaque();

            if (rangoAtaqueJugador.intersects(rectEnemigo)) {

>>>>>>> f323b5d112ae3d7b56a44b2e4a0e6c38e4912fa4
                int danio = 4;
                enemigo->recibirDanio(danio);

                if (!enemigo->estaVivo()) {
                    enemigosEliminados++;
                }
            }
        }
    }
}

void Nivel1::actualizarTemporizador()
{
    if (!nivelActivo) return;

    tiempoRestante--;
    tiempoTranscurrido++;

    if (tiempoTranscurrido == 5 && enemigos.size() < 2) generarEnemigo(1);
    else if (tiempoTranscurrido == 10 && enemigos.size() < 3) generarEnemigo(2);

    int minutos = tiempoRestante / 60;
    int segundos = tiempoRestante % 60;

    labelTiempo->setText(QString("Tiempo: %1:%2")
                             .arg(minutos)
                             .arg(segundos, 2, 10, QChar('0')));

    if (tiempoRestante <= 0) {
        nivelActivo = false;
        timerJuego->stop();
        timerSegundo->stop();
        finalizarNivel(false);
    }

}
