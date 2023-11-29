#include <SFML/Graphics.hpp>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;
const int CELL_SIZE = 10;

sf::Vector2i getMousePositionInGrid(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    return sf::Vector2i(mousePos.x / CELL_SIZE, mousePos.y / CELL_SIZE);
}

bool isValidPosition(int x, int y) {
    return x >= 0 && x < WIDTH / CELL_SIZE && y >= 0 && y < HEIGHT / CELL_SIZE;
}

int countNeighbours(const std::vector<std::vector<bool>>& grid, int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int nx = x + i, ny = y + j;
            if (isValidPosition(nx, ny) && grid[nx][ny])
                count++;
        }
    }
    return count;
}

void updateGrid(std::vector<std::vector<bool>>& grid) {
    std::vector<std::vector<bool>> newGrid = grid;
    for (int x = 0; x < WIDTH / CELL_SIZE; x++) {
        for (int y = 0; y < HEIGHT / CELL_SIZE; y++) {
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

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WIDTH, HEIGHT)), "Conway's Game of Life");
    window.setFramerateLimit(60);

    std::vector<std::vector<bool>> grid(WIDTH / CELL_SIZE, std::vector<bool>(HEIGHT / CELL_SIZE, false));
    bool simulationRunning = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
                simulationRunning = !simulationRunning;

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pos = getMousePositionInGrid(window);
                if (isValidPosition(pos.x, pos.y))
                    grid[pos.x][pos.y] = !grid[pos.x][pos.y];
            }
        }

        window.clear(sf::Color::Black);
        for (int x = 0; x < WIDTH / CELL_SIZE; x++) {
            for (int y = 0; y < HEIGHT / CELL_SIZE; y++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE));
                cell.setFillColor(grid[x][y] ? sf::Color::White : sf::Color::Black);
                window.draw(cell);
            }
        }
        window.display();

        if (simulationRunning) {
            updateGrid(grid);
        }
    }

    return 0;
}
