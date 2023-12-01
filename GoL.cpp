#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <stack>

// Gotta' define 'dem buttons, ya feel me?
std::vector<sf::ConvexShape> defineButtonShapes();
bool handleButtonPress(const sf::Vector2i& mousePos, std::vector<std::vector<bool>>& grid, bool& simulationRunning);
void clearGrid(std::vector<std::vector<bool>>& grid);
void createRandomSoup(std::vector<std::vector<bool>>& grid);
bool rightKeyPressed = false;
bool leftKeyPressed = false;

// Constants for the window size and cell size.
const int WIDTH = 800;
const int HEIGHT = 600;
const int CELL_SIZE = 8;
const int GRID_WIDTH = WIDTH / CELL_SIZE;
const int GRID_HEIGHT = HEIGHT / CELL_SIZE;

// Define constants for button dimensions and positions
const int BUTTON_WIDTH = 20;
const int BUTTON_HEIGHT = 20;
const int RANDOM_SOUP_BUTTON_X = WIDTH - 60;
const int RANDOM_SOUP_BUTTON_Y = 10;
const int CLEAR_BUTTON_X = WIDTH - 30;
const int CLEAR_BUTTON_Y = 10;

// Function to Draw the Buttons
std::vector<sf::ConvexShape> defineButtonShapes() {
    std::vector<sf::ConvexShape> buttons;

    // Random Soup Button
    sf::ConvexShape randomSoupButton;
    randomSoupButton.setPointCount(4);
    randomSoupButton.setPoint(0, sf::Vector2f(RANDOM_SOUP_BUTTON_X, RANDOM_SOUP_BUTTON_Y));
    randomSoupButton.setPoint(1, sf::Vector2f(RANDOM_SOUP_BUTTON_X + BUTTON_WIDTH, RANDOM_SOUP_BUTTON_Y));
    randomSoupButton.setPoint(2, sf::Vector2f(RANDOM_SOUP_BUTTON_X + BUTTON_WIDTH, RANDOM_SOUP_BUTTON_Y + BUTTON_HEIGHT));
    randomSoupButton.setPoint(3, sf::Vector2f(RANDOM_SOUP_BUTTON_X, RANDOM_SOUP_BUTTON_Y + BUTTON_HEIGHT));
    randomSoupButton.setFillColor(sf::Color::Blue);
    buttons.push_back(randomSoupButton);

    // Clear Button
    sf::ConvexShape clearButton;
    clearButton.setPointCount(4);
    clearButton.setPoint(0, sf::Vector2f(CLEAR_BUTTON_X, CLEAR_BUTTON_Y));
    clearButton.setPoint(1, sf::Vector2f(CLEAR_BUTTON_X + BUTTON_WIDTH, CLEAR_BUTTON_Y));
    clearButton.setPoint(2, sf::Vector2f(CLEAR_BUTTON_X + BUTTON_WIDTH, CLEAR_BUTTON_Y + BUTTON_HEIGHT));
    clearButton.setPoint(3, sf::Vector2f(CLEAR_BUTTON_X, CLEAR_BUTTON_Y + BUTTON_HEIGHT));
    clearButton.setFillColor(sf::Color::Red);
    buttons.push_back(clearButton);

    return buttons;
}

// Function to convert the mouse position to grid coordinates.
sf::Vector2i getMousePositionInGrid(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    return sf::Vector2i(mousePos.x / CELL_SIZE, mousePos.y / CELL_SIZE);
}

// Function to wrap the grid coordinates for toroidal topology.
int wrapCoordinate(int coordinate, int max) {
    if (coordinate < 0) return max - 1;
    if (coordinate >= max) return 0;
    return coordinate;
}

// Function to count the number of living neighbours around a cell.
int countNeighbours(const std::vector<std::vector<bool>>& grid, int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int nx = wrapCoordinate(x + i, GRID_WIDTH);
            int ny = wrapCoordinate(y + j, GRID_HEIGHT);
            if (grid[nx][ny]) count++;
        }
    }
    return count;
}

// Function to update the grid state based on Conway's Game of Life rules.
void updateGrid(std::vector<std::vector<bool>>& grid) {
    std::vector<std::vector<bool>> newGrid = grid;
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            int neighbours = countNeighbours(grid, x, y);
            if (grid[x][y]) {
                if (neighbours < 2 || neighbours > 3) newGrid[x][y] = false;
            } else {
                if (neighbours == 3) newGrid[x][y] = true;
            }
        }
    }
    grid = newGrid;
}

// Function to generate a random soup
void createRandomSoup(std::vector<std::vector<bool>>& grid) {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            grid[x][y] = rand() % 2;  // Randomly set each cell to true or false
        }
    }
}

// Function to clear the grid
void clearGrid(std::vector<std::vector<bool>>& grid) {
    for (int x = 0; x < grid.size(); x++) {
        for (int y = 0; y < grid[x].size(); y++) {
            grid[x][y] = false;  // Set each cell to dead (false)
        }
    }
}

// Function to handle button press
bool handleButtonPress(const sf::Vector2i& mousePos, std::vector<std::vector<bool>>& grid, bool& simulationRunning) {
    // Check for Random Soup button press
    if (mousePos.x >= RANDOM_SOUP_BUTTON_X && mousePos.x <= RANDOM_SOUP_BUTTON_X + BUTTON_WIDTH &&
        mousePos.y >= RANDOM_SOUP_BUTTON_Y && mousePos.y <= RANDOM_SOUP_BUTTON_Y + BUTTON_HEIGHT) {
        createRandomSoup(grid);
        return true; // Indicate a button was pressed
    }

    // Check for Clear button press
    if (mousePos.x >= CLEAR_BUTTON_X && mousePos.x <= CLEAR_BUTTON_X + BUTTON_WIDTH &&
        mousePos.y >= CLEAR_BUTTON_Y && mousePos.y <= CLEAR_BUTTON_Y + BUTTON_HEIGHT) {
        clearGrid(grid);
        return true; // Indicate a button was pressed
    }

    return false; // No button was pressed
}

// Initialise some stuff
std::vector<sf::ConvexShape> defineButtonShapes();
bool handleButtonPress(const sf::Vector2i& mousePos, std::vector<std::vector<bool>>& grid, bool& simulationRunning);
void clearGrid(std::vector<std::vector<bool>>& grid);
void createRandomSoup(std::vector<std::vector<bool>>& grid);
void updateGrid(std::vector<std::vector<bool>>& grid);
sf::Vector2i getMousePositionInGrid(sf::RenderWindow& window);

// New enum class for game states
enum class GameStatus {
    Home,
    Running,
    Exit
};

// Function to draw the home screen
void drawHomeScreen(sf::RenderWindow& homeWindow) {
    sf::RectangleShape startButton(sf::Vector2f(100, 50)); // Start button
    startButton.setPosition(WIDTH / 2 - 150, HEIGHT / 2);  // Center left
    startButton.setFillColor(sf::Color::Green);
    homeWindow.draw(startButton);

    sf::RectangleShape quitButton(sf::Vector2f(100, 50)); // Quit button
    quitButton.setPosition(WIDTH / 2 + 50, HEIGHT / 2);   // Center right
    quitButton.setFillColor(sf::Color::Red);
    homeWindow.draw(quitButton);
}

// Function to handle home screen button presses
GameStatus handleHomeScreenInteraction(const sf::Vector2i& mousePosition) {
    // Check if start button is pressed
    if (mousePosition.x >= (WIDTH / 2 - 150) && mousePosition.x <= (WIDTH / 2 - 50) &&
        mousePosition.y >= (HEIGHT / 2) && mousePosition.y <= (HEIGHT / 2 + 50)) {
        return GameStatus::Running;
    }

    // Check if quit button is pressed
    if (mousePosition.x >= (WIDTH / 2 + 50) && mousePosition.x <= (WIDTH / 2 + 150) &&
        mousePosition.y >= (HEIGHT / 2) && mousePosition.y <= (HEIGHT / 2 + 50)) {
        return GameStatus::Exit;
    }

    return GameStatus::Home;
}

int main() {
    GameStatus currentStatus = GameStatus::Home; // Start with the home screen
    std::stack<std::vector<std::vector<bool>>> gridHistory;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Conway's Game of Life");
    window.setFramerateLimit(60);
    std::vector<std::vector<bool>> grid(GRID_WIDTH, std::vector<bool>(GRID_HEIGHT, false));
    bool simulationRunning = false;
    std::vector<sf::ConvexShape> buttons = defineButtonShapes(); // Initialize button shapes
    sf::Clock clock;
    float timeSinceLastStep = 0.0f;
    
    while (window.isOpen()) {
        sf::Event event;
        float deltaTime = clock.restart().asSeconds();
        timeSinceLastStep += deltaTime;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (currentStatus == GameStatus::Home && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                currentStatus = handleHomeScreenInteraction(mousePos);
            }

            if (currentStatus == GameStatus::Running) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space) {
                        simulationRunning = !simulationRunning;
                    } else if (event.key.code == sf::Keyboard::Right && !rightKeyPressed) {
                        rightKeyPressed = true;
                        gridHistory.push(grid);
                        updateGrid(grid);
                        rightKeyPressed = true;
                    } else if (event.key.code == sf::Keyboard::Left && !leftKeyPressed) {
                        leftKeyPressed = true;
                        if (!gridHistory.empty()) {
                            grid = gridHistory.top();
                            gridHistory.pop();
                            leftKeyPressed = true;
                        }
                    }
                } else if (event.type == sf::Event::KeyReleased) {
                    if (event.key.code == sf::Keyboard::Right) {
                        rightKeyPressed = false;
                    } else if (event.key.code == sf::Keyboard::Left) {
                        leftKeyPressed = false;
                    }
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Handle button presses and determine if a button was clicked
                    bool buttonClicked = handleButtonPress(mousePos, grid, simulationRunning);

                    if (!buttonClicked) {
                        // If no button was clicked, handle cell toggling
                        sf::Vector2i gridPos = getMousePositionInGrid(window);
                        if (gridPos.x >= 0 && gridPos.x < GRID_WIDTH && gridPos.y >= 0 && gridPos.y < GRID_HEIGHT) {
                            grid[gridPos.x][gridPos.y] = !grid[gridPos.x][gridPos.y];
                        }
                    }
                }
            }

            // Additional states go here.
        }

        // Rendering based on the current game state
        window.clear(sf::Color::Black);

        if (currentStatus == GameStatus::Home) {
            drawHomeScreen(window);
        } else if (currentStatus == GameStatus::Running) {
            // Draw the grid cells
            for (int x = 0; x < GRID_WIDTH; x++) {
                for (int y = 0; y < GRID_HEIGHT; y++) {
                    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                    cell.setPosition(sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE));
                    cell.setFillColor(grid[x][y] ? sf::Color::White : sf::Color::Black);
                    window.draw(cell);
                }
            }

            // Draw the buttons
            for (const auto& button : buttons) {
                window.draw(button);
            }

            // Continuously update the grid state if the simulation is running
            if (simulationRunning) {
                gridHistory.push(grid);
                updateGrid(grid);
            }
        }

        window.display();

        // Check if the game should exit
        if (currentStatus == GameStatus::Exit) {
            window.close();
        }
    }

    return 0;
}
