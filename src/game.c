/* Language Includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Standard System Include */
#include <unistd.h>
#include <time.h>

#ifndef F_OK
#define F_OK 0
#endif

/* SDL Library */
#include <SDL.h>

/* Local Includes */
#include "types.h"
#include "wrappers.h"
#include "object.h"
#include "game.h"
#include "global.h"
#include "extern.h"

void endSDL()
{

    SDL_free(Global->basePath);
    SDL_DestroyRenderer(Global->renderer);
    SDL_DestroyWindow(Global->window);

    free(Global);
    SDL_Quit();
}

void startSDL()
{
    /* Global game object */
    Global = calloc(1, sizeof(GlobalSDL));

    if(Global == NULL)
    {
        fprintf(stderr, "[%s][%s: %d]Fatal Error: Memory c-allocation error\n", getDate(), __FILE__, __LINE__);
        exit(0);
    }

    Global->basePath = SDL_GetBasePath();

    if(Global->basePath == NULL)
    {
        fprintf(stderr, "[%s][%s: %d]Fatal Error: Could not get SDL Basepath String, error: %s\n", getDate(), __FILE__, __LINE__, SDL_GetError());
        exit(0);
    }

    if(freopen(getAbsolutePath(ERROR_FILE), "a", stderr) == NULL)
    {
       fprintf(stderr, "[%s][%s: %d]Warning: Freopen could not pipe stream\n", getDate(), __FILE__, __LINE__);
    }

    /* Initialize Random Seed */
    srand(time(NULL));

    Global->window = NULL;
    Global->renderer = NULL;

    /* Initialize Window */
    if(!initSDL("Harvesting"))
    {
        exit(0);
    }

    setWindowIcon(getAbsolutePath(IMG_DIR "icon.bmp"));

    /* Constant Logic / initialize */
    Global->screenWidth = 1920; //getNativeWidth();
    Global->screenHeight = 1080; //getNativeHeight();
    Global->screenTop = 64;
    Global->screenBottom = 5;
    Global->screenLeft = 5;
    Global->screenRight = 5;
    /* Make this an option */
    Global->framesPerSecond = 60.0;
    Global->gameTickRatio = (60.0 / Global->framesPerSecond);

    /* Initialize Keystates */
    Global->keystates = SDL_GetKeyboardState(NULL);

    Global->exit = 0;
    Global->state = DEFAULT;

    BASKET_SPEED = (7.5 * Global->gameTickRatio);
    FRUIT_SPEED = (1.5 * Global->gameTickRatio);
    BULLET_TINY_SPEED = (9.0 * Global->gameTickRatio);

    /* Global Score */
    score = 0;
}

void updateObjectCollision(Object ** basket, Object ** fruits)
{
    Object * fruitsRoot = *fruits;

    while(fruitsRoot != NULL)
    {
        if(objectCollision(fruitsRoot, *basket))
        {
            //FIXME: here, if fruit made a collision with basket then it makes live -- in fact we need to make score ++
            (*basket)->lives--;
            fruitsRoot->lives = 0;
        }

        fruitsRoot = fruitsRoot->next;
    }
}

Object * updateFruits(Object * fruits, SDL_Texture * image)
{
    Object * fruitsRoot;
    Object * fruit;
    Object * previousFruit;
    uint8_t random;
    uint8_t root = 1;

    FRUIT_SPEED *= (FRUIT_SPEED < BASKET_SPEED * 2) ? 1.00025 : 1;

    if((rand() % (int) (30 / ((FRUIT_SPEED < 30) ? FRUIT_SPEED : 30))) == 0)
    {
        random = rand() % 6;

        if(random >= 3)
        {
            fruit = createObject(image, 0, 1, ASTEROID_SMALL, 1, 0, 32, 32, 32, 1.0);
        }
        else if(random >= 1)
        {
            fruit = createObject(image, 0, 1, ASTEROID_MEDIUM, 3, 32, 32, 64, 64, 1.0);
        }
        else
        {
            fruit = createObject(image, 0, 1, ASTEROID_LARGE, 6, 96, 32, 96, 96, 1.0);
        }

        fruit->x = (int) ((rand() % (Global->screenWidth)) - (fruit->clip.w / 2));
        fruit->y = -(fruit->clip.h);

        fruit->next = fruits;
        fruits = fruit;
    }

    fruitsRoot = fruits;

    while(fruits != NULL)
    {
        if(fruits->y > (Global->screenHeight + fruits->clip.h) || fruits->lives <= 0)
        {
            fruit = fruits;
            fruits = fruits->next;

            if(root)
            {
                fruitsRoot = fruits;
            }
            else
            {
                previousFruit->next = fruits;
            }

            fruit->next = NULL;
            freeObjects(fruit);
            continue;
        }

        previousFruit = fruits;

        moveObject(fruits, 0, FRUIT_SPEED);
        fruits = fruits->next;

        if(root)
        {
            root = 0;
        }
    }

    return fruitsRoot;
}
//
//Object * updateUserBullets(Object * basket, Object * bullets, SDL_Texture * image, uint32_t * timer)
//{
//    Object * bulletsRoot;
//    Object * bullet;
//    Object * previousBullet;
//    uint8_t root = 1;
//
//    if(timer[BULLET_TINY_TIMER] < SDL_GetTicks())
//    {
//        if(Global->keystates[SDL_SCANCODE_1] || Global->keystates[SDL_SCANCODE_SPACE])
//        {
//            bullet = createObject(image, 0, 2, BULLET_TINY, 1, 0, 144, 16, 16, 1.0);
//
//            bullet->x = (basket->x + (bullet->clip.w / 2));
//            bullet->y = (basket->y - (bullet->clip.w / 2));
//
//            bullet->next = bullets;
//            bullets = bullet;
//
//            timer[BULLET_TINY_TIMER] = (SDL_GetTicks() + 150);
//        }
//    }
//
//    bulletsRoot = bullets;
//
//    while(bullets != NULL)
//    {
//        if(bullets->y <= Global->screenTop || bullets->lives <= 0)
//        {
//            bullet = bullets;
//            bullets = bullets->next;
//
//            if(root)
//            {
//                bulletsRoot = bullets;
//            }
//            else
//            {
//                previousBullet->next = bullets;
//            }
//
//            bullet->next = NULL;
//            freeObjects(bullet);
//            continue;
//        }
//
//        previousBullet = bullets;
//
//        moveObject(bullets, 0, (-1 * BULLET_TINY_SPEED));
//        bullets = bullets->next;
//
//        if(root)
//        {
//            root = 0;
//        }
//    }
//
//    return bulletsRoot;
//}

Object * updateUserBasketMovement(Object * basket)
{
    int8_t basketX = 0;
    //we don't wat basket to be moving by y
    int8_t basketY = 0;

    int temp = 0;

    /* User Keyboard  */
    if(Global->keystates[SDL_SCANCODE_LEFT] || Global->keystates[SDL_SCANCODE_A])
    {
        basketX--;
    }

    if(Global->keystates[SDL_SCANCODE_RIGHT] || Global->keystates[SDL_SCANCODE_D])
    {
        basketX++;
    }

    /* Updating basket Animation */
    if(basketX == 0)
    {
        basket->subImage = 0;
    }    
    else if(basketX == -1)
    {
        basket->subImage = 1;
    }
    else if(basketX == 1)
    {
        basket->subImage = 2;
    }

    basketX *= BASKET_SPEED;

    /* Setting Basket Boundaries */
    if((basket->x + basketX) < Global->screenLeft)
    {
        temp = (Global->screenLeft - basket->x);
        basketX = temp > 0 ? temp : 0;
    }

    if((basket->x + basket->clip.w + basketX) > (Global->screenWidth - Global->screenRight))
    {
       temp = ((Global->screenWidth - Global->screenRight) - basket->x - basket->clip.w);
        basketX = temp > 0 ? temp : 0;
    }

    moveObject(basket, basketX, basketY);

    return basket;
}

Object * updateUserActions(Object * basket, SDL_Texture * image, uint32_t * timer)
{
    return updateUserBasketMovement(basket);
//    return updateUserBullets(basket, bullets, image, timer);
}

void updateObjectAnimation(Object * obj)
{
    obj->subImage = ((obj->subImage + 1) >= obj->subImageNumber) ? 0 : (obj->subImage + 1);
}
