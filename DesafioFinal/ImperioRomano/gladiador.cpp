#include "gladiador.h"
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

Gladiador::Gladiador(QObject *parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    vida(100),
    velocidadX(0),
    velocidadY(0),
    velocidadMaxima(5.0),
    aceleracion(0.8),
    fuerzaSalto(-15.0),
    gravedad(0.8),
    enSuelo(true),
    saltando(false),
    atacando(false),
    mirandoDerecha(true),
    frameActual(0),
    contadorFrame(0),
    framesPorAnimacion(8),
    estadoActual(PARADO)
{
    cargarSprites();
    setPos(100, 400);
}

void Gladiador::cargarSprites()
{
    // Cargar la hoja de sprites completa
    spriteSheet.load(":/images/jugador.png");

    if (spriteSheet.isNull()) {
        qDebug() << "Error: No se pudo cargar SpritesaUsar.png";
        // Crear placeholder
        spriteSheet = QPixmap(800, 600);
        spriteSheet.fill(Qt::red);
    }

    // Cargar coordenadas desde archivo de texto
    cargarCoordenadasDesdeArchivo("gladiador.txt");

    // Organizar sprites por tipo
    organizarSpritesPorTipo();

    // Establecer sprite inicial
    if (!spritesParado.isEmpty()) {
        setPixmap(obtenerSpriteActual());
    }
}

void Gladiador::cargarCoordenadasDesdeArchivo(const QString &rutaArchivo)
{
    QFile archivo(rutaArchivo);

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: No se pudo abrir" << rutaArchivo;
        return;
    }

    QTextStream in(&archivo);

    // Leer línea por línea
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();

        // Ignorar líneas vacías o comentarios
        if (linea.isEmpty() || linea.startsWith("#")) {
            continue;
        }

        // Dividir la línea por espacios
        QStringList partes = linea.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        // Verificar que tenga el formato correcto: id x y ancho alto tipo
        if (partes.size() >= 6) {
            SpriteInfo info;
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
    qDebug() << "Sprites cargados:" << sprites.size();
}

void Gladiador::organizarSpritesPorTipo()
{
    // Separar sprites por tipo de animación
    for (const SpriteInfo &sprite : sprites) {
        switch (sprite.tipo) {
        case 0:
            spritesParado.append(sprite);
            break;
        case 1:
            spritesCaminando.append(sprite);
            break;
        case 2:
            spritesSaltando.append(sprite);
            break;
        case 3:
            spritesAtacando.append(sprite);
            break;
        }
    }

    qDebug() << "Parado:" << spritesParado.size()
             << "Caminando:" << spritesCaminando.size()
             << "Saltando:" << spritesSaltando.size()
             << "Atacando:" << spritesAtacando.size();
}

QPixmap Gladiador::obtenerSpriteActual()
{
    QVector<SpriteInfo> *spritesActuales = nullptr;

    // Seleccionar el vector de sprites según el estado
    switch (estadoActual) {
    case PARADO:
        spritesActuales = &spritesParado;
        break;
    case CAMINANDO:
        spritesActuales = &spritesCaminando;
        break;
    case SALTANDO:
        spritesActuales = &spritesSaltando;
        break;
    case ATACANDO:
        spritesActuales = &spritesAtacando;
        break;
    }

    // Verificar que haya sprites disponibles
    if (!spritesActuales || spritesActuales->isEmpty()) {
        return QPixmap(50, 50); // Placeholder
    }

    // Asegurar que el frame actual esté dentro del rango
    if (frameActual >= spritesActuales->size()) {
        frameActual = 0;
    }

    // Obtener información del sprite actual
    const SpriteInfo &info = spritesActuales->at(frameActual);

    // Extraer el sprite de la hoja usando las coordenadas
    QPixmap sprite = spriteSheet.copy(info.x, info.y, info.ancho, info.alto);

    // Escalar para que tenga un tamaño consistente (opcional)
    sprite = sprite.scaled(80, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Voltear horizontalmente si mira a la izquierda
    if (!mirandoDerecha) {
        sprite = sprite.transformed(QTransform().scale(-1, 1));
    }

    return sprite;
}

void Gladiador::moverIzquierda()
{
    velocidadX = -velocidadMaxima;
    mirandoDerecha = false;
    if (enSuelo && !atacando) {
        estadoActual = CAMINANDO;
    }
}

void Gladiador::moverDerecha()
{
    velocidadX = velocidadMaxima;
    mirandoDerecha = true;
    if (enSuelo && !atacando) {
        estadoActual = CAMINANDO;
    }
}

void Gladiador::saltar()
{
    if (enSuelo && !saltando) {
        velocidadY = fuerzaSalto;
        enSuelo = false;
        saltando = true;
        estadoActual = SALTANDO;
        frameActual = 0;
    }
}

void Gladiador::atacar()
{
    if (!atacando) {
        atacando = true;
        estadoActual = ATACANDO;
        frameActual = 0;
        contadorFrame = 0;
    }
}

void Gladiador::detener()
{
    velocidadX = 0;
    if (enSuelo && !atacando) {
        estadoActual = PARADO;
    }
}

void Gladiador::aplicarGravedad()
{
    if (!enSuelo) {
        velocidadY += gravedad;

        if (velocidadY > 20) {
            velocidadY = 20;
        }
    }
}

void Gladiador::actualizar()
{
    // Aplicar física
    aplicarGravedad();

    // Mover el gladiador
    setPos(x() + velocidadX, y() + velocidadY);

    // Verificar límites de la escena
    if (scene()) {
        QRectF limites = scene()->sceneRect();

        if (x() < 0) {
            setPos(0, y());
        }

        if (x() + 80 > limites.width()) {
            setPos(limites.width() - 80, y());
        }

        // Suelo
        if (y() >= 480) {
            setPos(x(), 480);
            velocidadY = 0;
            enSuelo = true;
            saltando = false;

            if (velocidadX == 0 && !atacando) {
                estadoActual = PARADO;
            }
        }
    }

    // Actualizar animación
    actualizarAnimacion();

    // Actualizar sprite visual
    actualizarSprite();

    // Contador de ataque
    if (atacando) {
        contadorFrame++;
        // Terminar ataque cuando se completen todos los frames
        if (frameActual >= spritesAtacando.size() - 1 && contadorFrame >= framesPorAnimacion) {
            atacando = false;
            contadorFrame = 0;
            frameActual = 0;
            estadoActual = enSuelo ? PARADO : SALTANDO;
        }
    }
}

void Gladiador::actualizarAnimacion()
{
    contadorFrame++;

    // Cambiar de frame cuando el contador llegue al límite
    if (contadorFrame >= framesPorAnimacion) {
        contadorFrame = 0;
        frameActual++;

        // Determinar número máximo de frames según el estado
        int maxFrames = 1;

        switch (estadoActual) {
        case PARADO:
            maxFrames = spritesParado.size();
            break;
        case CAMINANDO:
            maxFrames = spritesCaminando.size();
            break;
        case SALTANDO:
            maxFrames = spritesSaltando.size();
            break;
        case ATACANDO:
            maxFrames = spritesAtacando.size();
            break;
        }

        // Reiniciar animación (excepto ataque, que se maneja aparte)
        if (frameActual >= maxFrames) {
            if (estadoActual != ATACANDO) {
                frameActual = 0;
            }
        }
    }
}

void Gladiador::actualizarSprite()
{
    setPixmap(obtenerSpriteActual());
}

void Gladiador::setVida(int nuevaVida)
{
    vida = nuevaVida;

    if (vida < 0) vida = 0;
    if (vida > 100) vida = 100;

    emit vidaCambiada(vida);

    if (vida <= 0) {
        emit murio();
    }
}

void Gladiador::recibirDanio(int danio)
{
    setVida(vida - danio);
}

QRectF Gladiador::getBoundingBox() const
{
    return QRectF(x(), y(), 80, 120);
}
