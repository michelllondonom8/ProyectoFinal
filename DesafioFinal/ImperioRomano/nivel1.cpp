#include "nivel1.h"
#include "gladiador.h"
#include "enemigo.h"
#include <QMessageBox>
#include <QGraphicsPixmapItem>

Nivel1::Nivel1(QWidget *parent)
    : Nivel(1, nullptr),
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
}

void Nivel1::inicializarNivel()
{
    cargarFondo();

    // Crear jugador
    jugador = new Gladiador();
    escena->addItem(jugador);

    // Conectar señales del jugador
    connect(jugador, &Gladiador::vidaCambiada, this, [this](int vida) {
        barraVida->setValue(vida);
        if (vida <= 0) {
            finalizarNivel(false);
            QMessageBox::critical(this, "Derrota", "Has sido derrotado en el Coliseo.");
        }
    });

    // Crear enemigos
    crearEnemigos();

    // Timer para el cronómetro
    timerSegundo = new QTimer(this);
    connect(timerSegundo, &QTimer::timeout, this, &Nivel1::actualizarTemporizador);
    timerSegundo->start(1000); // 1 segundo

    nivelActivo = true;
}

void Nivel1::cargarFondo()
{
    // Cargar imagen de fondo del coliseo
    QPixmap fondo(":/images/Fondo1.jpg");

    if (!fondo.isNull()) {
        // Escalar al tamaño de la escena
        QPixmap fondoEscalado = fondo.scaled(
            escena->width(),
            escena->height(),
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );

        escena->setBackgroundBrush(QBrush(fondoEscalado));
    } else {
        // Si no se carga la imagen, poner fondo de color
        escena->setBackgroundBrush(QBrush(QColor(139, 69, 19))); // Marrón
    }
}

void Nivel1::crearEnemigos()
{
    // Crear 2 enemigos normales y 1 fuerte
    Enemigo *enemigo1 = new Enemigo(false); // Normal
    enemigo1->setPos(800, 480);
    escena->addItem(enemigo1);
    enemigos.append(enemigo1);

    Enemigo *enemigo2 = new Enemigo(false); // Normal
    enemigo2->setPos(950, 480);
    escena->addItem(enemigo2);
    enemigos.append(enemigo2);

    Enemigo *enemigoFuerte = new Enemigo(true); // Fuerte (200% vida)
    enemigoFuerte->setPos(1050, 480);
    escena->addItem(enemigoFuerte);
    enemigos.append(enemigoFuerte);
}

void Nivel1::actualizarJuego()
{
    if (!nivelActivo) return;

    // Actualizar jugador según teclas presionadas
    if (teclaIzquierda) {
        jugador->moverIzquierda();
    } else if (teclaDerecha) {
        jugador->moverDerecha();
    } else {
        jugador->detener();
    }

    if (teclaSalto) {
        jugador->saltar();
        teclaSalto = false; // Para que no salte continuamente
    }

    if (teclaAtaque) {
        jugador->atacar();
        teclaAtaque = false;
    }

    jugador->actualizar();

    // Actualizar enemigos
    for (Enemigo *enemigo : enemigos) {
        if (enemigo) {
            enemigo->actualizar(jugador->pos());
        }
    }

    // Verificar colisiones
    verificarColisiones();
}

void Nivel1::verificarColisiones()
{
    QRectF rectJugador = jugador->getBoundingBox();

    for (int i = 0; i < enemigos.size(); ++i) {
        Enemigo *enemigo = enemigos[i];
        if (!enemigo) continue;

        QRectF rectEnemigo = enemigo->getBoundingBox();

        // Verificar colisión
        if (rectJugador.intersects(rectEnemigo)) {
            // Si el jugador está atacando
            if (jugador->estaAtacando()) {
                enemigo->recibirDanio(10);

                // Si el enemigo murió, eliminarlo
                if (!enemigo->estaVivo()) {
                    escena->removeItem(enemigo);
                    delete enemigo;
                    enemigos[i] = nullptr;
                    enemigosEliminados++;
                }
            }
            // Si el enemigo está atacando
            else if (enemigo->estaAtacando()) {
                jugador->recibirDanio(5);
            }
        }
    }

    // Limpiar punteros nulos
    enemigos.removeAll(nullptr);
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
        QMessageBox::critical(this, "Tiempo agotado", "Se acabó el tiempo.");
    }
}
