#include <SFML/Graphics.hpp>
#include <random>
#include <stdlib.h>


// Globals, functions & classes
const double    pi = 2.0 * acos(0.0);       //!<  Pi.
const int       sampleAmount = 200;         //!<  Amount of samples taken by each spline.
const float     deltaDifference = 0.01f;   //!<  How little the part of a curve we look at is when we're doing derivation.


// Set up psuedo-random number generator
std::random_device  rnddev;
std::mt19937        gen(rnddev());
std::uniform_real_distribution <float> dist(0, 1);


/**
 * A class for simple 2D vectors.
 */
class Vec2 {
public:
    float   x,  //!<  x-position (width)
            y;  //!<  y-position (height)

    /**
     * Standard constructor.
     */
    Vec2 () {};

    /**
     * Constructor.
     */
    Vec2 (float xpos, float ypos) {
        x = xpos;
        y = ypos;
    }

    /**
     * sf::Vector2i copy constructor.
     */
    Vec2 (sf::Vector2i vec) {
        x = (float) vec.x;
        y = (float) vec.y;
    }

    /**
     * Addition by another Vec2.
     *
     * @param vec - The vector to add.
     * @return This vector, added with 'vec'.
     */
    Vec2 operator+= (Vec2 vec) {
        x += vec.x;
        y += vec.y;
        return (*this);
    }
    Vec2 operator+ (Vec2 vec) {
        Vec2 ret;
        ret.x = x + vec.x;
        ret.y = y + vec.y;
        return ret;
    }

    /**
     * Subtraction by another Vec2.
     *
     * @param vec - The vector to subtract.
     * @return This vector, subtracted by 'vec'.
     */
    Vec2 operator-= (Vec2 vec) {
        x -= vec.x;
        y -= vec.y;
        return (*this);
    }
    Vec2 operator- (Vec2 vec) {
        Vec2 ret;
        ret.x = x - vec.x;
        ret.y = y - vec.y;
        return ret;
    }

    /**
     * Division by number.
     *
     * @param num - The number to divide by.
     * @return This vector, divided by 'num'.
     */
    Vec2 operator/= (float num) {
        x /= num;
        y /= num;
        return (*this);
    }
    Vec2 operator/ (float n) {
        Vec2 ret;
        ret.x = x / n;
        ret.y = y / n;
        return ret;
    }

    /**
     * Multiplication by number.
     *
     * @param num - The number to multiplicate by.
     * @return This vector, multiplied by 'num'.
     */
    Vec2 operator*= (float num) {
        x *= num;
        y *= num;
        return (*this);
    }
    Vec2 operator* (float n) {
        Vec2 ret;
        ret.x = x * n;
        ret.y = y * n;
        return ret;
    }
    friend Vec2 operator* (float n, Vec2 u) {
        return u * n;
    }

    /**
     * Dot product.
     *
     * @param vec - The vector to dot with.
     * @return The dot product between this and 'vec'.
     */
    float operator* (Vec2 vec) {
        return x*vec.x + y*vec.y;
    }

    /**
     * Length.
     *
     * @return The length of this vector.
     */
    float len () {
        return sqrt(pow(x,2) + pow(y,2));
    }

    /**
     * Distance.
     *
     * @param vec - The other vector.
     * @return Distance between this vector and the other.
     */
    float dist(Vec2 vec) {
        return ((*this) - vec).len();
    }

    /**
     * Normalization.
     *
     * @return A new Vec2 that is this, normalized.
     */
    Vec2 norm () {
        return (*this) / (*this).len();
    }

    /**
     * Orthagonal.
     *
     * @return A new Vec2 which is orthagonal to this.
     */
    Vec2 orth () {
        return Vec2 (-y, x);
    }
};

/**
 * Creates a sf::Vector2f out of a Vec2
 *
 * @param vec - The Vec2.
 * @return The Vec2 as a sf::Vector2f
 */
sf::Vector2f vec2ToSF (Vec2 vec) {
    return sf::Vector2f (vec.x, vec.y);
}


/**
 * A simple struct for samples taken from spline()
 */
struct Sample {
    Vec2    pos;
    float   t;
};


/**
 * Bounces from f0 to f1 and then to f0 again between t=0 and t=1
 * @param f0 - Start value.
 * @param f1 - End value.
 * @param t  - Unclamped time.
 */
float linBounce (float f0, float f1, float t) {
    return abs(floor(t) - t + 0.5) * (f0 - f1) * 2 + f1;
}

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
 * Lerps between two vectors.
 *
 * @param p0 - Start position.
 * @param p1 - End position.
 * @param t  - Unclamped time.
 */
Vec2 lerp (Vec2 p0, Vec2 p1, float t) {
    if      (t < 0) t = 0;
    else if (t > 1) t = 1;
    return p0 + (p1 - p0) * t;
}

/**
 * Lerps between two colors.
 *
 * @param c0 - Start color.
 * @param c1 - End color.
 * @param t  - Unclamped time.
 */
sf::Color lerp (sf::Color c0, sf::Color c1, float t) {
    if      (t < 0) t = 0;
    else if (t > 1) t = 1;

    sf::Color c (lerp(c0.r, c1.r, t),
                 lerp(c0.g, c1.g, t),
                 lerp(c0.b, c1.b, t));

    return c;
}

/**
 * Smooth steps between two vectors.
 *
 * @param p0 - Start position.
 * @param p1 - End position.
 * @param t  - Unclamped time.
 */
Vec2 sstep3 (Vec2 p0, Vec2 p1, float t) {
    return lerp (p0, p1, t*t*(3 - 2*t) );
}

/**
 * Some weird function I found by experimenting in geogebra
 *
 * @param p0 - Start position.
 * @param p1 - End position.
 * @param t  - Unclamped time.
 */
Vec2 weird (Vec2 p0, Vec2 p1, float t) {
    return lerp (p0, p1, 6*pow(t,3) - 5*pow(t,5) );
}

/**
 * Splines with a given list of control-points.
 * note: the curve doesn't necessarely touch any other points than the first and last.
 *
 * @param points     - Control points.
 * @param t          - Unclamped time.
 * @param f          - Interpolation function.
 * @param loopAround - How many times to loop around (1->end connects with start. 2+ -> spiral patterns)
 *
 * @return The position of a point on the spline curve at the given time.
 */
Vec2 spline (std::vector <Vec2> points, float t, Vec2 (*f)(Vec2, Vec2, float) = lerp, int loopAround = 0 ) {
    std::vector <Vec2> derivatives = points;
    std::vector <Vec2> nextDerivatives;

    // Interpolate every 'derivative' except the last with the next until there's only one left
    while (derivatives.size() > 1) {

        for (int i=0; i<derivatives.size(); i++) {
            Vec2 derivative      = derivatives[i];
            Vec2 nextDerivative  = derivatives[i+1];

            if ( i != derivatives.size() - 1) {
                nextDerivatives.push_back( f (derivative, nextDerivative, t) );

            } else if ( loopAround > 0 ) {
                loopAround--;
                nextDerivative = derivatives[0];
                nextDerivatives.push_back( f (derivative, nextDerivative, t) );
            }
        }

        derivatives = nextDerivatives;
        nextDerivatives.clear();
    }

    return derivatives[0];
}


/**
 * A class for a river which can sprout particles.
 */
class River {
public:
    float       width,              //!<  Width of the river.
                flowSpeed;          //!<  How fast the water in the river flows
    sf::Color   flowColor,          //!<  Color which the water pulsates/flows with
                waterColor,         //!<  The color of the water
                particleColor,      //!<  Initial color of the water-particles
                particleColorFade;  //!<  End color of the water-particles

    /**
     * Sets the River's sample points and removes the old.
     */
    void setControlPoints (std::vector <Vec2> points) {
        controlPoints = points;

        // Clear old sample points and make new
        for (int i=0; i<samplePoints.size(); i++) delete samplePoints[i];
        samplePoints.clear();

        for (int i=0; i<sampleAmount; i++) {
            float t = 1.f / (float)sampleAmount * (float)i;

            Sample* samplePoint = new Sample;
            samplePoint->pos = spline (controlPoints, t, lerp);
            samplePoint->t   = t;

            samplePoints.push_back (samplePoint);
        }
    }

    /**
     * Gets the River's control points.
     *
     * @return The River's control points.
     */
    std::vector <Vec2>      getControlPoints()  { return controlPoints; }

    /**
     * Gets the River's sample points.
     *
     * @return The River's sample points.
     */
    std::vector <Sample*>   getSamplePoints()   { return samplePoints;  }

    /**
     * Gets the nearest sample point.
     *
     * @param pos - The position.
     * @return The sample point nearest to the position.
     */
    Sample* getNearestSample (Vec2 pos) {
        Sample* nearestSample = nullptr;
        float   nearestSampleDist = -1.f;

        for (int i=0; i<samplePoints.size(); i++) {
            Sample* sample      = samplePoints[i];
            float   sampleDist  = sample->pos.dist(pos);

            if (nearestSample == nullptr || sampleDist < nearestSampleDist) {
                nearestSample       = sample;
                nearestSampleDist   = sampleDist;
            }
        }

        return nearestSample;
    }

    /**
     * Gets the nearest control point's index
     *
     * @param t - The time-coordinate.
     * @return The index of the closest control point to the time-coordinate.
     */
    int getNearestControlPoint ( float t ) {
        Vec2    samplePoint = spline ( controlPoints, t, lerp );
        float   index       = -1,
                dist        = -1;

        for (int i=0; i<controlPoints.size(); i++) {
            Vec2  controlPoint  = getNearestSample ( controlPoints[i] )->pos;
            float tdist         = controlPoint.dist( samplePoint );

            if ( index == -1 || tdist < dist ) {
                index = i;
                dist = tdist;
            }
        }

        return index;
    }

    /**
     * Gets the (approximate) velocity at a given time-coordinate.
     *
     * @param t - The time.
     * @return The approximate velocity at the time.
     */
    Vec2 getVelocity (float t) {
        float   t0 = t - deltaDifference / 2.f,
                t1 = t + deltaDifference / 2.f;

        Vec2    p0 = spline ( controlPoints, t0, lerp ),
                p1 = spline ( controlPoints, t1, lerp );

        return  ( p1 - p0 ) / deltaDifference;
    }

    /**
     * Creates particles that sprout out at the given time-coordinate.
     *
     * @param t - The time.
     * @param pos - Position the particle is spawned on.
     */
    void makeParticles (float t, Vec2 pos) {
        Vec2 vel = getVelocity(t).norm() * (dist(gen)+0.5)*2.f + Vec2( (dist(gen)-0.5f), (dist(gen)-0.5f) );
        Vec2 p0  = pos;
        Vec2 p1  = p0 + vel * flowSpeed;

        particlesP0.push_back   (p0);
        particlesP1.push_back   (p1);
        particlesT.push_back    (0);
    }

    /**
     * Draws the river.
     *
     * @param dt - How much time has passed since the last call.
     * @param window - The window to draw upon.
     */
    void draw (float dt, sf::RenderWindow& window) {
        riverAge += dt;

        // Make two triangles that together look like a trapezoid connecting this sample with the next
        std::vector <sf::Vertex> triangleVertex;

        for (int i=0; i<samplePoints.size()-1; i++) {
            Sample      samplePoint0 = *samplePoints[i];
            float       t0     = samplePoint0.t;
            Vec2        pos0   = samplePoint0.pos,
                        vel0   = getVelocity(t0);
            sf::Color   color0 = lerp ( waterColor, flowColor, linBounce(0, 1, t0 - riverAge) );

            Sample      samplePoint1 = *samplePoints[i+1];
            float       t1     = samplePoint1.t;
            Vec2        pos1   = samplePoint1.pos,
                        vel1   = getVelocity(t1);
            sf::Color   color1 = lerp ( waterColor, flowColor, linBounce(0, 1, t1 - riverAge) );

            // First triangle
            sf::Vertex  corner0 ( vec2ToSF( pos0 + vel0.norm().orth() * width ), color0 ),
                        corner1 ( vec2ToSF( pos0 - vel0.norm().orth() * width ), color0 ),
                        corner2 ( vec2ToSF( pos1 - vel1.norm().orth() * width ), color1 );

            triangleVertex.push_back(corner0);
            triangleVertex.push_back(corner1);
            triangleVertex.push_back(corner2);

            // Second triangle
            sf::Vertex  corner3 ( vec2ToSF( pos1 + vel1.norm().orth() * width ), color1 ),
                        corner4 ( vec2ToSF( pos1 - vel1.norm().orth() * width ), color1 ),
                        corner5 ( vec2ToSF( pos0 + vel0.norm().orth() * width ), color0 );

            triangleVertex.push_back(corner3);
            triangleVertex.push_back(corner4);
            triangleVertex.push_back(corner5);
        }

        // Iterate through particles and add their triangles to the vertex
        for (int i=0; i<particlesT.size(); i++) {
            particlesT[i] = particlesT[i] + dt;

            // Remove particle if it's too old
            if ( particlesT[i] > 1.f ) {
                particlesP0.erase( particlesP0.begin() + i );
                particlesP1.erase( particlesP1.begin() + i );
                particlesT.erase ( particlesT.begin()  + i );
                continue;
            }

            // Make the triangle and add it to vector
            Vec2        particlePos = lerp ( particlesP0[i], particlesP1[i], particlesT[i] );
            sf::Color   particleCol = lerp ( particleColor, particleColorFade, particlesT[i] );
            float       c           = cos(pi/4.f),
                        s           = sin(pi/4.f);

            sf::Vertex  corner0 ( vec2ToSF(particlePos + Vec2 ( 0     , s*-5.f )) , particleCol ),
                        corner1 ( vec2ToSF(particlePos + Vec2 ( c*5.f , s*5.f  )) , particleCol ),
                        corner2 ( vec2ToSF(particlePos + Vec2 ( c*-5.f, s*5.f  )) , particleCol );

            triangleVertex.push_back( corner0 );
            triangleVertex.push_back( corner1 );
            triangleVertex.push_back( corner2 );
        }

        window.draw(&triangleVertex[0], triangleVertex.size(), sf::Triangles);
    }

private:
    std::vector <Vec2>      controlPoints,          //!<  Control points
                            particlesP0,            //!<  P0 (start position) of particles
                            particlesP1;            //!<  P1 (end position) of particles
    std::vector <float>     particlesT;             //!<  t (time between 0 and 1) of particles
    std::vector <Sample*>   samplePoints;           //!<  Sample points
    float                   riverAge;               //!<  How 'old' the river is (how much it has been updated)
};


/**
 * The main program.
 */
int main()
{
    // Create window
    int resX = 900; //!<  Screen width
    int resY = 900; //!<  Screen height
    sf::RenderWindow window(sf::VideoMode(resX, resY), "River editor");

    // Set up clock
    sf::Clock clock;
    clock.restart();

    float time = 0.f;

    // Create river
    River river;

    river.width             = 25.f;
    river.flowSpeed         = 150.f;
    river.flowColor         = sf::Color (255,150,255);
    river.waterColor        = sf::Color (50,150,255);
    river.particleColor     = sf::Color (255,150,255);
    river.particleColorFade = sf::Color (200,255,240);

    river.setControlPoints({ Vec2(100,100),
                             Vec2(100, resY*0.8f-100),
                             Vec2(resX*0.8f-100,100),
                             Vec2(resX-100, resY-100) });

    // Curve editing
    bool canCreatePoint = true;
    int definingPoint   = -1;

    // Gameloop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Timers
        float dt        = clock.restart().asSeconds();
        time            += dt;

        // Clear window
        window.clear(sf::Color::White);

        // Modify curve
        Vec2                mousePos        = Vec2 (sf::Mouse::getPosition(window));
        Sample*             nearestSample   = river.getNearestSample(mousePos);
        std::vector <Vec2>  controlPoints   = river.getControlPoints();

        if ( sf::Mouse::isButtonPressed ( sf::Mouse::Button::Left ) ) {

            // Move newly created control point until mousebutton isn't held
            if ( !canCreatePoint && definingPoint != -1 ) {
                canCreatePoint = false;

                // Remove control point if rightclick is pressed
                if ( sf::Mouse::isButtonPressed ( sf::Mouse::Button::Right )) {
                    controlPoints.erase( controlPoints.begin() + definingPoint );
                    river.setControlPoints(controlPoints);
                    definingPoint = -1;

                } else {
                    controlPoints[definingPoint] = mousePos;
                    river.setControlPoints(controlPoints);
                }

            } else {
                // Start editing control points if they are aimed at and clicked
                for (int i=0; i<controlPoints.size(); i++) {
                    if ( mousePos.dist(controlPoints[i]) < 10 ) {
                        definingPoint = i;
                        canCreatePoint = false;
                        break;
                    }
                }

                // Insert a new controlpoint between the two closest controlpoints if there's no points to move
                if (canCreatePoint && mousePos.dist(nearestSample->pos) < river.width ) {
                    canCreatePoint = false;
                    int nearestControlPoint = river.getNearestControlPoint ( nearestSample->t ) ;
                    definingPoint = nearestControlPoint + 1;

                    controlPoints.insert ( controlPoints.begin() + definingPoint , mousePos );
                    river.setControlPoints (controlPoints);
                }
            }

        // If curve is not being modified, draw particles when mouse touches the river
        } else {
            definingPoint = -1;
            canCreatePoint = true;

            if ( nearestSample->pos.dist(mousePos) < river.width * 1.5f ) {
                river.makeParticles(nearestSample->t, mousePos);
            }
        }

        // Draw control points
        for (int i=0; i<controlPoints.size(); i++) {
            Vec2 pos = controlPoints[i];
            sf::CircleShape     circle;
            circle.setRadius    (10);
            circle.setFillColor (sf::Color::Red);
            circle.setOrigin    (10, 10);
            circle.setPosition  (pos.x, pos.y);
            window.draw         (circle);
        }

        // Draw river
        river.draw(dt, window);

        // Display window
        window.display();
    }

    return 0;
}
