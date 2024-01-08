#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>


// Globals, functions & classes
const double    pi = 2.0 * acos(0.0);   //!<  Pi.


/**
 * Gets the area of a circle with the given radius.
 *
 * @param radius - The radius of the circle.
 * @return The area.
 */
float getArea(float radius) {
    return pi * pow(radius, 2);
}

/**
 * Gets the radius of a circle with a given area.
 *
 * @param area - The area of the circle.
 * @return The radius.
 */
float getRad(float area) {
    return sqrt(area / pi);
}


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
     * sf::Vector2f copy constructor.
     */
    Vec2 (sf::Vector2f vec) {
        x = vec.x;
        y = vec.y;
    }

    /**
     * sf::Vector2i copy constructor.
     */
    Vec2 (sf::Vector2i vec) {
        x = (float) vec.x;
        y = (float) vec.y;
    }

    /**
     * Cout
     */
    friend std::ostream& operator<< (std::ostream& os, const Vec2 vec) {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
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
     * Projection.
     *
     * @param vec - The vector that is projected onto this.
     * @return A new Vec2 that is 'vec' projected onto this.
     */
    Vec2 proj (Vec2 vec) {
        return (*this) * ((*this)*vec / pow((*this).len(), 2));
    }

    /**
     * Reflection.
     *
     * @param vec - A vector.
     * @return A new Vec2 that is 'vec' reflected about this.
     */
    Vec2 refl (Vec2 vec) {
        return (*this).proj(vec) * 2 - vec;
    }

    /**
     * Orthagonal.
     *
     * @return A new Vec2 which is orthagonal to this.
     */
    Vec2 orth () {
        return Vec2 (-y, x);
    }

    /**
     * Rotation.
     *
     * @param angle - Angle to rotate with.
     * @return A new Vec2 which is this rotated 'angle' around origo.
     */
    Vec2 rot (float angle) {
        return Vec2 ( cos(angle) * x - sin(angle) * y,
                      sin(angle) * x + cos(angle) * y );
    }
};


/**
 * A class for simple lines.
 */
class Line {
public:
    Vec2    origo;  //!<  Origo
    float   ang;    //!<  Angle

    /**
     * Default constructor.
     */
    Line () {};

    /**
     * Point-angle constructor.
     */
    Line (Vec2 origo, float angle) {
        this->origo = origo;
        ang = angle;
    }

    /**
     * Gets direction of line as a Vec2.
     */
    Vec2 getDir () {
        return Vec2 (cos(ang), sin(ang));
    }

    /**
     * Localizes a point so that it's relative to this line.
     *
     * @param vec - The vector to localize.
     * @return A new Vec2 object that is vec, but localized so that it's relative to this line.
     */
    Vec2 localize (Vec2 point) {
        return (point - origo).rot(-ang);
    }

    /**
     * Globalizes a point so that it's relative to the screen.
     */
    Vec2 globalize (Vec2 point) {
        return point.rot(ang) + origo;
    }

    /**
     * Draws the line.
     *
     * @param window - Pointer to the window which to draw the line onto.
     */
    void draw (sf::RenderWindow* window) {
        sf::Vertex line[2];
        Vec2 dir = getDir();
        Vec2 p0 = origo - dir * 9999.f;
        Vec2 p1 = origo + dir * 9999.f;
        line[0].position = sf::Vector2f(p0.x, p0.y);
        line[0].color  = sf::Color::White;
        line[1].position = sf::Vector2f(p1.x, p1.y);
        line[1].color = sf::Color::White;
        window->draw(line, 2, sf::Lines);

        sf::CircleShape           origoMarker;
        origoMarker.setRadius    (5);
        origoMarker.setFillColor (sf::Color::White);
        origoMarker.setOrigin    (5, 5);
        origoMarker.setPosition  (origo.x, origo.y);
        window->draw             (origoMarker);

        Vec2 p3 = origo + dir * 100.f;
        sf::CircleShape               directionMarker;
        directionMarker.setRadius    (3);
        directionMarker.setFillColor (sf::Color::White);
        directionMarker.setOrigin    (3, 3);
        directionMarker.setPosition  (p3.x, p3.y);
        window->draw                 (directionMarker);
    }
};


/**
 * A class for circles that bounce around and absorb eachother.
 */
class Circle {
public:
    Vec2        pos,              //!<  Position of the Circle
                vel;              //!<  Velocity of the Circle
    float       rad;              //!<  Radius of the Circle
    sf::Color   color;            //!<  Color of the circle
    bool        alive,            //!<  Whether or not the circle is 'alive'
                collidesWithLines; //!<  Whether or not the circle collides with lines

    /**
     * Default constructor.
     */
    Circle () {};

    /**
     * Constructor.
     */
    Circle (Vec2 position, Vec2 velocity, float radius) {
        pos = position;
        vel = velocity;
        rad = radius;
        color = sf::Color::Yellow;
        alive = true;
        collidesWithLines = true;
    }

    /**
     * SFML Copy constructor.
     */
    Circle (sf::CircleShape circle) {
        pos = circle.getPosition();
        vel = Vec2 (0, 0);
        rad = circle.getRadius();
        color = circle.getFillColor();
        alive = true;
    }

    /**
     * Updates the disk.
     *
     * @param dt - How much time passed since last update.
     * @param lines - A vector of Line-pointers that this disk may collide with.
     * @param circles - A vector of Circle-pointers that this may collide with.
     * @param resX - Width of the screen.
     * @param resY - Height of the screen.
     */
    void update (float dt, std::vector <Line*> lines, std::vector <Circle*> circles, float resX, float resY) {
        pos = pos + vel * dt;

        // Collision with lines
        if (collidesWithLines) {
            for (int i=0; i<lines.size(); i++) {
                Line*   line = lines[i];
                Vec2    localPos = line->localize(pos);

                // Check if disk is touching the line (and reflect it)
                if ( localPos.y < rad ) {
                    vel = ( line->getDir() ).refl(vel);
                    localPos.y = rad;
                    pos = line->globalize(localPos);
                }
            }
        }

        // Collision with walls
        if (pos.x < rad) {
            vel.x *= -1.f;
            pos.x = rad;
        } else if (pos.x > resX - rad) {
            vel.x *= -1.f;
            pos.x = resX - rad;
        }
        if (pos.y < rad) {
            vel.y *= -1.f;
            pos.y = rad;
        } else if (pos.y > resY - rad) {
            vel.y *= -1.f;
            pos.y = resY - rad;
        }

        // Collision with circles
        for (int i=0; i<circles.size(); i++) {

            // Check if [i] is alive and not this circle
            if ( circles[i]->alive && circles[i] != this ) {
                Circle* circle = circles[i];
                float   distance = pos.dist( circle->pos );

                // If this circle can eat [i]
                if ( rad > circle->rad && distance < rad + circle->rad) {
                    float eatAmount = getArea(circle->rad) * dt + 5.f;

                    // Devour entire circle if its radius is less than 2
                    if ( circle->rad <= 5.f ) {
                        eatAmount       = getArea( circle->rad );
                        circle->alive   = false;
                    }

                    // Transfer mass
                    rad         = getRad(getArea(rad) + eatAmount);
                    circle->rad = getRad(getArea(circle->rad) - eatAmount);
                }
            }
        }
    }

    /**
     * Draws the disk.
     *
     * @param window - Pointer to the window which to draw the disk onto.
     */
    void draw (sf::RenderWindow* window) {
        sf::CircleShape     circle;
        circle.setRadius    (rad);
        circle.setFillColor (color);
        circle.setOrigin    (rad, rad);
        circle.setPosition  (pos.x, pos.y);
        window->draw        (circle);
    }
};






/**
 * The main program.
 */
int main()
{
    // Create window
    int resX = 900; //!<  Screen width
    int resY = 900; //!<  Screen height
    sf::RenderWindow window(sf::VideoMode(resX, resY), "Agario");

    // Set up psuedo-random number generator
    std::random_device  rnddev;
    std::mt19937        gen(rnddev());
    std::uniform_real_distribution <float> dist(0, 1);

    // Make line(s)
    Line line1 (Vec2(0, resY/4), pi/4.f-pi/2.f);
    std::vector <Line*> lines {&line1};

    // Make player disk
    std::vector <Circle*> circles;

    Vec2  playerSpawnPos        = Vec2 (resX/16, resY/16);  // Player spawnpos
    float playerRadius          = 20.f;                     // Player (spawn)size
    float playerMoveSpeed       = 75.f;                     // How much velocity the player gains by 'jumping'
    float playerMoveCooldown    = 0.1f;                     // How many seconds the player has to wait between each jump
    float playerMoveTimer       = 0;

    Circle* player              = new Circle (playerSpawnPos, Vec2(0,0), playerRadius);
    player->color               = sf::Color::Red;
    player->collidesWithLines   = false;
    circles.push_back(player);

    // Make other disks
    float   enemyMoveSpeed        = 50.f,   // How fast enemies move (this remains constant)
            enemyMinSize          = 5.f,    // Minimum enemy (spawn)size
            enemyMaxSize          = 15.f;   // Maximum enemy (spawn)size
    int     enemyCount            = 100;    // How many enemies there are

    for (int i=1; i<enemyCount+1; i++) {
        // Shoot disk out at a random angle between 135 and 225 degrees
        float angle = dist(gen) * pi * 2.f;
        Circle* circle = new Circle (Vec2 ( dist(gen)*resX , dist(gen)*resY ),
                                     Vec2 ( cos(angle)*enemyMoveSpeed , sin(angle)*enemyMoveSpeed ),
                                     dist(gen) * (enemyMaxSize - enemyMinSize) + enemyMinSize );
        circles.push_back(circle);
    }

    // Set up clock
    sf::Clock clock;
    clock.restart();

    float time = 0.f;
    float restartGameTimer = 0.f;

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
        playerMoveTimer -= dt;

        // Player movement
        if ( player->alive && playerMoveTimer <= 0 && sf::Mouse::isButtonPressed( sf::Mouse::Button::Left ) ) {
            playerMoveTimer = playerMoveCooldown;

            // Boost player away from mouse
            Vec2 moveDir = ( player->pos - Vec2(sf::Mouse::getPosition(window)) ).norm();
            player->vel += moveDir * playerMoveSpeed;

            // Spawn blob and decrease mass if player is big enough
            if ( player->rad > 2.f ) {
                float blobArea  = getArea (player->rad) / 10.f;
                float blobRad   = getRad (blobArea);
                Circle* blob    = new Circle (player->pos - moveDir * (blobRad + player->rad + 1), moveDir * -playerMoveSpeed, blobRad);
                circles.push_back(blob);

                player->rad = getRad( getArea( player->rad ) - blobArea );
            }
        }

        // Player deceleration
        player->vel -= player->vel * dt / 4.f;

        // Update and draw
        float totalArea = 0.f;
        for (int i=0; i<circles.size(); i++) {
            if ( circles[i]->alive ) {
                totalArea += getArea( circles[i]->rad );
                circles[i]->update(dt, lines, circles, resX, resY);
                circles[i]->draw(&window);
            }
        }

        for (int i=0; i<lines.size(); i++) {
            lines[i]->draw(&window);
        }

        // Win/Loss
        if ( getArea ( player->rad ) / totalArea >= 0.8f || !player->alive || time >= 120.f) {
            time = 0.f;
            restartGameTimer += dt;
        }

        if (restartGameTimer > 0.f) {
            restartGameTimer += dt;

            // Suck up all circles into one
            int livingCircles = 0;
            int winnerIndex = -1;
            for (int i=0; i<circles.size(); i++) {
                Circle* circle = circles[i];

                if (circle->alive) {
                    livingCircles +=1;
                    winnerIndex = i;
                    circle->vel += (Vec2(resX/2, resY/2) - circle->pos).norm() * restartGameTimer * 0.3f;
                }
            }

            // Restart game when only one circle is left
            if (livingCircles == 1) {
                restartGameTimer = 0;
                Circle* winner = circles[winnerIndex];

                // Delete old circles (except winner)
                for (int i=0; i<circles.size(); i++) {
                    if ( i != winnerIndex ) {
                        delete circles[i];
                    }
                }

                // Clear old circle-vector
                circles.clear();
                circles.push_back(winner);
                winner->vel = winner->vel.norm() * enemyMoveSpeed;

                // Make new circles
                winner->rad = enemyMaxSize;
                for (int i=0; i<enemyCount; i++) {
                    Vec2    newCirclePos (resX/2 + resX/3 * (dist(gen) - 0.5f),
                                          resY/2 + resY/3 * (dist(gen) - 0.5f));
                    Vec2    newCircleDir (newCirclePos - Vec2 (resX/2, resY/2));
                    Circle* newCircle = new Circle (newCirclePos,
                                                    newCircleDir.norm() * enemyMoveSpeed,
                                                    dist(gen) * (enemyMaxSize - enemyMinSize) + enemyMinSize );
                    circles.push_back(newCircle);
                }


                // Make one of the circles the 'player' if the player is dead
                if (player == nullptr || !player->alive) {
                    player = circles[0];
                }

                // ...And set all of the player's properties back
                player->pos = playerSpawnPos;
                player->vel = Vec2(0,0);
                player->rad = playerRadius;
                player->color = sf::Color(255,0,0);
                player->collidesWithLines = false;
            }
        }

        // Display window
        window.display();
    }

    return 0;
}
