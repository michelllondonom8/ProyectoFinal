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
    // Implementación de métodos virtuales puros de Nivel
    void inicializarNivel() override;
    void actualizarJuego() override;
    void verificarColisiones() override;
    void cargarFondo() override;

    // Métodos específicos de Nivel1
    void crearEnemigos();
    void actualizarTemporizador();
    void verificarCondicionesVictoria();

private:
    QList<Enemigo*> enemigos;
    QTimer *timerSegundo;
    int enemigosEliminados;
};

#endif // NIVEL1_H
