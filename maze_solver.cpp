#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <utility>
#include <chrono>
#include <thread>
#include <algorithm>

using namespace std;

/**
 * MazeSolver Class
 * Handles solving exactly one Maze using multiple approaches (BFS & DFS).
 * Expected Grid:
 *  0 = Wall
 *  1 = Open Path
 */
class MazeSolver {
private:
    vector<vector<int>> grid;
    int rows;
    int cols;
    pair<int, int> startPos;
    pair<int, int> endPos;
    
    // 4 Directional Movements: Up, Right, Down, Left
    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};

    // Helper to check if a cell is safely walkable
    bool isSafe(int r, int c) const {
        return (r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c] == 1);
    }
    
    // Optional delay for step-by-step traversal visualization
    void delay(int ms = 500) const {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

public:
    MazeSolver(const vector<vector<int>>& grid_input, pair<int, int> start, pair<int, int> end) 
        : grid(grid_input), startPos(start), endPos(end) {
        rows = grid.size();
        cols = grid.empty() ? 0 : grid[0].size();
    }

    /**
     * Print Maze with a given Path Mapping
     * Path is passed as a 2D grid containing specific path markers (e.g., '*' for final path, '.' for explored).
     */
    void printMaze(const vector<vector<char>>& overlayPath) const {
        cout << string(cols * 2 + 3, '-') << "\n";
        for (int r = 0; r < rows; ++r) {
            cout << "| ";
            for (int c = 0; c < cols; ++c) {
                if (make_pair(r, c) == startPos) {
                    cout << "S ";
                } else if (make_pair(r, c) == endPos) {
                    cout << "E ";
                } else if (overlayPath[r][c] != ' ') {
                    // Show path / explored marker
                    cout << overlayPath[r][c] << " ";
                } else if (grid[r][c] == 0) {
                    // Wall
                    cout << "█ ";
                } else {
                    // Empty 
                    cout << "  ";
                }
            }
            cout << "|\n";
        }
        cout << string(cols * 2 + 3, '-') << "\n";
    }

    /**
     * Shows initial setup
     */
    void displaySetup() const {
        cout << "Initial Maze Configuration:\n";
        cout << "S = Start, E = End, █ = Wall, Space = Open Path\n";
        vector<vector<char>> emptyOverlay(rows, vector<char>(cols, ' '));
        printMaze(emptyOverlay);
    }

    /**
     * Depth-First Search (DFS) implementation using backtracking
     * Time Complexity: O(V + E) -> O(rows * cols) in worst case.
     * Space Complexity: O(V) -> O(rows * cols) due to worst-case recursion stack or explicit stack memory.
     */
    void solveDFS(bool debugMode = false) {
        cout << "\n--- Solving using DFS (Backtracking) ---\n";
        if (!isSafe(startPos.first, startPos.second) || !isSafe(endPos.first, endPos.second)) {
            cout << "No path found. Start or End is blocked/invalid.\n";
            return;
        }

        vector<vector<bool>> visited(rows, vector<bool>(cols, false));
        vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(cols, {-1, -1}));
        
        stack<pair<int, int>> st;
        st.push(startPos);
        visited[startPos.first][startPos.second] = true;
        
        bool found = false;
        int steps = 0;

        vector<vector<char>> debugOverlay(rows, vector<char>(cols, ' '));

        while (!st.empty()) {
            auto [r, c] = st.top();
            st.pop();
            steps++;

            if (debugMode) {
                if (make_pair(r, c) != startPos && make_pair(r, c) != endPos) debugOverlay[r][c] = '.';
                cout << "Step " << steps << ": Traversing (" << r << ", " << c << ")\n";
                printMaze(debugOverlay);
                delay(300);
            }

            if (r == endPos.first && c == endPos.second) {
                found = true;
                break;
            }

            // Expanding in 4 directions
            for (int i = 0; i < 4; ++i) {
                int nr = r + dr[i];
                int nc = c + dc[i];

                if (isSafe(nr, nc) && !visited[nr][nc]) {
                    visited[nr][nc] = true;
                    parent[nr][nc] = {r, c};
                    st.push({nr, nc});
                }
            }
        }

        if (found) {
            cout << "Path found in " << steps << " traversal steps!\n";
            reconstructPath(parent, "DFS Solution Path:");
        } else {
            cout << "No path found.\n";
        }
    }

    /**
     * Breadth-First Search (BFS) Implementation
     * Guarantees Shortest Path in an unweighted grid.
     * Time Complexity: O(V + E) -> O(rows * cols).
     * Space Complexity: O(V) -> O(rows * cols) for the queue.
     */
    void solveBFS(bool debugMode = false) {
        cout << "\n--- Solving using BFS (Shortest Path) ---\n";
        if (!isSafe(startPos.first, startPos.second) || !isSafe(endPos.first, endPos.second)) {
            cout << "No path found. Start or End is blocked/invalid.\n";
            return;
        }

        vector<vector<bool>> visited(rows, vector<bool>(cols, false));
        vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(cols, {-1, -1}));
        
        queue<pair<pair<int, int>, int>> q; // { {row, col}, distance }
        q.push({startPos, 0});
        visited[startPos.first][startPos.second] = true;

        bool found = false;
        int exploredNodes = 0;

        vector<vector<char>> debugOverlay(rows, vector<char>(cols, ' '));

        while (!q.empty()) {
            auto curr = q.front();
            auto [r, c] = curr.first;
            int dist = curr.second;
            q.pop();
            exploredNodes++;

            if (debugMode) {
                if (make_pair(r, c) != startPos && make_pair(r, c) != endPos) debugOverlay[r][c] = '.';
                cout << "Step " << exploredNodes << ": Exploring (" << r << ", " << c << "), Distance: " << dist << "\n";
                printMaze(debugOverlay);
                delay(300);
            }

            if (r == endPos.first && c == endPos.second) {
                found = true;
                break;
            }

            for (int i = 0; i < 4; ++i) {
                int nr = r + dr[i];
                int nc = c + dc[i];

                if (isSafe(nr, nc) && !visited[nr][nc]) {
                    visited[nr][nc] = true;
                    parent[nr][nc] = {r, c};
                    q.push({{nr, nc}, dist + 1});
                }
            }
        }

        if (found) {
            cout << "Destination reached after exploring " << exploredNodes << " nodes!\n";
            reconstructPath(parent, "BFS Shortest Path:");
        } else {
            cout << "No path found.\n";
        }
    }

private:
    /**
     * Backtraces from endPos to startPos to mark the exact path using '*'
     */
    void reconstructPath(const vector<vector<pair<int, int>>>& parent, const string& title) const {
        vector<vector<char>> pathOverlay(rows, vector<char>(cols, ' '));
        int pathLength = 0;
        
        pair<int, int> curr = endPos;
        // Keep backtracking until we reach startPos
        while (curr != startPos) {
            // Note: start and end themselves will be overwritten with 'S' and 'E' by printMaze logic, 
            // but we add stars along the way.
            pathOverlay[curr.first][curr.second] = '*';
            curr = parent[curr.first][curr.second];
            pathLength++;
        }
        pathOverlay[startPos.first][startPos.second] = '*';
        
        cout << title << "\n";
        cout << "Path Length: " << pathLength << " steps.\n";
        printMaze(pathOverlay);
    }
};

int main() {
    /* Sample 10x10 Maze
     * 1 = Path, 0 = Wall
     */
    vector<vector<int>> mazeGrid = {
        {1, 1, 0, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
        {1, 1, 1, 1, 1, 1, 0, 1, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 1, 0},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 1, 1, 1, 0, 1, 1, 1}
    };

    pair<int, int> startPos = {0, 0};
    pair<int, int> endPos = {9, 9};

    MazeSolver solver(mazeGrid, startPos, endPos);
    
    // Display the initial map
    solver.displaySetup();

    // Run DFS (Turn debugMode to true to see step-by-step)
    solver.solveDFS(false);

    // Run BFS to find the shortest path
    solver.solveBFS(false);
    
    return 0;
}
