#include "nivel.h"
#include "gladiador.h"
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>
#include <QScreen>
#include <QGuiApplication>

Nivel::Nivel(int numero, QWidget *parent)
    : QGraphicsView(parent),
    escena(nullptr),
    jugador(nullptr),
    timerJuego(nullptr),
    numeroNivel(numero),
    tiempoRestante(60),
    nivelActivo(false),
    teclaIzquierda(false),
    teclaDerecha(false),
    teclaSalto(false),
    teclaAtaque(false)
{
    configurarEscena();
    configurarHUD();
}

Nivel::~Nivel()
{
    if (timerJuego) {
        timerJuego->stop();
        delete timerJuego;
    }
    if (escena) delete escena;
}

void Nivel::configurarEscena()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int anchoVentana = screenGeometry.width();
    int altoVentana = screenGeometry.height();
    escena = new QGraphicsScene(0, 0, anchoVentana, altoVentana, this);
    setScene(escena);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(anchoVentana, altoVentana);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet("background: black;");

    anchoPantalla = anchoVentana;
    altoPantalla = altoVentana;

    timerJuego = new QTimer(this);
    connect(timerJuego, &QTimer::timeout, this, &Nivel::actualizarJuego);
    timerJuego->start(16);
}

void Nivel::configurarHUD()
{
    barraVida = new QProgressBar();
    barraVida->setRange(0, 100);
    barraVida->setValue(100);
    barraVida->setTextVisible(true);
    barraVida->setFormat("Vida: %p%");
    barraVida->setStyleSheet(
        "QProgressBar {"
        "  border: 2px solid #555;"
        "  border-radius: 5px;"
        "  background-color: #333;"
        "  color: white;"
        "  text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #00AA00;"
        "}"
        );
    barraVida->setFixedSize(200, 30);

    labelTiempo = new QLabel("Tiempo: 1:00");
    labelTiempo->setStyleSheet(
        "QLabel {"
        "  color: white;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  background-color: rgba(0, 0, 0, 150);"
        "  padding: 10px;"
        "  border-radius: 5px;"
        "}"
        );

    QGraphicsProxyWidget *proxyVida = escena->addWidget(barraVida);
    proxyVida->setPos(10, 10);

    QGraphicsProxyWidget *proxyTiempo = escena->addWidget(labelTiempo);
    proxyTiempo->setPos(1200 - 150, 10);
}

void Nivel::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        teclaIzquierda = true;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        teclaDerecha = true;
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
    case Qt::Key_Space:
        teclaSalto = true;
        break;
    case Qt::Key_Control:
    case Qt::Key_Return:
        teclaAtaque = true;
        break;
    case Qt::Key_Escape:
        finalizarNivel(false);
        break;
    }

    QGraphicsView::keyPressEvent(event);
}

void Nivel::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        teclaIzquierda = false;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        teclaDerecha = false;
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
    case Qt::Key_Space:
        teclaSalto = false;
        break;
    case Qt::Key_Control:
    case Qt::Key_Return:
        teclaAtaque = false;
        break;
    }

    QGraphicsView::keyReleaseEvent(event);
}

void Nivel::finalizarNivel(bool exitoso)
{
    nivelActivo = false;
    if (timerJuego) {
        timerJuego->stop();
    }

    if (exitoso) {
        emit nivelCompletado(numeroNivel);
    } else {
        emit nivelFallido();
    }
}
