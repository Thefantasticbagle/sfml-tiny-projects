#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <stdlib.h>

const double pi = 2.0 * acos(0.0);
float   windowx = 800,
        windowy = 800;

std::random_device  rnddev;
std::mt19937        gen(rnddev( ));
std::uniform_real_distribution <float> dist(0, 1);


float* generateHalfWayPoints2 ( float startValue, float endValue, int generations, float alpha, float beta ) {
    int startIndex  = 0,
        endIndex    = pow(2, generations);

    float* ret = new float[endIndex];
    ret[startIndex] = startValue;
    ret[endIndex]   = endValue;

    for (int i=1; i<generations+1; i++){
        int     stepSize    = pow(2, generations - i);
        int     stepAmount  = pow(2, i);
        beta *= pow(2, -alpha);

        for (int j=0; j<stepAmount; j+=2) {
            float displacement = (dist(gen)-0.5f)*2.f * beta;
            ret[(j+1)*stepSize] = ( ret[j*stepSize] + ret[(j+2)*stepSize] ) / 2.f + displacement;
        }
    }

    return ret;
}

std::vector <sf::Vector2f> heightmapToVector ( float* heightmap, int heightmapSize ) {
    std::vector <sf::Vector2f> ret;
    for (int i=0; i<heightmapSize; i++) {
        sf::Vector2f vec ( windowx / (float)heightmapSize * (float)i , heightmap[i] );
        //float ang = 2.f*pi / (float)heightmapSize*(float)i;
        //sf::Vector2f vec ( cos(ang) * (heightmap[i]) + windowx/2.f, sin(ang) * (heightmap[i]) + windowy/2.f );

        ret.push_back(vec);
    }
    return ret;
}

void drawLinesBetweenPoints ( std::vector <sf::Vector2f> points, sf::RenderWindow& window, sf::Color color ) {
    std::vector <sf::Vertex> lineVertex;

    for (int i=0; i<points.size(); i++) lineVertex.push_back( sf::Vertex ( points[i], color ) );
    window.draw( &lineVertex[0], lineVertex.size(), sf::Lines );

    for (int i=0; i<lineVertex.size()-1; i++) lineVertex[i] = lineVertex[i+1];
    window.draw( &lineVertex[0], lineVertex.size()-1, sf::Lines );
}

void drawMountains ( std::vector <sf::Vector2f> points, sf::RenderWindow& window, sf::Color color, float startY ) {
    std::vector <sf::Vertex> triangleVertex;

    for (int i=0; i<points.size()-1; i++) {
        // First triangle
        sf::Vector2f    c0 = points[i],
                        c1 ( c0.x, startY ),
                        c2 = points[i+1];

        triangleVertex.push_back( sf::Vertex ( c0, color ) );
        triangleVertex.push_back( sf::Vertex ( c1, color ) );
        triangleVertex.push_back( sf::Vertex ( c2, color ) );

        // Second triangle
        sf::Vector2f    c3 ( c2.x, startY );

        triangleVertex.push_back( sf::Vertex ( c1, color ) );
        triangleVertex.push_back( sf::Vertex ( c2, color ) );
        triangleVertex.push_back( sf::Vertex ( c3, color ) );
    }

    window.draw( &triangleVertex[0], triangleVertex.size(), sf::Triangles );
}

float hash11(float p)
{
    p = p * .1031 - floor(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return p - floor(p);
}

float noise11_octave(float p, float scale, float seed) {
  p *= scale;
  float     pint = floor(p),
            pfract = p - pint,
            res = (1.0f - pfract) * hash11(pint + seed) + pfract * hash11(pint + 1.0f + seed);
  return (res - 0.5f)*2.0f;
}

float lerp ( float f0, float f1, float t ) {
    return (1.f-t)*f0 + t*f1;
}

sf::Vector2f lerp ( sf::Vector2f p0, sf::Vector2f p1, float t ) {
    return p0*(1.f-t) + p1*t;
}

float sstep3 ( float t) {
    return t*t *(3 - 2*t);
}

void drawCircle ( sf::Vector2f position, float radius, sf::RenderWindow& window, sf::Color color = sf::Color::White ) {
    sf::CircleShape      circle;
    circle.setPosition  (position);
    circle.setRadius    (radius);
    circle.setOrigin    (radius, radius);
    circle.setFillColor (color);
    window.draw         (circle);
}


int main()
{
    // Initiate the main window, clock and controls
    sf::RenderWindow window (sf::VideoMode(windowx, windowy), "Noise");

    sf::Clock clock;
    clock.restart();

    int     mouseHeldLeft   = 0,
            mouseHeldRight  = 0;

    // Game vars
    std::vector <sf::Vector2f> points1;         // Blue line
    std::vector <sf::Vector2f> permapoints;     // Red mountains
    std::vector <sf::Vector2f> midline { sf::Vector2f (0, windowy/2.f), sf::Vector2f (windowx, windowy/2.f) };

    int     samples = 1000;     // Amount of samples taken
    float   minp    = 0.f,      // Minimum p-value for noise function
            maxp    = 1.f,      // Maximum p-value for noise function
            seed    = 13242.3f; // Seed for noise function

    // Make 'snakes'
    std::vector <int>   snakeCurrIndex;
    std::vector <float> snakeCurrTime;

    for (int i=0; i<100; i++) {
        snakeCurrIndex.push_back ( i );
        snakeCurrTime.push_back( 0.f );
    }

	// Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Time and controls
        float dt = clock.restart().asSeconds();

        if (sf::Mouse::isButtonPressed( sf::Mouse::Button::Left )) mouseHeldLeft++;
        else mouseHeldLeft = 0;

        if (sf::Mouse::isButtonPressed( sf::Mouse::Button::Right )) mouseHeldRight++;
        else mouseHeldRight = 0;

        // Clear screen
        window.clear();

        // Set scale of noise function
        float scaleX = sf::Mouse::getPosition(window).x / 5.f;
        float scaleY = windowy/2.f - sf::Mouse::getPosition(window).y;

        // Get samples from noise function
        points1.clear();
        for (int i=0; i<samples; i++) {
            float curp       = (maxp-minp)*(float)i/(float)samples + minp;
            sf::Vector2f point ( windowx/(float)samples*(float)i, noise11_octave( curp, scaleX, seed ) * scaleY + windowy/2.f );
            points1.push_back(point);
        }

        // Add / average out terrain by left/rightclicking
        if (mouseHeldLeft == 1 || mouseHeldRight == 1) {
            if (permapoints.size() == 0) {
                permapoints = points1;

            } else {
                // Add (leftclick)
                if (mouseHeldLeft == 1) {
                    for (int i=0; i<samples; i++)
                        permapoints[i].y += points1[i].y - windowy/2.f;

                // Average out (rightclick)
                } else {
                    for (int i=0; i<samples; i++)
                        permapoints[i].y = (permapoints[i].y + points1[i].y ) / 2.f;
                }
            }
        }


        // Update and draw stuff
        if (permapoints.size() > 0) {
            // Terrain / red mountains
            drawMountains(permapoints, window, sf::Color(255,100,100), windowy);

            // Snakes
            for (int i=0; i<snakeCurrIndex.size(); i++) {
                // Advance time and loop back if end is reached
                snakeCurrTime[i] += samples/10.f * dt;
                if (snakeCurrTime[i] >= 1.f) {
                    snakeCurrTime[i]  = 0.f;
                    snakeCurrIndex[i] += 1;
                    if (snakeCurrIndex[i] >= samples-1)   snakeCurrIndex[i] = 0; // Loop to other side
                }

                // Interpolate position
                int             currIndex   = snakeCurrIndex[i];
                float           t           = sstep3( snakeCurrTime[i] );
                sf::Vector2f    smooth      = lerp ( permapoints[currIndex],   permapoints[currIndex + 1], t );

                // Move the snake off the track (except for the indicator dot)
                if (i != snakeCurrIndex.size()-1) {
                    smooth.x = windowx/(float)samples*(float)i + windowx/2.f - windowx/samples*100/2.f ;
                    smooth.y -= windowy/4.f;
                }

                // Update and draw
                sf::Vector2f pos = smooth;
                drawCircle( pos, 4.f/(float)snakeCurrIndex.size()*(float)i+1.f, window );
            }
        }

        // Draw blue line and midline
        drawLinesBetweenPoints(points1, window, sf::Color(100,100,255));
        drawLinesBetweenPoints(midline, window, sf::Color::White);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
