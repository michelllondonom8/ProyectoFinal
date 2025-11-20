#include "nivel3.h"
#include "enemigo.h"
#include "catapulta.h"
#include "proyectil.h"
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QRandomGenerator>

Nivel3::Nivel3(QWidget *parent)
    : Nivel(3, parent),
    catapulta(nullptr),
    oleadaActual(1),
    enemigosEliminados(0),
    oleadaCompletada(false),
    contadorOleada(0),
    timerSegundo(nullptr)
{
    setWindowTitle("Nivel 3 - Defensa de Roma");
    tiempoRestante = 120; // 2 minutos
    inicializarNivel();
}

Nivel3::~Nivel3()
{
    if (timerSegundo) {
        timerSegundo->stop();
        delete timerSegundo;
    }
    qDeleteAll(enemigos);
    qDeleteAll(proyectiles);
}

void Nivel3::inicializarNivel()
{
    cargarFondo();

    // No hay jugador gladiador en este nivel
    jugador = nullptr;

    // Ocultar barra de vida (no se usa en este nivel)
    barraVida->hide();

    // Crear catapulta
    crearCatapulta();

    // Crear puerta de Roma (objetivo a defender)
    QGraphicsRectItem *puerta = new QGraphicsRectItem(1150, 400, 50, 200);
    puerta->setBrush(QBrush(QColor(139, 69, 19)));
    escena->addItem(puerta);

    // Timer para el cronómetro
    timerSegundo = new QTimer(this);
    connect(timerSegundo, &QTimer::timeout, this, [this]() {
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
    });
    timerSegundo->start(1000);

    // Generar primera oleada
    generarOleada();

    // Actualizar label de tiempo
    labelTiempo->setText("Tiempo: 2:00");

    nivelActivo = true;
}

void Nivel3::cargarFondo()
{
    QPixmap fondo(":/images/Nivel3_fondo.jpeg");

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
        escena->setBackgroundBrush(QBrush(QColor(135, 206, 235))); // Cielo azul
    }
}

void Nivel3::crearCatapulta()
{
    catapulta = new Catapulta(this);
    escena->addItem(catapulta);

    // Conectar señal de disparo
    connect(catapulta, &Catapulta::proyectilDisparado, this, [this](Proyectil *proyectil) {
        escena->addItem(proyectil);
        proyectiles.append(proyectil);

        connect(proyectil, &Proyectil::fueraDeLimites, this, [this, proyectil]() {
            escena->removeItem(proyectil);
            proyectiles.removeOne(proyectil);
            proyectil->deleteLater();
        });
    });
}

void Nivel3::generarOleada()
{
    // Limpiar enemigos anteriores
    qDeleteAll(enemigos);
    enemigos.clear();
    oleadaCompletada = false;
    contadorOleada = 0;

    switch (oleadaActual) {
    case 1:
        // Oleada 1: Un enemigo grande (2 vidas)
        {
            Enemigo *grande = new Enemigo(true); // true = fuerte/grande
            grande->setPos(1150, 480);
            escena->addItem(grande);
            enemigos.append(grande);
        }
        break;

    case 2:
        // Oleada 2: Dos enemigos pequeños (1 vida cada uno)
        {
            Enemigo *pequeno1 = new Enemigo(false);
            pequeno1->setPos(1150, 500);
            escena->addItem(pequeno1);
            enemigos.append(pequeno1);

            Enemigo *pequeno2 = new Enemigo(false);
            pequeno2->setPos(1100, 500);
            escena->addItem(pequeno2);
            enemigos.append(pequeno2);
        }
        break;

    case 3:
        // Oleada 3: Combinación de enemigos
        {
            Enemigo *grande = new Enemigo(true);
            grande->setPos(1150, 480);
            escena->addItem(grande);
            enemigos.append(grande);

            Enemigo *pequeno1 = new Enemigo(false);
            pequeno1->setPos(1100, 500);
            escena->addItem(pequeno1);
            enemigos.append(pequeno1);

            Enemigo *pequeno2 = new Enemigo(false);
            pequeno2->setPos(1050, 500);
            escena->addItem(pequeno2);
            enemigos.append(pequeno2);
        }
        break;
    }
}

void Nivel3::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && nivelActivo && catapulta) {
        catapulta->disparar();
    }
    QGraphicsView::mousePressEvent(event);
}

void Nivel3::actualizarJuego()
{
    if (!nivelActivo) return;

    // Actualizar catapulta (oscilación)
    if (catapulta) {
        catapulta->actualizar();
    }

    // Actualizar proyectiles
    actualizarProyectiles();

    // Actualizar enemigos
    actualizarEnemigos();

    // Verificar colisiones
    verificarColisionesProyectiles();

    // Limpiar proyectiles fuera de pantalla
    limpiarProyectiles();

    // Verificar condiciones
    verificarCondicionesVictoria();
}

void Nivel3::verificarColisiones()
{
    // Este método se requiere por la clase base, pero usamos verificarColisionesProyectiles
    verificarColisionesProyectiles();
}

void Nivel3::actualizarProyectiles()
{
    for (Proyectil *proyectil : proyectiles) {
        proyectil->actualizar();
    }
}

void Nivel3::actualizarEnemigos()
{
    for (int i = 0; i < enemigos.size(); ++i) {
        Enemigo *enemigo = enemigos[i];

        // Mover enemigo hacia la izquierda (hacia la puerta)
        enemigo->setPos(enemigo->x() - 2, enemigo->y());

        // Si llega a la puerta, el jugador pierde
        if (enemigo->x() <= 150) {
            finalizarNivel(false);
            QMessageBox::critical(this, "Derrota", "Los enemigos han llegado a la puerta de Roma.");
            return;
        }
    }
}

void Nivel3::verificarColisionesProyectiles()
{
    for (int i = 0; i < proyectiles.size(); ++i) {
        Proyectil *proyectil = proyectiles[i];
        QRectF rectProyectil = proyectil->sceneBoundingRect();

        for (int j = 0; j < enemigos.size(); ++j) {
            Enemigo *enemigo = enemigos[j];
            QRectF rectEnemigo = enemigo->getBoundingBox();

            if (rectProyectil.intersects(rectEnemigo)) {
                // Golpe exitoso
                enemigo->recibirDanio(100); // Daño suficiente para matar enemigos normales

                // Eliminar proyectil
                escena->removeItem(proyectil);
                proyectiles.removeAt(i);
                delete proyectil;
                i--;

                // Si el enemigo murió, eliminarlo
                if (!enemigo->estaVivo()) {
                    escena->removeItem(enemigo);
                    enemigos.removeAt(j);
                    delete enemigo;
                    enemigosEliminados++;
                }

                break;
            }
        }
    }
}

void Nivel3::limpiarProyectiles()
{
    for (int i = 0; i < proyectiles.size(); ++i) {
        Proyectil *proyectil = proyectiles[i];

        if (!proyectil->estaDentroDelLimite()) {
            escena->removeItem(proyectil);
            proyectiles.removeAt(i);
            delete proyectil;
            i--;
        }
    }
}

void Nivel3::verificarCondicionesVictoria()
{
    // Si todos los enemigos de la oleada fueron eliminados
    if (enemigos.isEmpty() && !oleadaCompletada) {
        oleadaCompletada = true;
        contadorOleada = 0;

        if (oleadaActual < 3) {
            oleadaActual++;
            QMessageBox::information(this, "Oleada completada",
                                     QString("¡Oleada %1 completada!\nPreparate para la siguiente oleada.").arg(oleadaActual - 1));
            generarOleada();
        } else {
            // Victoria total
            finalizarNivel(true);
            QMessageBox::information(this, "Victoria", "¡Has defendido Roma exitosamente!");
        }
    }
}
