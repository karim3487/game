/*  
 * game.h
 *
 * Game logic and orginization functions go here
 *
 */

#ifndef GAME_
#define GAME_

#ifdef __cplusplus
extern "C"
{
#endif

void endSDL();

/*  
 * Update all object collisions
 *
 * @param basket      Ship object to check collisions
 * @param bullets   Bullets object to check collisions
 * @param fruits Asteroids object to check collisions
 *
 */
void updateObjectCollision(Object ** basket, Object ** bullets, Object ** fruits);

/*  
 * Update an objects subimage to the next one in the chain
 *
 * @param obj   The object to update
 *
 */
void updateObjectAnimation(Object * obj);

/*  
 * General function to update all user-based actions
 *
 * @param basket      The basket object of the user
 * @param bullets   The bullets object chain
 * @param image     The user's spritesheet for creating bullets
 * @param timer     The global timer for checking bullet creation speed
 *
 * @return the bullet chain updated
 *
 */
Object * updateUserActions(Object * basket, Object * bullets, SDL_Texture * image, uint32_t * timer);

/*  
 * Update the user's basket based on keyboard input
 *
 * @param basket  The user basket object
 *
 */
void updateUserBasketMovement(Object * basket);

/*  
 * Update all the bullet objects based on user actions
 *
 * @param basket      The user basket for where to spawn new bullets
 * @param bullets   The bullet chain of objects for which to check
 * @param image     The spritesheet that contains the bullet sprites
 * @param timer     The global timer for checking bullet creation speed
 *
 * @return the bullet chain updated
 *
 */
Object * updateUserBullets(Object * basket, Object * bullets, SDL_Texture * image, uint32_t * timer);

/*  
 * Update the fruits object chain
 *
 * @param fruits     The fruits object chain
 * @param image         The spritesheet that contains the fruits
 *
 * @return the fruits object chain once it's been updated
 *
 */
Object * updateAsteroids(Object * fruits, SDL_Texture * image);

/*  
 * Game setup function, setup game settings and constants and initialize SDL2
 *
 */
void startSDL();

#ifdef __cplusplus
}
#endif

#endif
