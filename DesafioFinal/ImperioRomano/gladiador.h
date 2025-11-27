#ifndef GLADIADOR_H
#define GLADIADOR_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVector>
#include <QRectF>

class Gladiador : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Gladiador(QObject *parent = nullptr);

    void moverIzquierda();
    void moverDerecha();
    void saltar();
    void atacar();
    void detener();
    void actualizar();

    int getVida() const { return vida; }
    bool estaAtacando() const { return atacando; }
    bool estaEnSuelo() const { return enSuelo; }
    bool estaVivo() const { return vida > 0; }
    QRectF getBoundingBox() const;
    QRectF getRangoAtaque() const;

    void setVida(int nuevaVida);
    void recibirDanio(int danio);
    void resolverColision(const QRectF &obstaculoBBox);
    void aplicarKnockback(qreal fuerzaX);
signals:
    void vidaCambiada(int vida);
    void murio();

private:
    int vida;

    qreal velocidadX;
    qreal velocidadY;

    qreal velocidadMaxima;
    qreal aceleracion;
    qreal fuerzaSalto;
    qreal gravedad;

    bool enSuelo;
    bool saltando;
    bool atacando;
    bool mirandoDerecha;

    QVector<QPixmap> animIdle;
    QVector<QPixmap> animWalk;
    QVector<QPixmap> animJump;
    QVector<QPixmap> animAttack;
    QVector<QPixmap> animDeath;

    int frameActual;
    int contadorFrame;
    int framesPorAnimacion;

    int ataqueContador;

    enum EstadoAnimacion {
        PARADO = 0,
        CAMINANDO,
        SALTANDO,
        ATACANDO,
        MUERTO
    };

    EstadoAnimacion estadoActual;

    void cargarSpritesIndividuales();
    void aplicarGravedad();
    void actualizarAnimacion();
    void actualizarSprite();
    QPixmap obtenerSpriteActual();
};

#endif // GLADIADOR_H
