#include "enemigo.h"
#include <QtMath>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

Enemigo::Enemigo(bool fuerte, QObject *parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    esFuerte(fuerte),
    velocidad(2.0),
    rangoDeteccion(300.0),
    rangoAtaque(100.0),
    atacando(false),
    contadorAtaque(0),
    frameActual(0),
    contadorFrame(0),
    framesPorAnimacion(10),
    mirandoIzquierda(true),
    estadoActual(IDLE)
{
    // Configurar según tipo
    if (esFuerte) {
        vidaMaxima = 200;
        vida = 200;
        velocidad = 1.5;
    } else {
        vidaMaxima = 100;
        vida = 100;
        velocidad = 2.0;
    }

    cargarSprites();
}

void Enemigo::cargarSprites()
{
    // Cargar la hoja de sprites
    spriteSheet.load(":/Imagenes/enemigo1.png");

    if (spriteSheet.isNull()) {
        qDebug() << "Error: No se pudo cargar SpritesaUsar.png para enemigo";
        spriteSheet = QPixmap(800, 600);
        spriteSheet.fill(esFuerte ? Qt::darkRed : Qt::darkBlue);
    }

    // Cargar coordenadas desde archivo
    // Para enemigos normales usa coordenadas_enemigo.txt
    // Para enemigos fuertes usa coordenadas_enemigo_fuerte.txt
    QString archivo = esFuerte ? "enemigoInteligente.txt"
                               : "enemigo.txt";

    cargarCoordenadasDesdeArchivo(archivo);
    organizarSpritesPorTipo();

    // Establecer sprite inicial
    if (!spritesIdle.isEmpty()) {
        setPixmap(obtenerSpriteActual());
    }
}

void Enemigo::cargarCoordenadasDesdeArchivo(const QString &rutaArchivo)
{
    QFile archivo(rutaArchivo);

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: No se pudo abrir" << rutaArchivo;
        return;
    }

    QTextStream in(&archivo);

    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();

        if (linea.isEmpty() || linea.startsWith("#")) {
            continue;
        }

        QStringList partes = linea.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        if (partes.size() >= 6) {
            SpriteInfoEnemigo info;
            info.id = partes[0].toInt();
            info.x = partes[1].toInt();
            info.y = partes[2].toInt();
            info.ancho = partes[3].toInt();
            info.alto = partes[4].toInt();
            info.tipo = partes[5].toInt();

            sprites.append(info);
        }
    }

    archivo.close();
    qDebug() << "Sprites enemigo cargados:" << sprites.size();
}

void Enemigo::organizarSpritesPorTipo()
{
    for (const SpriteInfoEnemigo &sprite : sprites) {
        switch (sprite.tipo) {
        case 0:
            spritesIdle.append(sprite);
            break;
        case 1:
            spritesCaminando.append(sprite);
            break;
        case 2:
            spritesAtacando.append(sprite);
            break;
        }
    }

    qDebug() << "Enemigo - Idle:" << spritesIdle.size()
             << "Caminando:" << spritesCaminando.size()
             << "Atacando:" << spritesAtacando.size();
}

QPixmap Enemigo::obtenerSpriteActual()
{
    QVector<SpriteInfoEnemigo> *spritesActuales = nullptr;

    switch (estadoActual) {
    case IDLE:
        spritesActuales = &spritesIdle;
        break;
    case CAMINANDO:
        spritesActuales = &spritesCaminando;
        break;
    case ATACANDO:
        spritesActuales = &spritesAtacando;
        break;
    }

    if (!spritesActuales || spritesActuales->isEmpty()) {
        return QPixmap(80, 120);
    }

    if (frameActual >= spritesActuales->size()) {
        frameActual = 0;
    }

    const SpriteInfoEnemigo &info = spritesActuales->at(frameActual);

    // Extraer sprite usando coordenadas
    QPixmap sprite = spriteSheet.copy(info.x, info.y, info.ancho, info.alto);

    // Escalar según si es fuerte o no
    int anchoFinal = esFuerte ? 100 : 80;
    int altoFinal = esFuerte ? 140 : 120;
    sprite = sprite.scaled(anchoFinal, altoFinal, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Voltear si mira a la izquierda
    if (mirandoIzquierda) {
        sprite = sprite.transformed(QTransform().scale(-1, 1));
    }

    return sprite;
}

void Enemigo::actualizar(const QPointF &posJugador)
{
    if (!estaVivo()) return;

    actualizarComportamiento(posJugador);
    actualizarAnimacion();
    actualizarSprite();

    if (atacando) {
        contadorAtaque++;
        if (contadorAtaque > 30) {
            atacando = false;
            contadorAtaque = 0;
            estadoActual = IDLE;
            frameActual = 0;
        }
    }
}

void Enemigo::actualizarComportamiento(const QPointF &posJugador)
{
    qreal dx = posJugador.x() - x();
    qreal dy = posJugador.y() - y();
    qreal distancia = qSqrt(dx * dx + dy * dy);

    mirandoIzquierda = (dx < 0);

    if (distancia < rangoAtaque) {
        if (!atacando) {
            atacando = true;
            contadorAtaque = 0;
            estadoActual = ATACANDO;
            frameActual = 0;
        }
    }
    else if (distancia < rangoDeteccion) {
        if (!atacando) {
            estadoActual = CAMINANDO;
        }

        if (dx > 0) {
            setPos(x() + velocidad, y());
        } else {
            setPos(x() - velocidad, y());
        }
    }
    else {
        if (!atacando) {
            estadoActual = IDLE;
        }
    }
}

void Enemigo::actualizarAnimacion()
{
    contadorFrame++;

    if (contadorFrame >= framesPorAnimacion) {
        contadorFrame = 0;
        frameActual++;

        int maxFrames = 1;

        switch (estadoActual) {
        case IDLE:
            maxFrames = spritesIdle.size();
            break;
        case CAMINANDO:
            maxFrames = spritesCaminando.size();
            break;
        case ATACANDO:
            maxFrames = spritesAtacando.size();
            break;
        }

        if (frameActual >= maxFrames) {
            frameActual = 0;
        }
    }
}

void Enemigo::actualizarSprite()
{
    setPixmap(obtenerSpriteActual());
}

void Enemigo::recibirDanio(int danio)
{
    vida -= danio;
    if (vida < 0) vida = 0;

    if (vida <= 0) {
        emit murio();
    }
}

QRectF Enemigo::getBoundingBox() const
{
    int ancho = esFuerte ? 100 : 80;
    int alto = esFuerte ? 140 : 120;
    return QRectF(x(), y(), ancho, alto);
}
