#include <SFML/Graphics.hpp> // Included this for the graphics 
#include <SFML/Audio.hpp> // included this for the sound effects and music
#include <iostream>  // Include this header for std::cerr
#include <vector>
#include <algorithm>

// Initialization of global variables so they can be accessed throughout the game, such as coinCount, which needs to be displayed across all the UIs
sf::Font font;
sf::Text gameOverText;
sf::Text victoryGameText;
class Enemy;
int coinCount = 0;



// Method to render the health bar, with a parameters of the window, position, label, health and maxHealth so the healthbar percentage can be calculated and displayed
void renderHealthBar(sf::RenderWindow& window, const sf::Vector2f& position, const std::string& label, int health, int maxHealth) {
    // Label for the health bar, such as for enemy, player
    sf::Text healthLabel;
    healthLabel.setFont(font);
    healthLabel.setString(label);
    healthLabel.setCharacterSize(24);
    healthLabel.setFillColor(sf::Color::White);
    healthLabel.setPosition(position.x, position.y);

    // Health bar background (black bar)
    sf::RectangleShape healthBarBackground(sf::Vector2f(200.f, 20.f));
    healthBarBackground.setPosition(position.x, position.y + 30.f);  // Slightly below the label
    healthBarBackground.setFillColor(sf::Color::Black);

    // Health bar (green bar)
    sf::RectangleShape healthBar(sf::Vector2f(200.f * (static_cast<float>(health) / maxHealth), 20.f));
    healthBar.setPosition(position.x, position.y + 30.f);
    healthBar.setFillColor(sf::Color::Green);



    // Draw the label, background, and health bar
    window.draw(healthLabel);
    window.draw(healthBarBackground);
    window.draw(healthBar);
}


class Button { // Defining the button class to be used for the main menu
public:
    Button(const sf::Vector2f& position, const std::string& text, sf::Font& font) { // Constructor for the button with parameters, first one is a vector called position, 2nd is a string storing the text to be used inside the button and 3rd is the font to be used
        button.setSize(sf::Vector2f(800.f, 100.f));  // Setting the same size for each button instantiated using this class
        button.setPosition(position); // Set the position equal to position parameter
        button.setFillColor(sf::Color::Blue);  // Setting the buttons background colour to blue

        // Set the font and font size
        label.setFont(font);
        label.setString(text);
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::White); // Set the text colour to white

        // Center the text inside the button
        sf::FloatRect textRect = label.getLocalBounds();
        label.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        label.setPosition(position.x + button.getSize().x / 2.0f, position.y + button.getSize().y / 2.0f);
    }

    // method so the button can be render alongside the label
    void render(sf::RenderWindow& window) {
        window.draw(button);
        window.draw(label);
    }

    // Boolean variable storing if the button has been clicked with a parameter of the window
    bool isClicked(const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) && sf::Mouse::isButtonPressed(sf::Mouse::Left); // This line of code is checking if the button has been clicked by the LMB
    }

private:
    sf::RectangleShape button;
    sf::Text label;
};

// Bullet class for the players bullets, storing variables shape, velocity, integer variable damage and a float variable speed
class Bullet {


public:

    sf::RectangleShape shape;
    sf::Vector2f velocity;
    int damage;
    float speed;

    Bullet(float x, float y, float speed, sf::Color color, int damage) // Constructor for the bullet class so we can instantiate it with all the relevant variables of the bullet to be used throughout the game
        : speed(speed), damage(damage) {
        shape.setSize(sf::Vector2f(20.f, 5.f));  // Bullet size
        shape.setFillColor(color); // sets the bullets colour to the defined color in the constructor
        shape.setPosition(x, y); // Sets the position of the bullet
    }

    // Method to update the bullet and move it towards the right
    void update() {
        shape.move(speed, 0.f);  // Shoot bullets to the right of the screen
    }

    // Boolean variable to track if the player's bullets have exited the screen
    bool isOutOfBounds(int windowWidth) const {
        return shape.getPosition().x > windowWidth;  // Bullet is off the screen (right side)
    }

    // Render method so we can draw each bullet
    void render(sf::RenderWindow& window) {
        window.draw(shape);  // Draw the bullet on the screen
    }

    // Boolean variable to track the collision between the bullet and enemies
    bool checkCollision(const sf::RectangleShape& targetShape) const {
        return shape.getGlobalBounds().intersects(targetShape.getGlobalBounds());
    }

    sf::RectangleShape getShape() const {
        return shape;
    }

    // Definition of the equality operator to allow std::remove to work for the removal of each bullet that exits the right side of the screen
    bool operator==(const Bullet& other) const {
        return shape.getPosition() == other.shape.getPosition() && speed == other.speed;
    }

    // Set bullet's velocity
    void setVelocity(const sf::Vector2f& direction) {
        velocity = direction * speed;
    }

    // Integer method that returns the damage inflicted by the bullet
    int getDamage() const { return damage; }

public:
    sf::Vector2f position;
    sf::Vector2f direction;  // Bullet's movement direction
};

// Enemy bullet class for each enemy that fires bullets, inherited from the bullet class, so the direction of the enemy bullet fires to the left of the screen
class enemyBullet : public Bullet {
public:
    // Constructor: Initialize with position, speed, color, and damage
    enemyBullet(float x, float y, float speed, sf::Color color, int damage)
        : Bullet(x, y, speed, color, damage) {
        // Set the bullet to move left by setting the direction to -1.x (negative X direction)
        setVelocity(sf::Vector2f(-1.f, 0.f));  // Move left
    }

    // Override the update method to move the bullet according to the new direction
    void update() {
        shape.move(velocity);  // Move the bullet in the direction defined by velocity
    }
};
// Entity class to be used for the player and enemy classes from which they inherit
class Entity {
public:
    Entity(float x, float y, sf::Color color, float width, float height, int health)
        : health(health), maxHealth(health) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(color);
        shape.setPosition(x, y);
    }

    // Method to render the entity in the window
    void render(sf::RenderWindow& window) {
        window.draw(shape);
    }

    // Method for returning the health of the entity
    int getHealth() const {
        return health;
    }

    // Method for taking damage
    void takeDamage(int amount) {
        health -= amount;
        if (health < 0) health = 0;
    }

    // Boolean variable checking if the entity is alive if the health is greater than 0
    bool isAlive() const { return health > 0; }

    // Setting the shape of each entity to a rectangle
    sf::RectangleShape shape;

    virtual ~Entity() = default;

public:
    int health;
    int maxHealth;
};

// Definition of the player class
class Player : public Entity {
public:
    float speed;  // Player movement speed
    sf::Clock fireCooldownClock;  // Timer for firing cooldown
    float fireCooldownTime = 0.1f;  //Float var Cooldown time between shots (in seconds) to control how fast the player can shoot

    enum Direction {
        NONE,
        UP,
        DOWN,
        LEFT,
        RIGHT
    } currentDirection;

    // Constructor for Player class
    Player(float x, float y, sf::Color color, float width, float height, int health, float speed)
        : Entity(x, y, color, width, height, health), speed(speed), currentDirection(NONE) {
        shape.setPosition(position);
    }

    sf::Vector2f position;  // Add a position variable to explicitly track the player's position

    // Reset the player's state
    void reset() {
        health = 100;  // Reset health to 100 (or whatever starting health is)
        position = sf::Vector2f(100.f, 100.f);  // Reset to a starting position
        shape.setPosition(position);  // Update the shape's position
        currentDirection = NONE;  // Reset direction to NONE
        fireCooldownClock.restart();  // Reset cooldown clock
    }

    // Method to handle player movement based on keyboard input wasd
    void updateMovement() {
        // Check for key presses and update the currentDirection var
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            currentDirection = UP;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            currentDirection = DOWN;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            currentDirection = LEFT;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            currentDirection = RIGHT;
        }

        // Move the player based on the currentDirection
        switch (currentDirection) {
        case UP:
            shape.move(0.f, -speed);  // Move up
            break;
        case DOWN:
            shape.move(0.f, speed);   // Move down
            break;
        case LEFT:
            shape.move(-speed, 0.f);  // Move left
            break;
        case RIGHT:
            shape.move(speed, 0.f);   // Move right
            break;
        default:
            break;  // Do nothing if no key is pressed
        }
    }

    // Method to handle shooting for the player
    void updateShooting(std::vector<Bullet>& bullets) {
        // Only shoot if space is pressed and the cooldown is over
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            // Only fire if enough time has passed since the last shot
            if (fireCooldownClock.getElapsedTime().asSeconds() >= fireCooldownTime) {
                float spawnX = shape.getPosition().x + shape.getSize().x;  // Right side of the red box
                float spawnY = shape.getPosition().y + shape.getSize().y / 2.f - 2.5f;  // Center height of red box

                // Create a bullet and push it to the bullets vector
                bullets.push_back(Bullet(spawnX, spawnY, 15.f, sf::Color::Yellow, 5));  // Bullet speed: 15.f, damage: 10
                fireCooldownClock.restart();  // Restart the cooldown timer after firing to ensure consistent firing
            }
        }
    }

    //  the render method to draw the player
    void render(sf::RenderWindow& window) {
        window.draw(shape);
    }

    // Getter for speed
    float getSpeed() const {
        return speed;
    }

    // Get the player's position (to let enemies follow them)
    sf::Vector2f getPosition() const {
        return shape.getPosition();
    }

    // Get the size of the player's rectangle shape (width, height)
    sf::Vector2f getSize() const {
        return shape.getSize();  // Returns the width and height of the player's shape
    }
    // method so the player can take damage
    void takeDamage(int amount)  {
        Entity::takeDamage(amount);  // Call the base class takeDamage

        if (health == 0) {
            std::cout << "Player is dead!" << std::endl;  // Output to the console to confirm the player is dead if their health is equal to 0
        }
    }

      // Tracks the player's coins

    // Add a coin
    void addCoin() {
        coinCount++;
    }

    // Reset coins when player dies
    void resetCoins() {
        coinCount = 0;
    }

    // Render coins in the top-right corner
    void renderCoins(sf::RenderWindow& window, sf::Texture& coinTexture, sf::Font& font) {
        // Coin sprite
        sf::Sprite coinSprite;
        coinSprite.setTexture(coinTexture);
        coinSprite.setPosition(window.getSize().x - 100.f, 20.f);  // Position it in the top-right corner
        window.draw(coinSprite);

        // Coin amount
        sf::Text coinText;
        coinText.setFont(font);
        coinText.setString(std::to_string(coinCount));
        coinText.setCharacterSize(30);
        coinText.setFillColor(sf::Color::Yellow);
        coinText.setPosition(window.getSize().x - 50.f, 20.f);  // Position next to coin image
        window.draw(coinText);
    }

};


// Enemy class, inherits from Entity
class Enemy : public Entity {
public:

    int health;
    float speed;
    sf::Clock shootCooldownClock;
    float shootCooldownTime = 0.2f;
    sf::Vector2f shootDirection;  // Direction of the bullet when shootin
    float speedFactor;  // Factor to make the enemy slower than the player

    // Constructor for Enemy, which calls the base Entity constructor
    Enemy(float x, float y, sf::Color color, float width, float height, int health, float speedFactor)
        : Entity(x, y, color, width, height, health), speed(speed), speedFactor(speedFactor) {
    }

   

    // Take damage and print when destroyed
    void takeDamage(int damage) {
        Entity::takeDamage(damage);  // Call base class takeDamage

        if (!isAlive()) {
            std::cout << "Enemy destroyed!" << std::endl; // Output to the console to confirm the enemy is destroyed if they are notAlive
        }
    }

    // the isAlive boolean method to return true if their health is greater than 0
    bool isAlive() const {
        return getHealth() > 0;  // Assumes `getHealth()` is correctly implemented in the base class
    }

    // Method to update the enemy's movement (towards the player)
    void moveTowardsPlayer(const sf::Vector2f& playerPosition, float playerSpeed, const std::vector<Enemy>& enemies) {
        // Update the speed of the enemy to match the player's speed
        speed = playerSpeed * speedFactor;

        // Calculate direction vector towards the player
        sf::Vector2f direction = playerPosition - shape.getPosition();
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0) {
            direction /= length;  // Normalize direction vector
            sf::Vector2f newPosition = shape.getPosition() + direction * speed;  // Calculate the new position

            // Check if the new position causes an overlap with any other enemy
            if (!isOverlapping(newPosition, enemies)) {
                shape.move(direction * speed);  // Only move if no overlap between enemies
            }
        }
    }

    // Check if this enemy's new position will overlap with any other enemy
    bool isOverlapping(const sf::Vector2f& newPosition, const std::vector<Enemy>& enemies) {
        // Create a temporary shape for the enemy's new position
        sf::RectangleShape tempShape = shape;
        tempShape.setPosition(newPosition);

        // Check for overlap with other enemies
        for (const auto& enemy : enemies) {
            if (&enemy != this && tempShape.getGlobalBounds().intersects(enemy.shape.getGlobalBounds())) {
                return true;  // Return true if there's an overlap
            }
        }
        return false;  // No overlap detected
    }

    // Method for shooting at the player
    void shootAtPlayer(const sf::Vector2f& playerPosition, std::vector<enemyBullet>& enemyBullets) {
        if (shootCooldownClock.getElapsedTime().asSeconds() >= shootCooldownTime) {
            // Calculate the direction of the bullet towards the player
            sf::Vector2f bulletDirection(1.f, 0.f);

            
    

                // Create a bullet and add it to the vector of bullets
                float spawnX = shape.getPosition().x + shape.getSize().x / 2.f;  // Middle of the enemy
                float spawnY = shape.getPosition().y + shape.getSize().y / 2.f;  // Center height of the enemy

                enemyBullets.push_back(enemyBullet(spawnX, spawnY, 10.f, sf::Color::Red, 5));  // Create bullet with damage 10

                // Reset the shoot cooldown timer
                shootCooldownClock.restart();
            
        }
    }

};


// Function to initialize font and text
void initializeGameOverText() {
    if (!font.loadFromFile("C:/Users/kwood/source/Repos/3rdYEAR_GAME/practical_1/robot.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
    }

    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(50);  // Set text size
    gameOverText.setFillColor(sf::Color::White);  // Set text color to white
    gameOverText.setStyle(sf::Text::Bold);  // Optional: Make text bold
}

// Method to initialize the text displayed when the player wins a level
void initializeVictoryText() {
    if (!font.loadFromFile("C:/Users/kwood/source/Repos/3rdYEAR_GAME/practical_1/robot.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
    }

    victoryGameText.setFont(font);
    victoryGameText.setString("Victory!");
    victoryGameText.setCharacterSize(50);  // Set text size
    victoryGameText.setFillColor(sf::Color::White);  // Set text color to white
    victoryGameText.setStyle(sf::Text::Bold);  // Optional: Make text bold
}

// Method to display the game over screen
void displayGameOverScreen(sf::RenderWindow& window) {
    // Create a blue rectangle for the "Game Over" screen background
    float rectWidth = 600.f;
    float rectHeight = 200.f;
    sf::RectangleShape gameOverRectangle(sf::Vector2f(rectWidth, rectHeight));
    gameOverRectangle.setFillColor(sf::Color::Blue);

    // Position the rectangle in the center of the window
    gameOverRectangle.setPosition(
        (window.getSize().x - rectWidth) / 2.f,
        (window.getSize().y - rectHeight) / 2.f
    );

    // Center the text within the rectangle
    gameOverText.setPosition(
        (window.getSize().x - gameOverText.getLocalBounds().width) / 2.f,
        (window.getSize().y - gameOverText.getLocalBounds().height) / 2.f
    );

    // Draw the rectangle and text to the window
    window.draw(gameOverRectangle);
    window.draw(gameOverText);
}

void displayVictoryScreen(sf::RenderWindow& window) {
    // Create a blue rectangle for the "Victory" screen background

    

    float rectWidth = 600.f;
    float rectHeight = 200.f;
    sf::RectangleShape gameOverRectangle(sf::Vector2f(rectWidth, rectHeight));
    gameOverRectangle.setFillColor(sf::Color::Blue);

    // Position the rectangle in the center of the window
    gameOverRectangle.setPosition(
        (window.getSize().x - rectWidth) / 2.f,
        (window.getSize().y - rectHeight) / 2.f
    );

    // Center the text within the rectangle
    victoryGameText.setPosition(
        (window.getSize().x - victoryGameText.getLocalBounds().width) / 2.f,
        (window.getSize().y - victoryGameText.getLocalBounds().height) / 2.f
    );



    // Draw the rectangle and text
    window.draw(gameOverRectangle);
    window.draw(victoryGameText);
}

// renderMainMenu function so we can return to the main menu upon completion or failure of a level
void renderMainMenu(sf::RenderWindow& window, Button& startButton, Button& settingsButton, Button& garageButton, Button& exitButton) {
    
    startButton.render(window);
    settingsButton.render(window);
    garageButton.render(window);
    exitButton.render(window);
    window.display();  // Update the display after rendering the main menu
}

// resetGameState method so we can reset all of the values of the game upon completion or faikure of a level
void resetGameState(bool&level1Started, bool&levelWon, Player& player,
    std::vector<Enemy>& enemies, std::vector<Bullet>& bullets, std::vector<enemyBullet>& enemyBullets) {
    
    // Ensure you stop the game and go to the main menu with all game variables being reset
    level1Started = false;  // Stops the current level
    levelWon = false;

    player.reset();  // Reset player state (e.g., health, position)
    enemies.clear(); // Clear any existing enemies
    bullets.clear(); // Clear bullets
    enemyBullets.clear(); // Clear enemy bullets
}

int main() {
    // setting the integer variables for the screen width and the screen height
    int width = 1920;
    int height = 900;

    // Initalisation of 20 boolean variables which will track where the game state currently is so as to ensure a smooth transition between game menus
    bool level1Won = false;
    bool level2Won = false;
    bool level3Won = false;
    bool level4Won = false;
    bool level5Won = false;
    bool level6Won = false;
    bool level7Won = false;
    bool level8Won = false;
    bool level9Won = false;
    bool level10Won = false;
    
    bool level1Started = false;
    bool level2Started = false;
    bool level3Started = false;
    bool level4Started = false;
    bool level5Started = false;
    bool level6Started = false;
    bool level7Started = false;
    bool level8Started = false;
    bool level9Started = false;
    bool level10Started = false;

    // Calling upon the sf RenderWindow method and setting the resolution to 1920x1080
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Warfare In Sky");
    bool isFullScreen = true; // tracking variable to track if we are in fullscreen or not (for the settings menu)
    

    // Load the coin image from the file
    // IMPORTANT NOTE: AN ABSOLUTE DIRECTORY HAD TO BE USED FOR THIS, REPLACE WITH YOUR OWN IF RUNNING FROM VISUAL STUDIO
    sf::Texture coinTexture;
    if (!coinTexture.loadFromFile("C:/Users/kwood/source/Repos/3rdYEAR_GAME/practical_1/key.png")) {
        std::cerr << "Error loading coin image!" << std::endl;
        return -1;
    }

    // Load the font to be used
    if (!font.loadFromFile("C:/Users/kwood/source/Repos/3rdYEAR_GAME/practical_1/robot.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    // Load the background image texture
    // IMPORTANT NOTE: I had to use an absolute directory to load the background images as it would not work any other way, so replace my username with your own otherwise this won't work
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("C:/Users/kwood/source/Repos/3rdYEAR_GAME/practical_1/pixelated-sky-2.jpg")) {
        std::cerr << "Error loading background image!" << std::endl;
        return -1;
    }

    // a text button to toggle fullscreen
    sf::Text fullscreenText;
    fullscreenText.setFont(font);
    fullscreenText.setString("Toggle Fullscreen (F)");
    fullscreenText.setCharacterSize(30);
    fullscreenText.setFillColor(sf::Color::White);
    fullscreenText.setPosition(300, 200);  // Position for fullscreen toggle text

    //  a text button to switch to windowed mode
    sf::Text windowedModeText;
    windowedModeText.setFont(font);
    windowedModeText.setString("Windowed Mode (800x600)");
    windowedModeText.setCharacterSize(30);
    windowedModeText.setFillColor(sf::Color::White);
    windowedModeText.setPosition(300, 250);  // Position for windowed mode text


    // Create the sprites for the background cloud image animation
    sf::Sprite backgroundSprite1;
    sf::Sprite backgroundSprite2;

    // Setting the background images to the same image
    backgroundSprite1.setTexture(backgroundTexture);
    backgroundSprite2.setTexture(backgroundTexture);

    // Set the initial position of both sprites
    backgroundSprite1.setPosition(0.f, 0.f);
    backgroundSprite2.setPosition(backgroundTexture.getSize().x, 0.f);

    // Define speed for cloud movement, using floats
    float cloudSpeed = 0.1f;
    float cloudWidth = backgroundTexture.getSize().x;

    // Load font
    sf::Font font;
    if (!font.loadFromFile("C:/Users/kwood/source/Repos/3rdYEAR_GAME/practical_1/robot.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    // Draw background clouds
    window.draw(backgroundSprite1);
    window.draw(backgroundSprite2);


    // Define button sizes and positions
    float rectWidth = 800.f;
    float rectHeight = 200.f;
    float spacing = 50.f;

    // DefinING the position for each button for main menu based on screen settings
    sf::Vector2f position1((width - rectWidth) / 2, (height - 3 * rectHeight - 2 * spacing) / 2);
    sf::Vector2f position2((width - rectWidth) / 2, (height - 3 * rectHeight - 2 * spacing) / 2 + rectHeight + spacing);
    sf::Vector2f position3((width - rectWidth) / 2, (height - 3 * rectHeight - 2 * spacing) / 2 + 2 * (rectHeight + spacing));
    sf::Vector2f position4((width - rectWidth) / 2, (height - 4 * rectHeight - 3 * spacing) / 2 + 3 * (rectHeight + spacing) + 100.f);

    // Initialization of each button for the main menu, with a font and a relevant label
    Button startButton(position1, "Start Game", font);
    Button settingsButton(position2, "Settings", font);
    Button garageButton(position3, "Garage", font);
    Button exitButton(position4, "Exit Game", font);

    // Create the fullscreen button
    Button fullscreenButton(position1, "Fullscreen off/on", font);

    
    // Button for returning to the main menu from the game
    Button backButton(sf::Vector2f((width - rectWidth) / 2, height - rectHeight - 30.f), "Back to Main Menu", font);

    bool levelWon = false;
    bool inGameMenu = false;
    bool inSettingsMenu = false;
    bool inGarageMenu = false;  // Track if we are in the garage menu
    bool inGame = false;  // Flag to track if we're in the game (red box)
    bool inVictoryScreen = false;
    bool inDefeatScreen = false;


    // Create a text object for the header
    sf::Text headerText;
    headerText.setFont(font);
    headerText.setString("Warfare In Sky");
    headerText.setCharacterSize(80);
    headerText.setFillColor(sf::Color::Blue);

    // Center the header text
    float headerX = (width - headerText.getLocalBounds().width) / 2;
    headerText.setPosition(headerX, (height - 3 * rectHeight - 2 * spacing) / 2 - 100.f);

    // Instantiate the Player object with similar properties
    float playerSpeed = 0.1f;
    int playerHealth = 100;
    Player player(width / 5.f, height / 2.f, sf::Color::Green, 50.f, 50.f, playerHealth, playerSpeed);


    // Instantiate the enemies vector which will store all the enemies inside
    std::vector<Enemy> enemies;
    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 0.5f));
    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 0.5f));

    // Declare a clock to track the firing cooldown
    sf::Clock fireCooldownClock;
    float fireCooldownTime = 0.1f;  // Time in seconds between shots, this value ensures that the player wont fire too fast


    // initialisation of vectors enemyBullets and bullets(for the player)
    std::vector<enemyBullet> enemyBullets;
    std::vector<Bullet> bullets;

    

    // Call the function to initialize the "Game Over" text to prepare for displaying the gameDefeatScreen
    initializeGameOverText();

    // Calls the function intializeVictoryText() to setup the text to be displayed in victory message
    initializeVictoryText();

    // Main game loop while the window is open
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // Closes the window
            }

            
            

            // Button click handlers for the main menu (Start Game, Settings, Garage, Exit Game)
            if (!inGameMenu && !inSettingsMenu && !inGarageMenu) {  // Main menu
                if (startButton.isClicked(window)) {
                    std::cout << "Start Game button clicked!" << std::endl;
                    inGameMenu = true;  // Switch to the game menu
                    player.renderCoins(window, coinTexture, font); // Render the currency at the top right hand of the screen
                }

                if (settingsButton.isClicked(window)) {
                    std::cout << "Settings button clicked!" << std::endl;
                    inSettingsMenu = true;  // Switch to the settings menu
                    player.renderCoins(window, coinTexture, font); // Render the currency at the top right of the screen
                }

                if (garageButton.isClicked(window)) {
                    std::cout << "Garage button clicked!" << std::endl;
                    inGarageMenu = true;  // Switch to the garage menu
                    player.renderCoins(window, coinTexture, font); // Render the currency at the top right of the screen
                }

                if (exitButton.isClicked(window)) {
                    std::cout << "Exit Game button clicked!" << std::endl; // Console output so we can track if this button was being pressed for debugging purposes
                    window.close();
                }
            }
            else if (inGameMenu) {  // Game menu (Level selection)
                if (backButton.isClicked(window)) {
                    std::cout << "Back to Main Menu button clicked!" << std::endl; // This is for debugging purposes
                    inGameMenu = false;  // Goes back to main menu
                    player.renderCoins(window, coinTexture, font); // // Render the currency at the top right of the screen
                }
                // Loop through levels 1 to 10 to display the levels 1 to 10 in the level choice menu
                for (int level = 1; level <= 10; ++level) {
                    // Calculate button position for each level
                    float buttonPosX = (width - 1000.f) / 2 + (level - 1) * 100;
                    float buttonPosY = height / 2 - 50.f;  // Y position stays constant

                    // Check if mouse is pressed and inside the level button area
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        if (mousePos.x > buttonPosX && mousePos.x < buttonPosX + 100 &&
                            mousePos.y > buttonPosY && mousePos.y < buttonPosY + 100) {

                            std::cout << "Level " << level << " clicked!" << std::endl;

                            inGame = true;  // Enter the game
                            inGameMenu = false;  // Exit the level selection menu

                            // Set the appropriate level flag to true
                            switch (level) {
                            case 1:
                                player.renderCoins(window, coinTexture, font);
                                level1Started = true;
                                level1Won = false;

                                // Reset the level flag and victory flag before starting
                                level1Started = true;  // Indicate that Level 1 has started
                                levelWon = false;      // Ensure the victory flag is false
                                player.reset();        // Reset the players variables
                                enemies.clear();       // Clear the enemies list (or initialize new enemies for the level)
                                bullets.clear();       // Clear player bullets 
                                enemyBullets.clear();  // Clear enemy bullets 

                                // Enemies to spawn for level 1
                                enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 0.5f)); // Enemy for level 1
                                enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 0.5f)); // Enemy for level 1


                                break;
                             case 2:
                                

                                

                                // Reset the level flag and victory flag before starting
                                level2Started = true;  // Indicate that Level 2 has started
                                level2Won = false;      // Ensure the victory flag level 2 is false
                                player.reset();        // Reset the player
                                enemies.clear();       // Clear the enemies list or initialize new enemies for new level
                                bullets.clear();       
                                enemyBullets.clear();  

                                // Enemies for level 2
                                enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));

                                enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 


                                break;

                                case 3:

                                    // Reset the level flag and victory flag before starting
                                    level3Started = true;  // Indicate that Level 2 has started
                                    level3Won = false;      // Ensure the victory flag is false
                                    player.reset();        // Reset the player (position, health, etc.)
                                    enemies.clear();      
                                    bullets.clear();       
                                    enemyBullets.clear();  

                                    // enemies for level 3
                                    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));

                                    break;
                                case 4:

                                    // Reset the level flag and victory flag before starting and resets all values so everythung displays correctly
                                    level4Started = true;  
                                    level4Won = false;      
                                    player.reset();        
                                    enemies.clear();       
                                    bullets.clear();       
                                    enemyBullets.clear();  

                                    // Enemies for level 4
                                    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));


                                    break;
                                case 5:

                                    // Reset the level flag and victory flag before starting and resets all game vars
                                    level5Started = true;  // Indicate that Level 2 has started
                                    level5Won = false;     
                                    player.reset();        
                                    enemies.clear();       
                                    bullets.clear();       
                                    enemyBullets.clear();  

                                    // Enemies for level 5
                                    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 2.f));

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    break;

                                case 6:

                                    // Reset the level flag and victory flag before starting
                                    level6Started = true;  // Indicate that Level 2 has started
                                    level6Won = false;      
                                    player.reset();        
                                    enemies.clear();      
                                    bullets.clear();       
                                    enemyBullets.clear();  

                                    // Enemies for level 6
                                    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 2.f));

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    break;

                                case 7:

                                    // Reset the level flag and victory flag before starting
                                    level5Started = true; 
                                    level5Won = false;      
                                    player.reset();        
                                    enemies.clear();       
                                    bullets.clear();       
                                    enemyBullets.clear();  

                                    // Enemies for level 7
                                    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 2.f));

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    break;

                                case 8:

                                    // Reset the level flag and victory flag before starting
                                    level5Started = true;  // Indicate that Level 2 has started
                                    level5Won = false;      // Ensure the victory flag is false
                                    player.reset();        // Reset the player (position, health, etc.)
                                    enemies.clear();       // Clear the enemies list (or initialize new enemies for the level)
                                    bullets.clear();       // Clear player bullets (reset for the level)
                                    enemyBullets.clear();  // Clear enemy bullets (reset for the level)

                                    // Optionally: Add code to spawn enemies for Level 1
                                    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); // Example enemy
                                    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); // Example enemy

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); // Example enemy
                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 2.f));

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    break;

                                case 9:
                                    // Reset the level flag and victory flag before starting
                                    level5Started = true;  // Indicate that Level 2 has started
                                    level5Won = false;      
                                    player.reset();        
                                    enemies.clear();       
                                    bullets.clear();       
                                    enemyBullets.clear();  

                                    // enemies for Level 9
                                    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 2.f));

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    break;
                                    
                                case 10:
                                    // Reset the level flag and victory flag before starting
                                    level10Started = true;  // Indicate that Level 2 has started
                                    level10Won = false;      
                                    player.reset();        
                                    enemies.clear();       
                                    bullets.clear();       
                                    enemyBullets.clear();  

                                    // enemies for Level 10
                                    enemies.push_back(Enemy(1700.f, 300.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); // Enemies with damage 1.f
                                    enemies.push_back(Enemy(1600.f, 500.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f)); 
                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 2.f));

                                    enemies.push_back(Enemy(1500.f, 200.f, sf::Color::Red, 50.f, 50.f, 50, 1.f));
                                    break;
                                
                            }
                            break;  // Exit the loop after the level is selected
                        }
                    }
                }

            }
            else if (inSettingsMenu) {  // Settings menu
                if (backButton.isClicked(window)) {

                    


                    std::cout << "Back to Main Menu button clicked!" << std::endl;
                    inSettingsMenu = false;  // Go back to main menu
                    player.renderCoins(window, coinTexture, font);
                }
            }
            else if (inGarageMenu) {  // Garage menu
                if (backButton.isClicked(window)) {
                    std::cout << "Back to Main Menu button clicked!" << std::endl;
                    inGarageMenu = false;  // Go back to main menu
                }
            } // Handle the Back to Main Menu button click when in the game
            else if (inGame && backButton.isClicked(window)) {
                std::cout << "Back to Main Menu clicked!" << std::endl;
                inGame = false;  // Set inGame to false, going back to the main menu
                inGameMenu = false;  // Ensure the game menu doesn't stay active so it doesnt overlap with other menu items
            } else if (inVictoryScreen && levelWon) {
                
                backButton.render(window); // Render the back to main menu button to each window
            }
        }

       
        player.renderCoins(window, coinTexture, font); // Render the currency to the screen constantly
        

        // Update cloud background position 
        backgroundSprite1.move(-cloudSpeed, 0.f);
        backgroundSprite2.move(-cloudSpeed, 0.f);

        if (backgroundSprite1.getPosition().x <= -cloudWidth) {
            backgroundSprite1.setPosition(backgroundSprite2.getPosition().x + cloudWidth, 0.f);
        }

        if (backgroundSprite2.getPosition().x <= -cloudWidth) {
            backgroundSprite2.setPosition(backgroundSprite1.getPosition().x + cloudWidth, 0.f);
        }

        // Handle movement of red box when inGame is true
        // Movement speed
        float moveSpeed = 0.2f;

        // Create a velocity vector to store movement direction (starts with no movement)
        sf::Vector2f velocity(0.f, 0.f);

        if (inGame) {

            player.renderCoins(window, coinTexture, font);

            // Remove bullets that are out of bounds (right side of the screen)
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](const Bullet& bullet) {
                return bullet.isOutOfBounds(window.getSize().x);  // Bullet is off the screen
                }), bullets.end());

            
            

           
            
        }

       
        // Draw background clouds
        window.draw(backgroundSprite1);
        window.draw(backgroundSprite2);

        // Draw header text
        window.draw(headerText);


        if (!inGameMenu && !inSettingsMenu && !inGarageMenu && !inGame) {  // Main menu
            startButton.render(window);
            settingsButton.render(window);
            garageButton.render(window);
            exitButton.render(window);
            player.renderCoins(window, coinTexture, font);
            
        }
        else if (inGameMenu) {  // Game menu (Level selection)
            // Formatting the level choice menu
            sf::RectangleShape levelBox(sf::Vector2f(1100.f, 333.f));
            levelBox.setPosition((width - levelBox.getSize().x) / 2, (height - levelBox.getSize().y) / 2);
            levelBox.setFillColor(sf::Color(0, 0, 255));
            player.renderCoins(window, coinTexture, font);

            // Draws the box where all of the levels through 1 to 10 are displayed
            window.draw(levelBox);

            // Display level numbers (1 to 10) horizontally
            float levelSpacing = 100.f;
            for (int i = 1; i <= 10; ++i) {
                sf::Text levelText;
                levelText.setFont(font);
                levelText.setString(std::to_string(i));
                levelText.setCharacterSize(50);
                levelText.setFillColor(sf::Color::White);
                levelText.setPosition((width - 1000.f) / 2 + levelSpacing * (i - 1), height / 2);

                window.draw(levelText);
            }

            backButton.render(window);
        }
        else if (inSettingsMenu) {  // Settings menu
            
            player.renderCoins(window, coinTexture, font);

            // Handle the fullscreen toggle button click
            if (fullscreenButton.isClicked(window)) {
                std::cout << "Fullscreen button clicked!" << std::endl;
                if (isFullScreen) {
                    // Set window to windowed mode (800x600)
                    window.create(sf::VideoMode(1920, 1080), "Game", sf::Style::Close);
                    isFullScreen = false;
                }
                else {
                    // Set window to fullscreen mode (use the current screen resolution)
                    window.create(sf::VideoMode::getDesktopMode(), "Game", sf::Style::Fullscreen);
                    isFullScreen = true;
                }
            }

            


            
            fullscreenButton.render(window);

            backButton.render(window);
        }
        else if (inGarageMenu) {  // Garage menu
            sf::RectangleShape garageBox(sf::Vector2f(600.f, 200.f));
            garageBox.setPosition((width - garageBox.getSize().x) / 2, (height - garageBox.getSize().y) / 2);
            garageBox.setFillColor(sf::Color(0, 0, 255));

            player.renderCoins(window, coinTexture, font);

            window.draw(garageBox);

            backButton.render(window);
        }

        // If level 1 has been started, draw the player box and enemies
        if (level1Started == true) {

            inVictoryScreen = false;
            inDefeatScreen = false;

            // Remove dead enemies safely
            enemies.erase(
                std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                    return !enemy.isAlive();  // Remove enemies that are dead
                    }),
                enemies.end()
            );

            // Check if player is dead
            if (!player.isAlive()) {
                std::cout << "Game Over! Player has died!" << std::endl;
                level1Started = false;
            }

            if (enemies.empty()) {
                level1Won = true;  // The player has won the level
                level1Started = false;
                std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
            }

            // Draw the player box (red box)
            player.render(window);  // The player's red box

            player.renderCoins(window, coinTexture, font);



            // Draw the enemies
            for (auto& enemy : enemies) {
                enemy.render(window);  // Render each enemy from the enemies vector
            }

            

            // Update player bullets and remove out-of-bounds ones
            for (auto it = bullets.begin(); it != bullets.end(); ) {
                it->update();  // Update the bullet's position

                // Check if the bullet is off-screen
                if (it->isOutOfBounds(width)) {
                    it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                }
                else {
                    ++it;  // Move to the next bullet
                }
            }

            // Update enemy bullets and remove out-of-bounds ones
            for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                it->update();  // Update the enemy bullet's position

                // Check if the bullet is off-screen (out of bounds)
                if (it->isOutOfBounds(width)) {
                    it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                }
                else {
                    ++it;  // Move to the next enemy bullet
                }
            }

            // Render the player bullets
            for (const auto& bullet : bullets) {
                window.draw(bullet.shape);
            }

            // Render the enemy bullets
            for (const auto& bullet : enemyBullets) {
                window.draw(bullet.shape);
            }


            // Define the starting position for the health bars and labels
            sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

            // Render player health bar and label
            renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

            // Adjust the vertical spacing between enemy health bars
            float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

            // Render health bars for each enemy dynamically
            for (size_t i = 0; i < enemies.size(); ++i) {
                // Adjust the vertical position for each enemy's health bar
                renderHealthBar(window,
                    sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                    "Enemy " + std::to_string(i + 1),
                    enemies[i].getHealth(),
                    50);
            }

            // Handle player and enemy updates here movement, collision detection


            player.updateMovement();
            player.updateShooting(bullets);  // This handles shooting and firing cooldown





            // Check for collisions between enemy bullets and player
            for (auto& bullet : enemyBullets) {
                if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                    player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                    bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                }
            }

            // Check for collisions between player bullets and enemies
            for (auto& bullet : bullets) {
                // If the bullet is fired by the player and hits an enemy
                for (auto& enemy : enemies) {
                    if (bullet.checkCollision(enemy.shape)) {
                        enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                        bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                        break;  // Exit the inner loop as the bullet has already hit an enemy
                    }
                }
            }

            for (auto it = enemies.begin(); it != enemies.end(); ) {
                if (!it->isAlive()) {
                    player.addCoin();  // Add 1 coin when an enemy is destroyed
                    it = enemies.erase(it);  // Remove enemy from the list
                }
                else {
                    ++it;
                }
            }

            


            // Reset coins if the player dies
            if (!player.isAlive()) {
                player.resetCoins();  // Reset the coin count on player death
            }


            // Move each enemy towards the player
            for (auto& enemy : enemies) {
                enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                enemy.shootAtPlayer(player.getPosition(), enemyBullets);
            }


        }
 else if (!level1Started && level1Won == true) {
     
     inVictoryScreen = true;
     displayVictoryScreen(window);
     backButton.render(window);

     if (backButton.isClicked(window)) {
         std::cout << "Back to Main Menu button clicked!" << std::endl;

         // Reset all flags
         inGame = false;            // Ensure the game isn't active
         inVictoryScreen = false; 
         inGameMenu = false;       
         inSettingsMenu = false;   
         inGarageMenu = false;      

         
         level1Started = false;     
         level1Won = false;         
         player.reset();            

         
         enemies.clear();          

         // Reset the UI elements
         renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
     }

     
     
     
        }
 else if (!level1Started && !player.isAlive()) {
            
     inDefeatScreen = true;
     displayGameOverScreen(window);
     backButton.render(window);

     if (backButton.isClicked(window)) {
         std::cout << "Back to Main Menu button clicked!" << std::endl;

         // Reset all flags
         inGame = false;            // Ensure the game isn't active
         inVictoryScreen = false;   
         inGameMenu = false;        
         inSettingsMenu = false;    
         inGarageMenu = false;      

         // Reset level flags
         level1Started = false;     
         levelWon = false;          
         player.reset();            

         // Reset the enemies 
         enemies.clear();   // Clear all enemies

         // Reset the UI elements
         renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
     }


        }
 else if (level2Started) {

     inVictoryScreen = false;
     inDefeatScreen = false;

     // Remove dead enemies safely
     enemies.erase(
         std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
             return !enemy.isAlive();  // Remove enemies that are dead
             }),
         enemies.end()
     );

     // Check if player is dead
     if (!player.isAlive()) {
         std::cout << "Game Over! Player has died!" << std::endl;
         level2Started = false;
     }

     if (enemies.empty()) {
         level2Won = true;  // The player has won the level
         level2Started = false;
         std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
     }

     // Draw the player box (red box)
     player.render(window);  // The player's red box


     player.renderCoins(window, coinTexture, font);


     // Draw the enemies
     for (auto& enemy : enemies) {
         enemy.render(window);  // Render each enemy from the enemies vector
     }


     // Update player bullets and remove out-of-bounds ones
     for (auto it = bullets.begin(); it != bullets.end(); ) {
         it->update();  // Update the bullet's position

         // Check if the bullet is off-screen (out of bounds)
         if (it->isOutOfBounds(width)) {
             it = bullets.erase(it);  // Remove the bullet if it's out of bounds
         }
         else {
             ++it;  // Move to the next bullet
         }
     }

     // Update enemy bullets and remove out-of-bounds ones
     for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
         it->update();  // Update the enemy bullet's position

         // Check if the bullet is off-screen (out of bounds)
         if (it->isOutOfBounds(width)) {
             it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
         }
         else {
             ++it;  // Move to the next enemy bullet
         }
     }

     // Render the player bullets
     for (const auto& bullet : bullets) {
         window.draw(bullet.shape);
     }

     // Render the enemy bullets
     for (const auto& bullet : enemyBullets) {
         window.draw(bullet.shape);
     }


     // Define the starting position for the health bars and labels
     sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

     // Render player health bar and label
     renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

     // Adjust the vertical spacing between enemy health bars
     float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

     // Render health bars for each enemy dynamically
     for (size_t i = 0; i < enemies.size(); ++i) {
         // Adjust the vertical position for each enemy's health bar
         renderHealthBar(window,
             sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
             "Enemy " + std::to_string(i + 1),
             enemies[i].getHealth(),
             50);
     }

     // Handle player and enemy updates here (movement, collision detection, etc.)


     player.updateMovement();
     player.updateShooting(bullets);  // This handles shooting and firing cooldown





     // Check for collisions between enemy bullets and player
     for (auto& bullet : enemyBullets) {
         if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
             player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
             bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
         }
     }

     // Check for collisions between player bullets and enemies
     for (auto& bullet : bullets) {
         // If the bullet is fired by the player and hits an enemy
         for (auto& enemy : enemies) {
             if (bullet.checkCollision(enemy.shape)) {
                 enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                 bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                 break;  // Exit the inner loop as the bullet has already hit an enemy
             }
         }
     }

     for (auto it = enemies.begin(); it != enemies.end(); ) {
         if (!it->isAlive()) {
             player.addCoin();  // Add 1 coin when an enemy is destroyed
             it = enemies.erase(it);  // Remove enemy from the list
         }
         else {
             ++it;
         }
     }




     // Reset coins if the player dies
     if (!player.isAlive()) {
         player.resetCoins();  // Reset the coin count on player death
     }

     // Move each enemy towards the player
     for (auto& enemy : enemies) {
         enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

         enemy.shootAtPlayer(player.getPosition(), enemyBullets);
     }

        }
 else if (!level2Started && level2Won == true) {
     {

         inVictoryScreen = true;
         displayVictoryScreen(window);
         backButton.render(window);

         if (backButton.isClicked(window)) {
             std::cout << "Back to Main Menu button clicked!" << std::endl;

             // Reset all flags
             inGame = false;            
             inVictoryScreen = false;   
             inGameMenu = false;        
             inSettingsMenu = false;    
             inGarageMenu = false;      

             // Reset level flags
             level2Started = false;     // Reset the level started flag
             level2Won = false;          // Reset the win flag
             player.reset();            // Reset the player (if needed)

             // Reset the enemies
             enemies.clear();           // Clear all enemies

             // Reset the UI elements
             renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
         }
     }
        } else if (!level2Started && !player.isAlive()) {
            // DISPLAY GAME OVER SCREEN WHEN THE PLAYER DIES 
     inDefeatScreen = true;
     displayGameOverScreen(window);
     backButton.render(window);

     if (backButton.isClicked(window)) {
         std::cout << "Back to Main Menu button clicked!" << std::endl;

         // Reset all flags
         inGame = false;           
         inVictoryScreen = false;  
         inGameMenu = false;        
         inSettingsMenu = false;    
         inGarageMenu = false;     

         // Reset level flags
         level2Started = false;     // Reset the level started flag
         level2Won = false;          // Reset the win flag
         player.reset();            // Reset the player

         // Reset the enemies (if needed)
         enemies.clear();           // Clear all enemies

         // Reset the UI elements
         renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
     }


        } else if (level3Started) {

            inVictoryScreen = false;
            inDefeatScreen = false;

            // Remove dead enemies safely
            enemies.erase(
                std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                    return !enemy.isAlive();  // Remove enemies that are dead
                    }),
                enemies.end()
            );

            // Check if player is dead
            if (!player.isAlive()) {
                std::cout << "Game Over! Player has died!" << std::endl;
                level3Started = false;
            }

            if (enemies.empty()) {
                level3Won = true;  // The player has won the level
                level3Started = false;
                std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
            }

            // Draw the player box (red box)
            player.render(window);  // The player's red box


            player.renderCoins(window, coinTexture, font);


            // Draw the enemies
            for (auto& enemy : enemies) {
                enemy.render(window);  // Render each enemy from the enemies vector
            }


            // Update player bullets and remove out-of-bounds ones
            for (auto it = bullets.begin(); it != bullets.end(); ) {
                it->update();  // Update the bullet's position

                // Check if the bullet is off-screen (out of bounds)
                if (it->isOutOfBounds(width)) {
                    it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                }
                else {
                    ++it;  // Move to the next bullet
                }
            }

            // Update enemy bullets and remove out-of-bounds ones
            for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                it->update();  // Update the enemy bullet's position

                // Check if the bullet is off-screen (out of bounds)
                if (it->isOutOfBounds(width)) {
                    it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                }
                else {
                    ++it;  // Move to the next enemy bullet
                }
            }

            // Render the player bullets
            for (const auto& bullet : bullets) {
                window.draw(bullet.shape);
            }

            // Render the enemy bullets
            for (const auto& bullet : enemyBullets) {
                window.draw(bullet.shape);
            }


            // Define the starting position for the health bars and labels
            sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

            // Render player health bar and label
            renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

            // Adjust the vertical spacing between enemy health bars
            float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

            // Render health bars for each enemy dynamically
            for (size_t i = 0; i < enemies.size(); ++i) {
                // Adjust the vertical position for each enemy's health bar
                renderHealthBar(window,
                    sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                    "Enemy " + std::to_string(i + 1),
                    enemies[i].getHealth(),
                    50);
            }

            // Handle player and enemy updates here (movement, collision detection, etc.)



            player.updateMovement();
            player.updateShooting(bullets);  // This handles shooting and firing cooldown





            // Check for collisions between enemy bullets and player
            for (auto& bullet : enemyBullets) {
                if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                    player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                    bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                }
            }

            // Check for collisions between player bullets and enemies
            for (auto& bullet : bullets) {
                // If the bullet is fired by the player and hits an enemy
                for (auto& enemy : enemies) {
                    if (bullet.checkCollision(enemy.shape)) {
                        enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                        bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                        break;  // Exit the inner loop as the bullet has already hit an enemy
                    }
                }
            }

            for (auto it = enemies.begin(); it != enemies.end(); ) {
                if (!it->isAlive()) {
                    player.addCoin();  // Add 1 coin when an enemy is destroyed
                    it = enemies.erase(it);  // Remove enemy from the list
                }
                else {
                    ++it;
                }
            }




            // Reset coins if the player dies
            if (!player.isAlive()) {
                player.resetCoins();  // Reset the coin count on player death
            }

            // Move each enemy towards the player
            for (auto& enemy : enemies) {
                enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                enemy.shootAtPlayer(player.getPosition(), enemyBullets);
            }

        }
        else if (!level3Started && level3Won == true) {
            {
                // DISPLAY VICTORY SCREEN IF PLAYER WINS A LEVEL
                inVictoryScreen = true;
                displayVictoryScreen(window);
                backButton.render(window);

                if (backButton.isClicked(window)) {
                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                    // Reset all flags
                    inGame = false;           
                    inVictoryScreen = false;   
                    inGameMenu = false;        
                    inSettingsMenu = false;    
                    inGarageMenu = false;      

                    // Reset level flags
                    level3Started = false;     // Reset the level started flag
                    level3Won = false;          // Reset the win flag
                    player.reset();            // Reset the player 

                    // Reset the enemies
                    enemies.clear();           // Clear all enemies

                    // Reset the UI elements
                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                }
            }
            }
        else if (!level3Started && !player.isAlive()) {
                 // DISPLAY GAME OVER SCREEN
                inDefeatScreen = true;
                displayGameOverScreen(window);
                backButton.render(window);

                if (backButton.isClicked(window)) {
                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                    // Reset all flags
                    inGame = false;            
                    inVictoryScreen = false;   
                    inGameMenu = false;        
                    inSettingsMenu = false;    
                    inGarageMenu = false;      

                    // Reset level flags
                    level3Started = false;     // Reset the level started flag
                    level3Won = false;          // Reset the win flag
                    player.reset();            // Reset the player

                    // Reset the enemies 
                    enemies.clear();           // Clear all enemies

                    // Reset the UI elements
                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                }


                } else if (level4Started) {

                    // Sets inVictoryScreen and inDefeatScreen to false
                    inVictoryScreen = false;
                    inDefeatScreen = false;

                    // Remove dead enemies safely
                    enemies.erase(
                        std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                            return !enemy.isAlive();  // Remove enemies that are dead
                            }),
                        enemies.end()
                    );

                    // Check if player is dead
                    if (!player.isAlive()) {
                        std::cout << "Game Over! Player has died!" << std::endl;
                        level2Started = false;
                    }

                    if (enemies.empty()) {
                        level4Won = true;  // The player has won the level
                        level4Started = false;
                        std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
                    }

                    // Draw the player box (red box)
                    player.render(window);  // The player's red box


                    player.renderCoins(window, coinTexture, font);


                    // Draw the enemies
                    for (auto& enemy : enemies) {
                        enemy.render(window);  // Render each enemy from the enemies vector
                    }


                    // Update player bullets and remove out-of-bounds ones
                    for (auto it = bullets.begin(); it != bullets.end(); ) {
                        it->update();  // Update the bullet's position

                        // Check if the bullet is off-screen (out of bounds)
                        if (it->isOutOfBounds(width)) {
                            it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                        }
                        else {
                            ++it;  // Move to the next bullet
                        }
                    }

                    // Update enemy bullets and remove out-of-bounds ones
                    for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                        it->update();  // Update the enemy bullet's position

                        // Check if the bullet is off-screen (out of bounds)
                        if (it->isOutOfBounds(width)) {
                            it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                        }
                        else {
                            ++it;  // Move to the next enemy bullet
                        }
                    }

                    // Render the player bullets
                    for (const auto& bullet : bullets) {
                        window.draw(bullet.shape);
                    }

                    // Render the enemy bullets
                    for (const auto& bullet : enemyBullets) {
                        window.draw(bullet.shape);
                    }


                    // Define the starting position for the health bars and labels
                    sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

                    // Render player health bar and label
                    renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

                    // Adjust the vertical spacing between enemy health bars
                    float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

                    // Render health bars for each enemy dynamically
                    for (size_t i = 0; i < enemies.size(); ++i) {
                        // Adjust the vertical position for each enemy's health bar
                        renderHealthBar(window,
                            sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                            "Enemy " + std::to_string(i + 1),
                            enemies[i].getHealth(),
                            50);
                    }

                    // Handle player and enemy updates here (movement, collision detection, etc.)



                    player.updateMovement();
                    player.updateShooting(bullets);  // This handles shooting and firing cooldown





                    // Check for collisions between enemy bullets and player
                    for (auto& bullet : enemyBullets) {
                        if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                            player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                            bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                        }
                    }

                    // Check for collisions between player bullets and enemies
                    for (auto& bullet : bullets) {
                        // If the bullet is fired by the player and hits an enemy
                        for (auto& enemy : enemies) {
                            if (bullet.checkCollision(enemy.shape)) {
                                enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                                bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                                break;  // Exit the inner loop as the bullet has already hit an enemy
                            }
                        }
                    }

                    for (auto it = enemies.begin(); it != enemies.end(); ) {
                        if (!it->isAlive()) {
                            player.addCoin();  // Add 1 coin when an enemy is destroyed
                            it = enemies.erase(it);  // Remove enemy from the list
                        }
                        else {
                            ++it;
                        }
                    }




                    // Reset coins if the player dies
                    if (!player.isAlive()) {
                        player.resetCoins();  // Reset the coin count on player death
                    }

                    // Move each enemy towards the player
                    for (auto& enemy : enemies) {
                        enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                        enemy.shootAtPlayer(player.getPosition(), enemyBullets);
                    }

                }
                else if (!level4Started && level4Won == true) {
                    {
                        // DISPLAY VICTORY SCREEN IF PLAYER WINS
                        inVictoryScreen = true;
                        displayVictoryScreen(window);
                        backButton.render(window);

                        if (backButton.isClicked(window)) {
                            std::cout << "Back to Main Menu button clicked!" << std::endl;

                            // Reset all flags
                            inGame = false;            
                            inVictoryScreen = false;   
                            inGameMenu = false;        
                            inSettingsMenu = false;    
                            inGarageMenu = false;      

                            // Reset level flags
                            level4Started = false;     // Reset the level started flag
                            level4Won = false;          // Reset the win flag
                            player.reset();            // Reset the player 

                            // Reset the enemies 
                            enemies.clear();           // Clear all enemies

                            // Reset the UI elements
                            renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                        }
                    }
                    } else if (!level4Started && !player.isAlive()) {

                        inDefeatScreen = true;
                        displayGameOverScreen(window);
                        backButton.render(window);

                        if (backButton.isClicked(window)) {
                            std::cout << "Back to Main Menu button clicked!" << std::endl;

                            // Reset all flags
                            inGame = false;            
                            inVictoryScreen = false;   
                            inGameMenu = false;        
                            inSettingsMenu = false;    
                            inGarageMenu = false;      

                            // Reset level flags
                            level4Started = false;     
                            level4Won = false;          
                            player.reset();            

                            // Reset the enemies 
                            enemies.clear();     // Clear all enemies

                            // Reset the UI elements
                            renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                        }


                        } else if (level5Started) {

                            inVictoryScreen = false;
                            inDefeatScreen = false;

                            // Remove dead enemies safely
                            enemies.erase(
                                std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                                    return !enemy.isAlive();  // Remove enemies that are dead
                                    }),
                                enemies.end()
                            );

                            // Check if player is dead
                            if (!player.isAlive()) {
                                std::cout << "Game Over! Player has died!" << std::endl;
                                level5Started = false;
                            }

                            if (enemies.empty()) {
                                level5Won = true;  // The player has won the level
                                level5Started = false;
                                std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
                            }

                            // Draw the player box (red box)
                            player.render(window);  // The player's red box


                            player.renderCoins(window, coinTexture, font);


                            // Draw the enemies
                            for (auto& enemy : enemies) {
                                enemy.render(window);  // Render each enemy from the enemies vector
                            }


                            // Update player bullets and remove out-of-bounds ones
                            for (auto it = bullets.begin(); it != bullets.end(); ) {
                                it->update();  // Update the bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next bullet
                                }
                            }

                            // Update enemy bullets and remove out-of-bounds ones
                            for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                                it->update();  // Update the enemy bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next enemy bullet
                                }
                            }

                            // Render the player bullets
                            for (const auto& bullet : bullets) {
                                window.draw(bullet.shape);
                            }

                            // Render the enemy bullets
                            for (const auto& bullet : enemyBullets) {
                                window.draw(bullet.shape);
                            }


                            // Define the starting position for the health bars and labels
                            sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

                            // Render player health bar and label
                            renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

                            // Adjust the vertical spacing between enemy health bars
                            float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

                            // Render health bars for each enemy dynamically
                            for (size_t i = 0; i < enemies.size(); ++i) {
                                // Adjust the vertical position for each enemy's health bar
                                renderHealthBar(window,
                                    sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                                    "Enemy " + std::to_string(i + 1),
                                    enemies[i].getHealth(),
                                    50);
                            }

                            // Handle player and enemy updates here (movement, collision detection, etc.)



                            player.updateMovement();
                            player.updateShooting(bullets);  // This handles shooting and firing cooldown





                            // Check for collisions between enemy bullets and player
                            for (auto& bullet : enemyBullets) {
                                if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                                    player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                                    bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                                }
                            }

                            // Check for collisions between player bullets and enemies
                            for (auto& bullet : bullets) {
                                // If the bullet is fired by the player and hits an enemy
                                for (auto& enemy : enemies) {
                                    if (bullet.checkCollision(enemy.shape)) {
                                        enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                                        bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                                        break;  // Exit the inner loop as the bullet has already hit an enemy
                                    }
                                }
                            }

                            for (auto it = enemies.begin(); it != enemies.end(); ) {
                                if (!it->isAlive()) {
                                    player.addCoin();  // Add 1 coin when an enemy is destroyed
                                    it = enemies.erase(it);  // Remove enemy from the list
                                }
                                else {
                                    ++it;
                                }
                            }




                            // Reset coins if the player dies
                            if (!player.isAlive()) {
                                player.resetCoins();  // Reset the coin count on player death
                            }

                            // Move each enemy towards the player
                            for (auto& enemy : enemies) {
                                enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                                enemy.shootAtPlayer(player.getPosition(), enemyBullets);
                            }

                        }
                        else if (!level5Started && level5Won == true) {
                            {

                                inVictoryScreen = true;
                                displayVictoryScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;            
                                    inVictoryScreen = false;   
                                    inGameMenu = false;        
                                    inSettingsMenu = false;    
                                    inGarageMenu = false;      

                                    // Reset level flags
                                    level5Started = false;     // Reset the level started flag
                                    level5Won = false;          // Reset the win flag
                                    player.reset();            // Reset the player 

                                    // Reset the enemies
                                    enemies.clear();           // Clear all enemies

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }
                            }
                        }
                        else if (!level5Started && !player.isAlive()) {

                            inDefeatScreen = true;
                            displayGameOverScreen(window);
                            backButton.render(window);

                            if (backButton.isClicked(window)) {
                                std::cout << "Back to Main Menu button clicked!" << std::endl;

                                // Reset all flags
                                inGame = false;            
                                inVictoryScreen = false;   
                                inGameMenu = false;        
                                inSettingsMenu = false;    
                                inGarageMenu = false;      

                                // Reset level flags
                                level5Started = false;     
                                level5Won = false;          
                                player.reset();            

                                // Reset the enemies 
                                enemies.clear();           // Clear all enemies

                                // Reset the UI elements
                                renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                            }

                            
                        }
                        else if (level6Started) {

                            inVictoryScreen = false;
                            inDefeatScreen = false;

                            // Remove dead enemies safely
                            enemies.erase(
                                std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                                    return !enemy.isAlive();  // Remove enemies that are dead
                                    }),
                                enemies.end()
                            );

                            // Check if player is dead
                            if (!player.isAlive()) {
                                std::cout << "Game Over! Player has died!" << std::endl;
                                level6Started = false;
                            }

                            if (enemies.empty()) {
                                level6Won = true;  // The player has won the level
                                level6Started = false;
                                std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
                            }

                            // Draw the player box (red box)
                            player.render(window);  // The player's red box


                            player.renderCoins(window, coinTexture, font);


                            // Draw the enemies
                            for (auto& enemy : enemies) {
                                enemy.render(window);  // Render each enemy from the enemies vector
                            }


                            // Update player bullets and remove out-of-bounds ones
                            for (auto it = bullets.begin(); it != bullets.end(); ) {
                                it->update();  // Update the bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next bullet
                                }
                            }

                            // Update enemy bullets and remove out-of-bounds ones
                            for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                                it->update();  // Update the enemy bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next enemy bullet
                                }
                            }

                            // Render the player bullets
                            for (const auto& bullet : bullets) {
                                window.draw(bullet.shape);
                            }

                            // Render the enemy bullets
                            for (const auto& bullet : enemyBullets) {
                                window.draw(bullet.shape);
                            }


                            // Define the starting position for the health bars and labels
                            sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

                            // Render player health bar and label
                            renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

                            // Adjust the vertical spacing between enemy health bars
                            float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

                            // Render health bars for each enemy dynamically
                            for (size_t i = 0; i < enemies.size(); ++i) {
                                // Adjust the vertical position for each enemy's health bar
                                renderHealthBar(window,
                                    sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                                    "Enemy " + std::to_string(i + 1),
                                    enemies[i].getHealth(),
                                    50);
                            }

                            // Handle player and enemy updates here (movement, collision detection, etc.)



                            player.updateMovement();
                            player.updateShooting(bullets);  // This handles shooting and firing cooldown





                            // Check for collisions between enemy bullets and player
                            for (auto& bullet : enemyBullets) {
                                if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                                    player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                                    bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                                }
                            }

                            // Check for collisions between player bullets and enemies
                            for (auto& bullet : bullets) {
                                // If the bullet is fired by the player and hits an enemy
                                for (auto& enemy : enemies) {
                                    if (bullet.checkCollision(enemy.shape)) {
                                        enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                                        bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                                        break;  // Exit the inner loop as the bullet has already hit an enemy
                                    }
                                }
                            }

                            for (auto it = enemies.begin(); it != enemies.end(); ) {
                                if (!it->isAlive()) {
                                    player.addCoin();  // Add 1 coin when an enemy is destroyed
                                    it = enemies.erase(it);  // Remove enemy from the list
                                }
                                else {
                                    ++it;
                                }
                            }




                            // Reset coins if the player dies
                            if (!player.isAlive()) {
                                player.resetCoins();  // Reset the coin count on player death
                            }

                            // Move each enemy towards the player
                            for (auto& enemy : enemies) {
                                enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                                enemy.shootAtPlayer(player.getPosition(), enemyBullets);
                            }

                            }
                        else if (!level6Started && level6Won == true) {
                            {

                                inVictoryScreen = true;
                                displayVictoryScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;            
                                    inVictoryScreen = false;   
                                    inGameMenu = false;        
                                    inSettingsMenu = false;    
                                    inGarageMenu = false;      

                                    // Reset level flags
                                    level6Started = false;     
                                    level6Won = false;          
                                    player.reset();            

                                    // Reset the enemies
                                    enemies.clear();   // Clear all enemies

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }
                            }
                       }
                        else if (!level6Started && !player.isAlive()) {

                            inDefeatScreen = true;
                            displayGameOverScreen(window);
                            backButton.render(window);

                            if (backButton.isClicked(window)) {
                                std::cout << "Back to Main Menu button clicked!" << std::endl;

                                // Reset all flags
                                inGame = false;            
                                inVictoryScreen = false;   
                                inGameMenu = false;        
                                inSettingsMenu = false;    
                                inGarageMenu = false;      

                                // Reset level flags
                                level6Started = false;     // Reset the level started flag
                                level6Won = false;          // Reset the win flag
                                player.reset();            // Reset the player 

                                // Reset the enemies (if needed)
                                enemies.clear();           // Clear all enemies

                                // Reset the UI elements
                                renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                            }

                        }
                        else if (level7Started) {

                            inVictoryScreen = false;
                            inDefeatScreen = false;

                            // Remove dead enemies safely
                            enemies.erase(
                                std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                                    return !enemy.isAlive();  // Remove enemies that are dead
                                    }),
                                enemies.end()
                            );

                            // Check if player is dead
                            if (!player.isAlive()) {
                                std::cout << "Game Over! Player has died!" << std::endl;
                                level7Started = false;
                            }

                            if (enemies.empty()) {
                                level7Won = true;  // The player has won the level
                                level7Started = false;
                                std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
                            }

                            // Draw the player box (red box)
                            player.render(window);  // The player's red box


                            player.renderCoins(window, coinTexture, font);


                            // Draw the enemies
                            for (auto& enemy : enemies) {
                                enemy.render(window);  // Render each enemy from the enemies vector
                            }


                            // Update player bullets and remove out-of-bounds ones
                            for (auto it = bullets.begin(); it != bullets.end(); ) {
                                it->update();  // Update the bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next bullet
                                }
                            }

                            // Update enemy bullets and remove out-of-bounds ones
                            for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                                it->update();  // Update the enemy bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next enemy bullet
                                }
                            }

                            // Render the player bullets
                            for (const auto& bullet : bullets) {
                                window.draw(bullet.shape);
                            }

                            // Render the enemy bullets
                            for (const auto& bullet : enemyBullets) {
                                window.draw(bullet.shape);
                            }


                            // Define the starting position for the health bars and labels
                            sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

                            // Render player health bar and label
                            renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

                            // Adjust the vertical spacing between enemy health bars
                            float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

                            // Render health bars for each enemy dynamically
                            for (size_t i = 0; i < enemies.size(); ++i) {
                                // Adjust the vertical position for each enemy's health bar
                                renderHealthBar(window,
                                    sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                                    "Enemy " + std::to_string(i + 1),
                                    enemies[i].getHealth(),
                                    50);
                            }

                            // Handle player and enemy updates here (movement, collision detection, etc.)



                            player.updateMovement();
                            player.updateShooting(bullets);  // This handles shooting and firing cooldown





                            // Check for collisions between enemy bullets and player
                            for (auto& bullet : enemyBullets) {
                                if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                                    player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                                    bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                                }
                            }

                            // Check for collisions between player bullets and enemies
                            for (auto& bullet : bullets) {
                                // If the bullet is fired by the player and hits an enemy
                                for (auto& enemy : enemies) {
                                    if (bullet.checkCollision(enemy.shape)) {
                                        enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                                        bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                                        break;  // Exit the inner loop as the bullet has already hit an enemy
                                    }
                                }
                            }

                            for (auto it = enemies.begin(); it != enemies.end(); ) {
                                if (!it->isAlive()) {
                                    player.addCoin();  // Add 1 coin when an enemy is destroyed
                                    it = enemies.erase(it);  // Remove enemy from the list
                                }
                                else {
                                    ++it;
                                }
                            }




                            // Reset coins if the player dies
                            if (!player.isAlive()) {
                                player.resetCoins();  // Reset the coin count on player death
                            }

                            // Move each enemy towards the player
                            for (auto& enemy : enemies) {
                                enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                                enemy.shootAtPlayer(player.getPosition(), enemyBullets);
                            }

                            }
                        else if (!level7Started && level7Won == true) {
                            {

                                inVictoryScreen = true;
                                displayVictoryScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;            // Ensure the game isn't active
                                    inVictoryScreen = false;   // Ensure victory screen is hidden
                                    inGameMenu = false;        // Ensure we're not in the game menu
                                    inSettingsMenu = false;    // Ensure settings menu is closed
                                    inGarageMenu = false;      // Ensure garage menu is closed

                                    // Reset level flags
                                    level7Started = false;     // Reset the level started flag
                                    level7Won = false;          // Reset the win flag
                                    player.reset();            // Reset the player (if needed)

                                    // Reset the enemies (if needed)
                                    enemies.clear();           // Clear all enemies

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }
                            }
                            }
                        else if (!level7Started && !player.isAlive()) {

                                inDefeatScreen = true;
                                displayGameOverScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;            // Ensure the game isn't active
                                    inVictoryScreen = false;   // Ensure victory screen is hidden
                                    inGameMenu = false;        // Ensure we're not in the game menu
                                    inSettingsMenu = false;    // Ensure settings menu is closed
                                    inGarageMenu = false;      // Ensure garage menu is closed

                                    // Reset level flags
                                    level7Started = false;     // Reset the level started flag
                                    level7Won = false;          // Reset the win flag
                                    player.reset();            // Reset the player (if needed)

                                    // Reset the enemies (if needed)
                                    enemies.clear();           // Clear all enemies

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }

                                }
                        else if (level8Started) {

                            inVictoryScreen = false;
                            inDefeatScreen = false;

                            // Remove dead enemies safely
                            enemies.erase(
                                std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                                    return !enemy.isAlive();  // Remove enemies that are dead
                                    }),
                                enemies.end()
                            );

                            // Check if player is dead
                            if (!player.isAlive()) {
                                std::cout << "Game Over! Player has died!" << std::endl;
                                level8Started = false;
                            }

                            if (enemies.empty()) {
                                level8Won = true;  // The player has won the level
                                level8Started = false;
                                std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
                            }

                            // Draw the player box (red box)
                            player.render(window);  // The player's red box


                            player.renderCoins(window, coinTexture, font);


                            // Draw the enemies
                            for (auto& enemy : enemies) {
                                enemy.render(window);  // Render each enemy from the enemies vector
                            }


                            // Update player bullets and remove out-of-bounds ones
                            for (auto it = bullets.begin(); it != bullets.end(); ) {
                                it->update();  // Update the bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next bullet
                                }
                            }

                            // Update enemy bullets and remove out-of-bounds ones
                            for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                                it->update();  // Update the enemy bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next enemy bullet
                                }
                            }

                            // Render the player bullets
                            for (const auto& bullet : bullets) {
                                window.draw(bullet.shape);
                            }

                            // Render the enemy bullets
                            for (const auto& bullet : enemyBullets) {
                                window.draw(bullet.shape);
                            }


                            // Define the starting position for the health bars and labels
                            sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

                            // Render player health bar and label
                            renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

                            // Adjust the vertical spacing between enemy health bars
                            float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

                            // Render health bars for each enemy dynamically
                            for (size_t i = 0; i < enemies.size(); ++i) {
                                // Adjust the vertical position for each enemy's health bar
                                renderHealthBar(window,
                                    sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                                    "Enemy " + std::to_string(i + 1),
                                    enemies[i].getHealth(),
                                    50);
                            }

                            // Handle player and enemy updates here (movement, collision detection, etc.)



                            player.updateMovement();
                            player.updateShooting(bullets);  // This handles shooting and firing cooldown





                            // Check for collisions between enemy bullets and player
                            for (auto& bullet : enemyBullets) {
                                if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                                    player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                                    bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                                }
                            }

                            // Check for collisions between player bullets and enemies
                            for (auto& bullet : bullets) {
                                // If the bullet is fired by the player and hits an enemy
                                for (auto& enemy : enemies) {
                                    if (bullet.checkCollision(enemy.shape)) {
                                        enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                                        bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                                        break;  // Exit the inner loop as the bullet has already hit an enemy
                                    }
                                }
                            }

                            for (auto it = enemies.begin(); it != enemies.end(); ) {
                                if (!it->isAlive()) {
                                    player.addCoin();  // Add 1 coin when an enemy is destroyed
                                    it = enemies.erase(it);  // Remove enemy from the list
                                }
                                else {
                                    ++it;
                                }
                            }




                            // Reset coins if the player dies
                            if (!player.isAlive()) {
                                player.resetCoins();  // Reset the coin count on player death
                            }

                            // Move each enemy towards the player
                            for (auto& enemy : enemies) {
                                enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                                enemy.shootAtPlayer(player.getPosition(), enemyBullets);
                            }

                            }
                        else if (!level8Started && level8Won == true) {
                            {

                                inVictoryScreen = true;
                                displayVictoryScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;            // Ensure the game isn't active
                                    inVictoryScreen = false;   // Ensure victory screen is hidden
                                    inGameMenu = false;        // Ensure we're not in the game menu
                                    inSettingsMenu = false;    // Ensure settings menu is closed
                                    inGarageMenu = false;      // Ensure garage menu is closed

                                    // Reset level flags
                                    level8Started = false;     // Reset the level started flag
                                    level8Won = false;          // Reset the win flag
                                    player.reset();            // Reset the player (if needed)

                                    // Reset the enemies (if needed)
                                    enemies.clear();           // Clear all enemies

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }
                            }
                            }
                        else if (!level8Started && !player.isAlive()) {

                                inDefeatScreen = true;
                                displayGameOverScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;            // Ensure the game isn't active
                                    inVictoryScreen = false;   // Ensure victory screen is hidden
                                    inGameMenu = false;        // Ensure we're not in the game menu
                                    inSettingsMenu = false;    // Ensure settings menu is closed
                                    inGarageMenu = false;      // Ensure garage menu is closed

                                    // Reset level flags
                                    level8Started = false;     // Reset the level started flag
                                    level8Won = false;          // Reset the win flag
                                    player.reset();            // Reset the player (if needed)

                                    // Reset the enemies (if needed)
                                    enemies.clear();           // Clear all enemies

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }

                                }
                        else if (level9Started) {

                            inVictoryScreen = false;
                            inDefeatScreen = false;

                            // Remove dead enemies safely
                            enemies.erase(
                                std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                                    return !enemy.isAlive();  // Remove enemies that are dead
                                    }),
                                enemies.end()
                            );

                            // Check if player is dead
                            if (!player.isAlive()) {
                                std::cout << "Game Over! Player has died!" << std::endl;
                                level6Started = false;
                            }

                            if (enemies.empty()) {
                                level9Won = true;  // The player has won the level
                                level9Started = false;
                                std::cout << "Congratulations! You've defeated all enemies!" << std::endl;
                            }

                            // Draw the player box (red box)
                            player.render(window);  // The player's red box


                            player.renderCoins(window, coinTexture, font);


                            // Draw the enemies
                            for (auto& enemy : enemies) {
                                enemy.render(window);  // Render each enemy from the enemies vector
                            }


                            // Update player bullets and remove out-of-bounds ones
                            for (auto it = bullets.begin(); it != bullets.end(); ) {
                                it->update();  // Update the bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next bullet
                                }
                            }

                            // Update enemy bullets and remove out-of-bounds ones
                            for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                                it->update();  // Update the enemy bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next enemy bullet
                                }
                            }

                            // Render the player bullets
                            for (const auto& bullet : bullets) {
                                window.draw(bullet.shape);
                            }

                            // Render the enemy bullets
                            for (const auto& bullet : enemyBullets) {
                                window.draw(bullet.shape);
                            }


                            // Define the starting position for the health bars and labels
                            sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

                            // Render player health bar and label
                            renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

                            // Adjust the vertical spacing between enemy health bars
                            float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

                            // Render health bars for each enemy dynamically
                            for (size_t i = 0; i < enemies.size(); ++i) {
                                // Adjust the vertical position for each enemy's health bar
                                renderHealthBar(window,
                                    sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                                    "Enemy " + std::to_string(i + 1),
                                    enemies[i].getHealth(),
                                    50);
                            }

                            // Handle player and enemy updates here (movement, collision detection, etc.)



                            player.updateMovement();
                            player.updateShooting(bullets);  // This handles shooting and firing cooldown





                            // Check for collisions between enemy bullets and player
                            for (auto& bullet : enemyBullets) {
                                if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                                    player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                                    bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                                }
                            }

                            // Check for collisions between player bullets and enemies
                            for (auto& bullet : bullets) {
                                // If the bullet is fired by the player and hits an enemy
                                for (auto& enemy : enemies) {
                                    if (bullet.checkCollision(enemy.shape)) {
                                        enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                                        bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                                        break;  // Exit the inner loop as the bullet has already hit an enemy
                                    }
                                }
                            }

                            for (auto it = enemies.begin(); it != enemies.end(); ) {
                                if (!it->isAlive()) {
                                    player.addCoin();  // Add 1 coin when an enemy is destroyed
                                    it = enemies.erase(it);  // Remove enemy from the list
                                }
                                else {
                                    ++it;
                                }
                            }




                            // Reset coins if the player dies
                            if (!player.isAlive()) {
                                player.resetCoins();  // Reset the coin count on player death
                            }

                            // Move each enemy towards the player
                            for (auto& enemy : enemies) {
                                enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                                enemy.shootAtPlayer(player.getPosition(), enemyBullets);
                            }

                            }
                        else if (!level9Started && level9Won == true) {
                            {

                                inVictoryScreen = true;
                                displayVictoryScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;            
                                    inVictoryScreen = false;   
                                    inGameMenu = false;        
                                    inSettingsMenu = false;    
                                    inGarageMenu = false;      

                                    // Reset level flags
                                    level9Started = false;   
                                    level9Won = false;         
                                    player.reset();            

                                    // Reset the enemies 
                                    enemies.clear();  

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }
                            }
                            }
                        else if (!level9Started && !player.isAlive()) {

                                inDefeatScreen = true;
                                displayGameOverScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;            
                                    inVictoryScreen = false;   
                                    inGameMenu = false;        
                                    inSettingsMenu = false;    
                                    inGarageMenu = false;      

                                    // Reset level flags
                                    level9Started = false;     
                                    level9Won = false;          
                                    player.reset();          

                                    // Reset the enemies 
                                    enemies.clear(); 

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }

                                }
                        else if (level10Started) {

                            inVictoryScreen = false;
                            inDefeatScreen = false;

                            // Remove dead enemies safely
                            enemies.erase(
                                std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                                    return !enemy.isAlive();  // Remove enemies that are dead
                                    }),
                                enemies.end()
                            );

                            // Check if player is dead
                            if (!player.isAlive()) {
                                std::cout << "Game Over! Player has died!" << std::endl;
                                level10Started = false;
                            }

                            if (enemies.empty()) {
                                level10Won = true;  // The player has won the level
                                level10Started = false;
                                std::cout << "Congratulations! You've defeated all enemies! And Won the game!" << std::endl;
                            }

                            // Draw the player box (red box)
                            player.render(window);  // The player's red box


                            player.renderCoins(window, coinTexture, font);


                            // Draw the enemies
                            for (auto& enemy : enemies) {
                                enemy.render(window);  // Render each enemy from the enemies vector
                            }


                            // Update player bullets and remove out-of-bounds ones
                            for (auto it = bullets.begin(); it != bullets.end(); ) {
                                it->update();  // Update the bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = bullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next bullet
                                }
                            }

                            // Update enemy bullets and remove out-of-bounds ones
                            for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
                                it->update();  // Update the enemy bullet's position

                                // Check if the bullet is off-screen (out of bounds)
                                if (it->isOutOfBounds(width)) {
                                    it = enemyBullets.erase(it);  // Remove the bullet if it's out of bounds
                                }
                                else {
                                    ++it;  // Move to the next enemy bullet
                                }
                            }

                            // Render the player bullets
                            for (const auto& bullet : bullets) {
                                window.draw(bullet.shape);
                            }

                            // Render the enemy bullets
                            for (const auto& bullet : enemyBullets) {
                                window.draw(bullet.shape);
                            }


                            // Define the starting position for the health bars and labels
                            sf::Vector2f healthBarPosition(20.f, height - 120.f);  // Starting position in bottom-left corner

                            // Render player health bar and label
                            renderHealthBar(window, healthBarPosition, "Player", player.getHealth(), 100);  // Assuming player's health is 50 out of 50

                            // Adjust the vertical spacing between enemy health bars
                            float enemyHealthBarSpacing = 60.f;  // Vertical space between each enemy's health bar

                            // Render health bars for each enemy dynamically
                            for (size_t i = 0; i < enemies.size(); ++i) {
                                // Adjust the vertical position for each enemy's health bar
                                renderHealthBar(window,
                                    sf::Vector2f(healthBarPosition.x, healthBarPosition.y + (i + 1) * enemyHealthBarSpacing),
                                    "Enemy " + std::to_string(i + 1),
                                    enemies[i].getHealth(),
                                    50);
                            }

                            // Handle player and enemy updates here (movement, collision detection, etc.)



                            player.updateMovement();
                            player.updateShooting(bullets);  // This handles shooting and firing cooldown





                            // Check for collisions between enemy bullets and player
                            for (auto& bullet : enemyBullets) {
                                if (bullet.shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                                    player.takeDamage(bullet.getDamage());  // Player takes damage from enemy bullet
                                    bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen (move off-screen)
                                }
                            }

                            // Check for collisions between player bullets and enemies
                            for (auto& bullet : bullets) {
                                // If the bullet is fired by the player and hits an enemy
                                for (auto& enemy : enemies) {
                                    if (bullet.checkCollision(enemy.shape)) {
                                        enemy.takeDamage(bullet.getDamage());  // Enemy takes damage from player bullet
                                        bullet.shape.setPosition(-100.f, -100.f);  // Remove bullet from screen
                                        break;  // Exit the inner loop as the bullet has already hit an enemy
                                    }
                                }
                            }

                            for (auto it = enemies.begin(); it != enemies.end(); ) {
                                if (!it->isAlive()) {
                                    player.addCoin();  // Add 1 coin when an enemy is destroyed
                                    it = enemies.erase(it);  // Remove enemy from the list
                                }
                                else {
                                    ++it;
                                }
                            }




                            // Reset coins if the player dies
                            if (!player.isAlive()) {
                                player.resetCoins();  // Reset the coin count on player death
                            }

                            // Move each enemy towards the player
                            for (auto& enemy : enemies) {
                                enemy.moveTowardsPlayer(player.getPosition(), player.getSpeed(), enemies);

                                enemy.shootAtPlayer(player.getPosition(), enemyBullets);
                            }

                            }
                        else if (!level9Started && level9Won == true) {
                            {

                                inVictoryScreen = true;
                                displayVictoryScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;
                                    inVictoryScreen = false;
                                    inGameMenu = false;
                                    inSettingsMenu = false;
                                    inGarageMenu = false;

                                    // Reset level flags
                                    level10Started = false;
                                    level10Won = false;
                                    player.reset();

                                    // Reset the enemies 
                                    enemies.clear();

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }
                            }
                            }
                        else if (!level10Started && !player.isAlive()) {

                                inDefeatScreen = true;
                                displayGameOverScreen(window);
                                backButton.render(window);

                                if (backButton.isClicked(window)) {
                                    std::cout << "Back to Main Menu button clicked!" << std::endl;

                                    // Reset all flags
                                    inGame = false;
                                    inVictoryScreen = false;
                                    inGameMenu = false;
                                    inSettingsMenu = false;
                                    inGarageMenu = false;

                                    // Reset level flags
                                    level10Started = false;
                                    level10Won = false;
                                    player.reset();

                                    // Reset the enemies 
                                    enemies.clear();

                                    // Reset the UI elements
                                    renderMainMenu(window, startButton, settingsButton, garageButton, exitButton);    // Ensure the main menu is rendered
                                }

                                }

        window.display();  // Display the window contents
    }
   
    
} 

