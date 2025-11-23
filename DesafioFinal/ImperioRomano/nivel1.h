#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivel.h"
#include <QList>

class Enemigo;

class Nivel1 : public Nivel
{
    Q_OBJECT

public:
    explicit Nivel1(QWidget *parent = nullptr);
    ~Nivel1();

protected:
    void inicializarNivel() override;
    void actualizarJuego() override;
    void verificarColisiones() override;
    void cargarFondo() override;
    void generarEnemigo(int tipo);
private slots:
    void actualizarTemporizador();
private:
    QList<Enemigo*> enemigos;
    QTimer *timerSegundo;
    int enemigosEliminados;
    int tiempoTranscurrido;
};

#endif // NIVEL1_H
