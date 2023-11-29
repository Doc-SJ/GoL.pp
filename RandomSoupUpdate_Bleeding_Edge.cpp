#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()

// Constants for the window size and cell size.
const int WIDTH = 800;
const int HEIGHT = 600;
const int CELL_SIZE = 8;
const int GRID_WIDTH = WIDTH / CELL_SIZE;
const int GRID_HEIGHT = HEIGHT / CELL_SIZE;

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

// Function to count the number of living neighbors around a cell.
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

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WIDTH, HEIGHT)), "Conway's Game of Life");
    window.setFramerateLimit(60);

    std::vector<std::vector<bool>> grid(GRID_WIDTH, std::vector<bool>(GRID_HEIGHT, false));
    bool simulationRunning = false;

    // Game loop: Handle events, update the state, and render the grid.
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Toggle simulation running state with the spacebar
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    simulationRunning = !simulationRunning;
                } else if (event.key.code == sf::Keyboard::Right && !simulationRunning) {
                    updateGrid(grid);  // Update the grid for one iteration
                }
            }

            // Toggle cell state with mouse button press
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Check if the mouse click is within the triangle button's bounds
                if (mousePos.x >= WIDTH - 30 && mousePos.x <= WIDTH - 10 && mousePos.y >= 10 && mousePos.y <= 30) {
                    createRandomSoup(grid);  // Create a random soup
                } else {
                    sf::Vector2i pos = getMousePositionInGrid(window);
                    pos.x = wrapCoordinate(pos.x, GRID_WIDTH);
                    pos.y = wrapCoordinate(pos.y, GRID_HEIGHT);
                    grid[pos.x][pos.y] = !grid[pos.x][pos.y];
                }
            }
        }

        // Render the cells on the window.
        window.clear(sf::Color::Black);
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE));
                cell.setFillColor(grid[x][y] ? sf::Color::White : sf::Color::Black);
                window.draw(cell);
            }
        }

        // Draw the triangle button for creating random soup
        sf::ConvexShape triangle;
        triangle.setPointCount(3);
        triangle.setPoint(0, sf::Vector2f(WIDTH - 30, 30));   // Top-right corner
        triangle.setPoint(1, sf::Vector2f(WIDTH - 10, 10));   // Top-right corner, 20 pixels left
        triangle.setPoint(2, sf::Vector2f(WIDTH - 10, 30));   // 20 pixels below the second point
        triangle.setFillColor(sf::Color::Red);  // Set triangle color
        window.draw(triangle);

        window.display();

        // Continuously update the grid state if the simulation is running.
        if (simulationRunning) {
            updateGrid(grid);
        }
    }

    return 0;
}
