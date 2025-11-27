#include "nivel.h"
#include "gladiador.h"
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>
#include <QScreen>
#include <QGuiApplication>
#include <QPropertyAnimation>
#include <QEasingCurve>

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
    teclaAtaque(false),
    mensajeTexto(nullptr),
    fondoMensaje(nullptr)
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

void Nivel::mostrarMensajeVictoria()
{
    // Fondo semi-transparente con degradado
    if (!fondoMensaje) {
        fondoMensaje = new QGraphicsRectItem(0, 0, escena->width(), escena->height());
        QLinearGradient gradient(0, 0, 0, escena->height());
        gradient.setColorAt(0, QColor(0, 50, 0, 200));
        gradient.setColorAt(1, QColor(0, 100, 0, 200));
        fondoMensaje->setBrush(QBrush(gradient));
        fondoMensaje->setZValue(1000);
        escena->addItem(fondoMensaje);
    }
    fondoMensaje->setVisible(true);

    // Crear texto de victoria
    if (!mensajeTexto) {
        mensajeTexto = new QGraphicsTextItem();
        mensajeTexto->setZValue(1001);
        escena->addItem(mensajeTexto);
    }

    QString textoVictoria = QString("⚔️ NIVEL %1 CONQUISTADO ⚔️").arg(numeroNivel);

    mensajeTexto->setHtml(
        QString("<div style='text-align: center;'>"
                "<span style='color: gold; font-size: 56px; font-weight: bold; "
                "text-shadow: 3px 3px 6px black;'>%1</span><br>"
                "<span style='color: white; font-size: 28px;'>¡Gloria a Roma!</span>"
                "</div>").arg(textoVictoria)
        );

    // Centrar el texto
    qreal x = (escena->width() - mensajeTexto->boundingRect().width()) / 2;
    qreal y = (escena->height() - mensajeTexto->boundingRect().height()) / 2;
    mensajeTexto->setPos(x, y);
    mensajeTexto->setVisible(true);

    // ANIMACIÓN SIMPLE CON TIMER (sin QPropertyAnimation)
    fondoMensaje->setOpacity(0);
    mensajeTexto->setOpacity(0);

    // Fade in con timer
    QTimer *timer = new QTimer(this);
    qreal *opacity = new qreal(0.0);

    connect(timer, &QTimer::timeout, [this, timer, opacity]() {
        *opacity += 0.05;

        if (*opacity >= 1.0) {
            *opacity = 1.0;
            timer->stop();
            timer->deleteLater();
            delete opacity;
        }

        if (fondoMensaje) fondoMensaje->setOpacity(*opacity);
        if (mensajeTexto) mensajeTexto->setOpacity(*opacity);
    });

    timer->start(30);  // 30ms = ~33 fps
}

void Nivel::mostrarMensajeDerrota()
{
    // Fondo rojo sangre
    if (!fondoMensaje) {
        fondoMensaje = new QGraphicsRectItem(0, 0, escena->width(), escena->height());
        QRadialGradient gradient(escena->width()/2, escena->height()/2, escena->width()/2);
        gradient.setColorAt(0, QColor(150, 0, 0, 220));
        gradient.setColorAt(1, QColor(50, 0, 0, 250));
        fondoMensaje->setBrush(QBrush(gradient));
        fondoMensaje->setZValue(1000);
        escena->addItem(fondoMensaje);
    }
    fondoMensaje->setVisible(true);

    if (!mensajeTexto) {
        mensajeTexto = new QGraphicsTextItem();
        mensajeTexto->setZValue(1001);
        escena->addItem(mensajeTexto);
    }

    mensajeTexto->setHtml(
        "<div style='text-align: center;'>"
        "<span style='color: #FF3333; font-size: 64px; font-weight: bold; "
        "text-shadow: 4px 4px 8px black;'>💀 GAME OVER 💀</span><br>"
        "<span style='color: #FFAAAA; font-size: 24px;'>Roma ha caído...</span>"
        "</div>"
        );

    // Centrar
    qreal x = (escena->width() - mensajeTexto->boundingRect().width()) / 2;
    qreal y = (escena->height() - mensajeTexto->boundingRect().height()) / 2;
    mensajeTexto->setPos(x, y);
    mensajeTexto->setVisible(true);

    // ANIMACIÓN SIMPLE CON TIMER
    fondoMensaje->setOpacity(0);
    mensajeTexto->setOpacity(0);

    QTimer *timer = new QTimer(this);
    qreal *opacity = new qreal(0.0);

    connect(timer, &QTimer::timeout, [this, timer, opacity]() {
        *opacity += 0.04;  // Más lento que victoria

        if (*opacity >= 1.0) {
            *opacity = 1.0;
            timer->stop();
            timer->deleteLater();
            delete opacity;
        }

        if (fondoMensaje) fondoMensaje->setOpacity(*opacity);
        if (mensajeTexto) mensajeTexto->setOpacity(*opacity);
    });

    timer->start(40);  // Más lento para efecto dramático
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
        // Mostrar mensaje de victoria y esperar 3 segundos
        mostrarMensajeVictoria();

        QTimer::singleShot(4000, this, [this]() {
            emit nivelCompletado(numeroNivel);
        });
    } else {
        // Mostrar mensaje de derrota y esperar 3 segundos
        mostrarMensajeDerrota();

        QTimer::singleShot(4000, this, [this]() {
            emit nivelFallido();
        });
    }
}
