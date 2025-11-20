#ifndef NIVEL_H
#define NIVEL_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QProgressBar>
#include <QLabel>

class Gladiador;

class Nivel : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Nivel(int numero, QWidget *parent = nullptr);
    virtual ~Nivel();

signals:
    void nivelCompletado(int numeroNivel);
    void nivelFallido();

protected:
    // Métodos virtuales para ser implementados por cada nivel
    virtual void inicializarNivel() = 0;
    virtual void actualizarJuego() = 0;
    virtual void verificarColisiones() = 0;
    virtual void cargarFondo() = 0;

    void configurarEscena();
    void configurarHUD();
    void finalizarNivel(bool exitoso);

    // Eventos de teclado
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // Miembros protegidos
    QGraphicsScene *escena;
    Gladiador *jugador;
    QTimer *timerJuego;

    QProgressBar *barraVida;
    QLabel *labelTiempo;

    int numeroNivel;
    int tiempoRestante;
    bool nivelActivo;

    // Dimensiones de la pantalla
    int anchoPantalla;
    int altoPantalla;

    // Estado de teclas
    bool teclaIzquierda;
    bool teclaDerecha;
    bool teclaSalto;
    bool teclaAtaque;
};

#endif // NIVEL_H
