#ifndef FISICA_H
#define FISICA_H

#include <QtGlobal>

namespace Fisica {

constexpr qreal SPRITE_SCALE = 1.0;
constexpr qreal VELOCIDAD_HORIZONTAL = 5.0;
constexpr qreal ACELERACION = 0.8;
constexpr qreal FUERZA_SALTO = -18.0;
constexpr qreal GRAVEDAD = 1.0;
constexpr qreal MAX_CAIDA = 20.0;
constexpr int SUELO_Y = 820;
constexpr int FRAMES_POR_ANIMACION = 8;

qreal aplicarGravedad(qreal velocidadActual);
qreal acelerarCaida(qreal velocidadActual);
void calcularTiroParabolico(qreal &vx, qreal &vy);

}

#endif // FISICA_H
