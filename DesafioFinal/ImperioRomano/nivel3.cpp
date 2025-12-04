#include "nivel3.h"
#include "gladiador.h"
#include "enemigo.h"
#include "Catapulta.h"
#include "proyectil.h"
#include <QDebug>

Nivel3::Nivel3(QWidget *parent)
    : Nivel(3, parent),
    catapulta(nullptr),
    puedeDisparar(true),
    enemigosNormalesGenerados(0),
    enemigosNormalesEliminados(0),
    enemigosInteligentesGenerados(0),
    enemigosInteligentesEliminados(0),
    timerGeneracion(nullptr),
    timerSegundo(nullptr),
    contadorGeneracion(0),
    musicaNivel3(nullptr),
    audioOutput(nullptr)
{
    tiempoRestante = 120;
    inicializarNivel();
}

Nivel3::~Nivel3()
{
    if (musicaNivel3) {
        musicaNivel3->stop();
        delete musicaNivel3;
    }
    if (audioOutput) {
        delete audioOutput;
    }
    qDeleteAll(enemigos);
    qDeleteAll(proyectiles);

    if (catapulta) delete catapulta;

    if (timerGeneracion) {
        timerGeneracion->stop();
        delete timerGeneracion;
    }

    if (timerSegundo) {
        timerSegundo->stop();
        delete timerSegundo;
    }
}

void Nivel3::inicializarNivel()
{
    cargarFondo();
    crearCatapulta();

    jugador = new Gladiador();
    jugador->setPos(60, escena->height());
    escena->addItem(jugador);

    timerGeneracion = new QTimer(this);
    connect(timerGeneracion, &QTimer::timeout, this, [this]() {
        contadorGeneracion++;

        if (enemigosNormalesGenerados < 7 && contadorGeneracion % 180 == 0) {
            generarEnemigo(false);
            enemigosNormalesGenerados++;
        }
        else if (enemigosNormalesGenerados >= 7 &&
                 enemigosInteligentesGenerados < 3 &&
                 contadorGeneracion % 240 == 0) {
            generarEnemigo(true);
            enemigosInteligentesGenerados++;
        }
    });
    timerGeneracion->start(16);

    timerSegundo = new QTimer(this);
    connect(timerSegundo, &QTimer::timeout, this, [this]() {
        if (!nivelActivo) return;

        tiempoRestante--;
        int minutos = tiempoRestante / 60;
        int segundos = tiempoRestante % 60;
        labelTiempo->setText(QString("Tiempo: %1:%2")
                                 .arg(minutos)
                                 .arg(segundos, 2, 10, QChar('0')));

        if (tiempoRestante <= 0) {
            nivelActivo = false;
            timerJuego->stop();
            timerGeneracion->stop();
            timerSegundo->stop();
            finalizarNivel(false);
        }
    });
    timerSegundo->start(1000);

    labelTiempo->setText("Tiempo: 2:00");
    musicaNivel3 = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicaNivel3->setAudioOutput(audioOutput);

    QUrl url = QUrl("qrc:/sounds/nivel3musica.mp3");
    musicaNivel3->setSource(url);
    audioOutput->setVolume(0.6);
    musicaNivel3->setLoops(QMediaPlayer::Infinite);
    musicaNivel3->play();
    nivelActivo = true;
}

void Nivel3::cargarFondo()
{
    QPixmap fondo(":/images/Nivel3_fondo.jpeg");

    if (!fondo.isNull()) {
        QPixmap scaled = fondo.scaled(escena->width(), escena->height(),
                                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        escena->setBackgroundBrush(scaled);
    } else {
        escena->setBackgroundBrush(QColor(135, 206, 235));
    }
}

void Nivel3::crearCatapulta()
{
    double x = 180;
    double y = escena->height();

    qDebug() << "Creando catapulta - Escena height:" << escena->height();

    catapulta = new Catapulta(x, y);
    escena->addItem(catapulta);
}

void Nivel3::disparar()
{
    if (!puedeDisparar || !nivelActivo || !catapulta) return;

    puedeDisparar = false;

    // Obtener punto de lanzamiento
    double x0, y0;
    catapulta->getPuntoLanzamiento(x0, y0);

    qDebug() << "=== DISPARANDO ===";
    qDebug() << "Ángulo:" << catapulta->getAngulo();
    qDebug() << "Posición inicial:" << x0 << y0;

    // Crear proyectil
    Proyectil *p = new Proyectil(
        x0, y0,
        catapulta->getAngulo(),
        VELOCIDAD_INICIAL,
        GRAVEDAD
        );

    escena->addItem(p);
    proyectiles.append(p);

    // Permitir nuevo disparo
    QTimer::singleShot(500, this, [this]() {
        puedeDisparar = true;
    });
}

void Nivel3::generarEnemigo(bool fuerte)
{
    Enemigo *enemigo = new Enemigo(fuerte);
    enemigo->setPos(escena->width() + 50, escena->height());
    escena->addItem(enemigo);
    enemigos.append(enemigo);

    connect(enemigo, &Enemigo::murio, this, [this, enemigo, fuerte]() {
        if (fuerte) enemigosInteligentesEliminados++;
        else enemigosNormalesEliminados++;

        QTimer::singleShot(2000, this, [this, enemigo]() {
            if (enemigos.contains(enemigo)) {
                escena->removeItem(enemigo);
                enemigos.removeOne(enemigo);
                enemigo->deleteLater();
            }
        });
    });
}

void Nivel3::actualizarProyectiles()
{
    for (int i = 0; i < proyectiles.size(); i++) {
        Proyectil *p = proyectiles[i];

        if (!p->estaActivo()) {
            escena->removeItem(p);
            delete p;
            proyectiles.removeAt(i);
            i--;
            continue;
        }

        p->actualizar(DT);

        if (p->getY() > escena->height() || p->getX() > escena->width() || p->getX() < 0) {
            p->desactivar();
        }
    }
}

void Nivel3::actualizarEnemigos()
{
    for (Enemigo *e : enemigos) {
        if (e && e->estaVivo()) {
            QPointF pos = e->pos();
            qreal velocidad = e->esFuerteEnemigo() ? 1.5 : 2.0;
            pos.setX(pos.x() - velocidad);
            e->setPos(pos);

            if (pos.x() < 200) {
                nivelActivo = false;
                timerJuego->stop();
                timerGeneracion->stop();
                if (timerSegundo) timerSegundo->stop();
                finalizarNivel(false);
                return;
            }
        }
    }
}

void Nivel3::verificarColisiones()
{
    for (Proyectil *p : proyectiles) {
        if (!p->estaActivo()) continue;

        QRectF rectProyectil(p->getX() - 17, p->getY() - 17, 45, 45);

        for (Enemigo *e : enemigos) {
            if (!e || !e->estaVivo()) continue;

            QRectF rectEnemigo = e->getBoundingBox();

            if (rectProyectil.intersects(rectEnemigo)) {
                int danio = e->esFuerteEnemigo() ? 75 : 50;
                e->recibirDanio(danio);
                p->desactivar();
                break;
            }
        }
    }
}

void Nivel3::actualizarJuego()
{
    if (!nivelActivo) return;

    if (teclaIzquierda && catapulta) {
        catapulta->setAngulo(catapulta->getAngulo() + 1.0);
    }
    else if (teclaDerecha && catapulta) {
        catapulta->setAngulo(catapulta->getAngulo() - 1.0);
    }

    if (teclaAtaque) {
        disparar();
        teclaAtaque = false;
    }

    actualizarProyectiles();
    actualizarEnemigos();
    verificarColisiones();

    if (enemigosNormalesEliminados >= 7 && enemigosInteligentesEliminados >= 3) {
        bool hayVivos = false;
        for (Enemigo *e : enemigos) {
            if (e && e->estaVivo()) {
                hayVivos = true;
                break;
            }
        }

        if (!hayVivos) {
            nivelActivo = false;
            timerJuego->stop();
            timerGeneracion->stop();
            if (timerSegundo) timerSegundo->stop();

            QTimer::singleShot(1000, this, [this]() {
                finalizarNivel(true);
            });
        }
    }
}

