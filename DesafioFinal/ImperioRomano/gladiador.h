#ifndef GLADIADOR_H
#define GLADIADOR_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVector>
#include <QRect>

// Estructura para almacenar información de cada sprite
struct SpriteInfo {
    int id;           // ID del sprite
    int x;            // Posición X en la hoja
    int y;            // Posición Y en la hoja
    int ancho;        // Ancho del sprite
    int alto;         // Alto del sprite
    int tipo;         // Tipo de animación (0=parado, 1=caminar, 2=saltar, 3=atacar)
};

class Gladiador : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Gladiador(QObject *parent = nullptr);

    // Métodos de movimiento
    void moverIzquierda();
    void moverDerecha();
    void saltar();
    void atacar();
    void detener();

    // Actualización
    void actualizar();

    // Getters
    int getVida() const { return vida; }
    bool estaAtacando() const { return atacando; }
    bool estaEnSuelo() const { return enSuelo; }
    QRectF getBoundingBox() const;

    // Setters
    void setVida(int nuevaVida);
    void recibirDanio(int danio);

signals:
    void vidaCambiada(int vida);
    void murio();

private:
    // Atributos de física
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

    // Sistema de sprites con coordenadas
    QPixmap spriteSheet;                    // La imagen grande
    QVector<SpriteInfo> sprites;            // Vector con todos los sprites
    QVector<SpriteInfo> spritesParado;      // Sprites filtrados por tipo
    QVector<SpriteInfo> spritesCaminando;
    QVector<SpriteInfo> spritesSaltando;
    QVector<SpriteInfo> spritesAtacando;

    // Control de animación
    int frameActual;
    int contadorFrame;
    int framesPorAnimacion;

    // Estados de animación
    enum EstadoAnimacion {
        PARADO = 0,
        CAMINANDO = 1,
        SALTANDO = 2,
        ATACANDO = 3
    };

    EstadoAnimacion estadoActual;

    // Métodos privados
    void cargarSprites();
    void cargarCoordenadasDesdeArchivo(const QString &rutaArchivo);
    void organizarSpritesPorTipo();
    void aplicarGravedad();
    void actualizarSprite();
    void actualizarAnimacion();
    QPixmap obtenerSpriteActual();
};

#endif // GLADIADOR_H

