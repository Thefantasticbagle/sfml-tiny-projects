#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>


// Globals, functions & classes
const double    pi              = 2.0 * acos(0.0);  //!<  Pi.
const int       windowx         = 900,              //!<  Screen width.
                windowy         = 900;              //!<  Screen height.
const float     seed            = 1284.5f,          //!<  Seed.
                gravity         = 98.1f,            //!<  Gravity.
                windVelocity    = -1000.f,          //!<  Windspeed.
                fluidConstant   = 0.1f;             //!<  Fluid constant.


// Set up psuedo-random number generator
std::random_device  rnddev;
std::mt19937        gen(rnddev());
std::uniform_real_distribution <float> dist(0, 1);


/**
 * Lerps between two floats.
 *
 * @param f0 - Start value.
 * @param f1 - End value.
 * @param t  - Unclamped time.
 */
float lerp (float f0, float f1, float t) {
    if      (t < 0) t = 0;
    else if (t > 1) t = 1;
    return f0 + (f1 - f0) * t;
}


/**
 * A class for circles that bounce around and absorb eachother.
 */
class Circle {
public:
    sf::Vector2f    pos,        //!<  Position of the Circle
                    force,      //!<  Current forces affecting the Circle
                    vel;        //!<  Velocity of the Circle
    float           mass,       //!<  Radius of the Circle
                    rad;
    sf::Color       color;

    /**
     * Default constructor.
     */
    Circle () {};

    /**
     * Constructor.
     */
    Circle (sf::Vector2f position, sf::Vector2f velocity, float radius, sf::Color col) {
        pos         = position;
        vel         = velocity;
        force       = sf::Vector2f (0,0);
        rad         = radius;
        mass        = pi * pow(radius, 2);
        color       = col;
    }

    /**
     * Updates the disk.
     *
     * @param dt - How much time passed since last update.
     */
    void update (float dt) {

        // Gravity
        force.y += gravity * mass;

        // Wind
        force.x += fluidConstant * (windVelocity - vel.x) * 2.f*rad;

        // Apply forces and update position
        vel     += dt * force / mass;
        force   = sf::Vector2f(0,0);
        pos     += vel * dt;

        // Collision with walls
        if (pos.y > windowy - rad) {
            vel.y *= -0.9f;
            pos.y = windowy - rad;
        }
    }

    void draw (sf::RenderWindow& window) {
        sf::CircleShape     circle;
        circle.setRadius    ( rad );
        circle.setFillColor ( color );
        circle.setOrigin    ( rad, rad );
        circle.setPosition  ( pos );
        window.draw(circle);
    }

};


/**
 * The main program.
 */
int main()
{
    // Create clock & window
    sf::RenderWindow window(sf::VideoMode(windowx, windowy), "Windy balls");

    sf::Clock clock;
    clock.restart();
    float time = 0.f;

    // Make circles
    std::vector <Circle*> circles;

    for (int i=0; i<100; i++) {
        sf::Vector2f    pos     ( lerp( windowx*0.75, windowx*1.25f, dist(gen) ),
                                  lerp( 0, -windowy*0.5f, dist(gen) )),

                        vel     ( lerp(-15.f,15.f,dist(gen)) , lerp(-15.f,15.f,dist(gen)) );

        float           radius = lerp(5,15,dist(gen));
        sf::Color       color  = sf::Color(255,255-255*radius/15.f,0);

        Circle* circle = new Circle ( pos, vel, radius, color );
        circles.push_back( circle );
    }

    // Gameloop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear window
        window.clear();

        // Timers
        float dt        = clock.restart().asSeconds();
        time            += dt;

        // Update & draw disks
        for (int i=0; i<circles.size(); i++) {
            Circle* circle = circles[i];

            circle->update(dt);
            circle->draw(window);
        }

        // Display window
        window.display();
    }

    return 0;
}
