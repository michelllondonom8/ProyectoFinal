#ifndef ENEMIGO_H
#define ENEMIGO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVector>

// Estructura para información de sprites
struct SpriteInfoEnemigo {
    int id;
    int x;
    int y;
    int ancho;
    int alto;
    int tipo;    // 0=idle, 1=caminar, 2=atacar
};

class Enemigo : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Enemigo(bool fuerte, QObject *parent = nullptr);

    // Métodos principales
    void actualizar(const QPointF &posJugador);
    void recibirDanio(int danio);

    // Getters
    int getVida() const { return vida; }
    int getVidaMaxima() const { return vidaMaxima; }
    bool estaVivo() const { return vida > 0; }
    bool estaAtacando() const { return atacando; }
    bool esFuerteEnemigo() const { return esFuerte; }
    QRectF getBoundingBox() const;

signals:
    void murio();

private:
    // Atributos básicos
    bool esFuerte;
    int vida;
    int vidaMaxima;
    qreal velocidad;
    qreal rangoDeteccion;
    qreal rangoAtaque;
    bool atacando;
    int contadorAtaque;

    // Sistema de sprites
    QPixmap spriteSheet;
    QVector<SpriteInfoEnemigo> sprites;
    QVector<SpriteInfoEnemigo> spritesIdle;
    QVector<SpriteInfoEnemigo> spritesCaminando;
    QVector<SpriteInfoEnemigo> spritesAtacando;

    // Control de animación
    int frameActual;
    int contadorFrame;
    int framesPorAnimacion;
    bool mirandoIzquierda;

    // Estados
    enum EstadoAnimacion {
        IDLE = 0,
        CAMINANDO = 1,
        ATACANDO = 2
    };

    EstadoAnimacion estadoActual;

    // Métodos privados
    void cargarSprites();
    void cargarCoordenadasDesdeArchivo(const QString &rutaArchivo);
    void organizarSpritesPorTipo();
    void actualizarComportamiento(const QPointF &posJugador);
    void actualizarAnimacion();
    void actualizarSprite();
    QPixmap obtenerSpriteActual();
};

#endif // ENEMIGO_H

#endif // ENEMIGO_H
