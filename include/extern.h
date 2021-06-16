/*  
 * extern.h
 *
 * All global variables / constants
 *
 */

#ifndef EXTERN_
#define EXTERN_

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern GlobalSDL * Global;

/* Declare Numeric Game Constants */
extern float BASKET_SPEED;
extern float FRUIT_SPEED;
extern float BULLET_TINY_SPEED;

/* Declare Global SDL Variables */
extern uint32_t score;

#ifdef __cplusplus
}
#endif

#endif
