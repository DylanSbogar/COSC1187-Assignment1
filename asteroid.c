#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "asteroid.h"

#if _WIN32
# include <windows.h>
#endif
#if __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
#endif

// Initialises the  asteroids
void init_asteroid(asteroid *asteroid, ship *ship, int w, int h)
{
    // Set the color of the asteroids.
    asteroid->outline_r = 1;
    asteroid->outline_g = 1;
    asteroid->outline_b = 1;

    // Set the angle of the asteroid.
    asteroid->angle = 90;

    // Set the rotation direction of the asteroid, either clockwise or anti-clockwise.
    asteroid->turn_val = rand() % (1 + 1 - -1) + -1;;

    // If the rand() returns 0 for the turn_val above, set it to -1.
    if(asteroid->turn_val == 0)
    {
        asteroid->turn_val = -1;
    }

    // Set the size of the asteroid.
    asteroid->size = rand() % (ASTEROID_MAX_SIZE + 1 - ASTEROID_MIN_SIZE) + ASTEROID_MIN_SIZE;

    // Set the velocity of the asteroid.
    asteroid->velocity = (rand() % (20 + 1 - 10) + 10);

    // Set the rotation velocity of the asteroid.
    asteroid->rotate_velocity = (rand() % (ASTEROID_ROTATE_MAX_SPEED + 1 - ASTEROID_ROTATE_MIN_SPEED) + ASTEROID_ROTATE_MIN_SPEED);

    // 2.1. Launch Position
    // Define a random position around the arena for an asteroid to spawn at.
    asteroid->starting_pos = (rand() % (360 - 1 + 1)) + 1;
    float theta = DEG_TO_RAD(asteroid->starting_pos);
    
    // Set the x and y co-ordinates of the asteroid.
    asteroid->pos.x = 
        (w * 1.1) * cos(theta)  + (w/2);
    asteroid->pos.y =
        (w * 1.1) * sin(theta) + (h/2);

    // Determine the angle of the asteroid, so its facing the ships current position.
    vector2d new_vec = direction_between_points(asteroid->pos, ship->pos);
    float length = pythagoras(new_vec.x,new_vec.y);

    // Normalise the direction vector of the asteroid.
    asteroid->dir.x = (new_vec.x / length);
    asteroid->dir.y = (new_vec.y / length);

    // Start asteroids as deactivated.
    asteroid->activated = false;

    // Generate an array of random points for the proceduraly generated asteroid shape.
    for(int i = 0; i < ASTEROID_POINTS; i++)
    {
        asteroid->points[i] = (rand() % (ASTEROID_MAX_SIZE + 1 - ASTEROID_MIN_SIZE) + ASTEROID_MIN_SIZE);
    }
}

void draw_asteroid(asteroid *asteroid)
{
    float x, y;
    float theta;

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glTranslatef(asteroid->pos.x, asteroid->pos.y, 0);
    // Rotate by the angle parameter, which has zero effect on the direction its moving.
    glRotatef(asteroid->angle, 0.0, 0.0, 1.0);

    // Apply the custom color.
    glColor3f(asteroid->outline_r, asteroid->outline_g, asteroid->outline_b);
    glBegin(GL_LINE_LOOP);

    // Draw the asteroid using the random points.
    for(int i = 0; i < ASTEROID_POINTS; i++)
    {
        theta = i / (float)ASTEROID_POINTS * 2.0 * M_PI;
        x = asteroid->points[i] * cosf(theta);
        y = asteroid->points[i] * sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();
}

void move_asteroid(asteroid *asteroid, float dt, int round)
{
    for(int i = 0; i < round; i++)
    {
        vector2d result;

        result.x = asteroid[i].dir.x * asteroid[i].velocity;
        result.y = asteroid[i].dir.y * asteroid[i].velocity;

        asteroid[i].pos.x = asteroid[i].pos.x + result.x;
        asteroid[i].pos.y = asteroid[i].pos.y + result.y;
    }
}

void rotate_asteroid(asteroid *asteroid, int turn_val, float dt)
{
    // Increment/Decrement the angle depending on whether the player is turning left/right.
    asteroid->angle += turn_val * asteroid->rotate_velocity;
}

bool ship_asteroid_collision(asteroid *asteroid, ship *ship)
{
    float length = 0;
    vector2d new_vec = direction_between_points(asteroid->pos, ship->pos);

    length = pythagoras(new_vec.x, new_vec.y);

    if(length <= asteroid->size + SHIP_COLLISION_RADIUS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int checkActivated(asteroid *asteroid, int w, int h, int num_detected)
{
    // If the asteroid is on-screen.
    if((asteroid->pos.x <= w && asteroid->pos.x >= 0) && (asteroid->pos.y <=h && asteroid->pos.y >= 0))
    {
        // Activate the asteroid, so we know to track its movements.
        asteroid->activated = true;
    }
    // If the asteroid is activated
    if(asteroid->activated)
    {
    // Check if the asteroid is to the right of left of the screen respectively.
    if(w <= asteroid->pos.x || 0 >= asteroid->pos.x)
    {
        asteroid->activated = false;
        num_detected--;
        return num_detected;
    }
    // Check if the asteroid is above or below the screen respectively.
    if(h <= asteroid->pos.y || 0 >= asteroid->pos.y)
    {
        asteroid->activated = false;
        num_detected--;
        return num_detected;
        }
    }

    return num_detected;
}
