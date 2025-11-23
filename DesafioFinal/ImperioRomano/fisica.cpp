#include "fisica.h"

namespace Fisica {

qreal aplicarGravedad(qreal velocidadActual)
{
    velocidadActual += GRAVEDAD;
    if (velocidadActual > MAX_CAIDA) velocidadActual = MAX_CAIDA;
    return velocidadActual;
}

qreal acelerarCaida(qreal velocidadActual)
{
    velocidadActual += GRAVEDAD * 1.2;
    const qreal limite = MAX_CAIDA + 6.0;
    if (velocidadActual > limite) velocidadActual = limite;
    return velocidadActual;
}

void calcularTiroParabolico(qreal &vx, qreal &vy)
{
    vy += GRAVEDAD;
    if (vy > MAX_CAIDA) vy = MAX_CAIDA;
}

}
