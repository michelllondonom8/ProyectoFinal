#ifndef ENEMIGO_H
#define ENEMIGO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRectF>

class Enemigo : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Enemigo(bool fuerte, QObject *parent = nullptr);

    void actualizar(const QPointF &posJugador);
    void recibirDanio(int danio);

    int getVida() const { return vida; }
    int getVidaMaxima() const { return vidaMaxima; }
    bool estaVivo() const { return vida > 0; }
    bool estaAtacando() const { return atacando; }
    bool esFuerteEnemigo() const { return esFuerte; }

    QRectF getBoundingBox() const;

signals:
    void murio();

private:
    bool esFuerte;
    int vida;
    int vidaMaxima;

    qreal velocidad;
    qreal rangoDeteccion;
    qreal rangoAtaque;

    bool atacando;
    int contadorAtaque;

    int frameActual;
    int contadorFrame;
    int framesPorAnimacion;
    bool mirandoIzquierda;

    enum EstadoAnimacion {
        IDLE,
        CAMINANDO,
        ATACANDO
    };
    EstadoAnimacion estadoActual;
    QPixmap crearPlaceholder() const;
    void actualizarAnimacion();
    void actualizarSprite();
};

#endif
