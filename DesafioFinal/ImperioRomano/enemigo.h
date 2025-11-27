#ifndef ENEMIGO_H
#define ENEMIGO_H
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QPixmap>
#include <QRectF>
#include <QVector>

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
    QRectF getRangoAtaque() const;

signals:
    void murio();
    void ataque();

private:
    bool esFuerte;
    int vida;
    int vidaMaxima;
    qreal velocidad;
    qreal rangoDeteccion;
    qreal rangoAtaque;

    bool atacando;
    int contadorAtaque;
    int cooldownAtaque;
    int cooldownAtaqueMax;
    bool yaGolpeo;

    int frameActual;
    int contadorFrame;
    int framesPorAnimacion;
    bool mirandoIzquierda;

    enum EstadoAnimacion {
        IDLE,
        CAMINANDO,
        ATACANDO,
        MUERTO
    };
    EstadoAnimacion estadoActual;

    QVector<QPixmap> animIdle;
    QVector<QPixmap> animWalk;
    QVector<QPixmap> animAttack;
    QVector<QPixmap> animDeath;

    QGraphicsRectItem *barraVidaFondo;
    QGraphicsRectItem *barraVida;

    void cargarSprites();
    QPixmap crearPlaceholder() const;
    void actualizarAnimacion();
    void actualizarSprite();
    QPixmap obtenerSpriteActual();
    void actualizarBarraVida();
};
#endif
