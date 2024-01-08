#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <stdlib.h>

// Globals
const double    pi      = 2.0 * acos(0.0);
float           windowx = 800,
                windowy = 800,
                seed    = 1234.f;


/**
 * Hash function
 *
 * @param p - Input.
 * @return Seemingly unrelated output between 0 and 1.
 */
float hash11(float p)
{
    p = p * .1031 - floor(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return p - floor(p);
}


/**
 * Midpoint displacement function (taken from lecture notes).
 *
 * @param dataPoints - How many data points to return (has go be log2-able).
 * @param alpha - How "smooth" the outputs are.
 * @param beta - How high/low the tops and bottoms can be.
 *
 * @return An array of midpoint displacements.
 */
float* midpointdisp ( int dataPoints, float alpha, float beta ) {
    // Initiate array
    float* nums = new float[dataPoints];
    nums[0] = 0;
    nums[dataPoints] = 0;

    // Generate points
    for (int stepSize = dataPoints/2; stepSize > 0; stepSize /= 2) {
        for (int step = stepSize; step < dataPoints; step += stepSize*2) {
            float displacement = hash11(seed + step + stepSize) * beta;
            nums[step] = ( nums[step - stepSize] + nums[step + stepSize] ) / 2.f + displacement;
        }
        beta /= pow(2, alpha);
    }

    return nums;
}


/**
 * Draws triangles that fill the space between a position and a given list of points.
 *
 * @param points - The points that are drawn to.
 * @param position - The position which is draw out from.
 * @param window - The window which is drawn upon.
 * @param color - Color of the triangles.
 */
void drawFromPosition ( std::vector <sf::Vector2f> points, sf::Vector2f position, sf::RenderWindow& window, sf::Color color = sf::Color::White ) {
    std::vector <sf::Vertex> triangleVertex;

    for (int i=0; i<points.size(); i++) {
        sf::Vector2f    c0 = points[i],
                        c2 = position,
                        c1 = points[i+1];

        // Connect start and end
        if (i == points.size()-1)
                        c1 = points[0];

        triangleVertex.push_back( sf::Vertex ( c0, color ) );
        triangleVertex.push_back( sf::Vertex ( c1, color ) );
        triangleVertex.push_back( sf::Vertex ( c2, color ) );
    }

    window.draw( &triangleVertex[0], triangleVertex.size(), sf::Triangles );
}


/**
 * The main function.
 */
int main()
{
    // Initiate the main window, clock and controls
    sf::RenderWindow window (sf::VideoMode(windowx, windowy), "Noise");
    sf::Vector2f     centrepos (windowx/2.f, windowy/2.f);

    sf::Clock clock;
    clock.restart();

    // Game vars
    int     samples = pow(2, 8);     // Amount of samples taken

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

        // Time and mousepos
        float dt = clock.restart().asSeconds();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Clear screen
        window.clear();

        // Get midpoint displacements based on mouse position
        float scaleX = (windowx/2.f - mousePos.x) / windowx/2.f * 3.f;
        float scaleY = (windowy/2.f - mousePos.y) / windowy/2.f * 500.f;
        float* displacements = midpointdisp ( samples, scaleX, scaleY );

        // Take samples along circle
        std::vector <sf::Vector2f> samplePoints;
        for (int i=0; i<samples; i++) {
            float   ang             = (float)i/(float)samples * 2*pi,
                    displacement    = displacements[i],

                    radius          = 150.f + displacement,

                    xpos            = cos(ang) * radius + windowx/2.f,
                    ypos            = sin(ang) * radius + windowy/2.f;

            sf::Vector2f point ( xpos, ypos );
            samplePoints.push_back(point);
        }

        // Draw circle
        drawFromPosition(samplePoints, centrepos, window, sf::Color(255,100,100));

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
