#ifndef NIVEL3_H
#define NIVEL3_H

#include "nivel.h"
#include <QList>
#include <QMouseEvent>

class Enemigo;
class Catapulta;
class Proyectil;

class Nivel3 : public Nivel
{
    Q_OBJECT

public:
    explicit Nivel3(QWidget *parent = nullptr);
    ~Nivel3();

protected:
    // Implementación de métodos virtuales puros de Nivel
    void inicializarNivel() override;
    void actualizarJuego() override;
    void verificarColisiones() override;
    void cargarFondo() override;

    // Métodos específicos de Nivel3
    void crearCatapulta();
    void generarOleada();
    void actualizarProyectiles();
    void actualizarEnemigos();
    void verificarColisionesProyectiles();
    void limpiarProyectiles();
    void verificarCondicionesVictoria();

    // Evento de mouse para disparar
    void mousePressEvent(QMouseEvent *event) override;

private:
    Catapulta *catapulta;
    QList<Enemigo*> enemigos;
    QList<Proyectil*> proyectiles;

    int oleadaActual;
    int enemigosEliminados;
    bool oleadaCompletada;
    int contadorOleada;

    QTimer *timerSegundo;
};

#endif // NIVEL3_H
