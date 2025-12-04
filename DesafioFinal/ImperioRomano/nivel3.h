#ifndef NIVEL3_H
#define NIVEL3_H

#include "nivel.h"
#include "Catapulta.h"
#include "Proyectil.h"
#include <QList>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class Enemigo;

class Nivel3 : public Nivel
{
    Q_OBJECT
public:
    explicit Nivel3(QWidget *parent = nullptr);
    ~Nivel3();

protected:
    void inicializarNivel() override;
    void actualizarJuego() override;
    void verificarColisiones() override;
    void cargarFondo() override;

private:
    void crearCatapulta();
    void disparar();
    void actualizarProyectiles();

    Catapulta *catapulta;
    bool puedeDisparar;
    QList<Proyectil*> proyectiles;

    void generarEnemigo(bool fuerte);
    void actualizarEnemigos();
    QList<Enemigo*> enemigos;

    int enemigosNormalesGenerados;
    int enemigosNormalesEliminados;
    int enemigosInteligentesGenerados;
    int enemigosInteligentesEliminados;

    QTimer *timerGeneracion;
    QTimer *timerSegundo;
    QMediaPlayer *musicaNivel3;
    QAudioOutput *audioOutput;
    int contadorGeneracion;

    const double GRAVEDAD = 0.4;
    const double VELOCIDAD_INICIAL = 15.0;
    const double DT = 1.0;
};

#endif // NIVEL3_H
