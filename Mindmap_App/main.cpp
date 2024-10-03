#include "raylib.h"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstring>

const int screenWidth = 800;
const int screenHeight = 600;
const int MAX_INPUT_CHARS = 100;

struct Node {
    Vector2 position;
    float width;
    float height;
    std::vector<Node*> children;
    Node* parent;
    int level;
    std::string text;
    bool isEditing;

    Node(Vector2 pos, float w, float h, Node* p = nullptr, int lvl = 0)
        : position(pos), width(w), height(h), parent(p), level(lvl), text("Node"), isEditing(false) {}
};

struct Connection {
    Node* start;
    Node* end;
    bool isHovered;

    Connection(Node* s, Node* e) : start(s), end(e), isHovered(false) {}

    bool operator==(const Connection& other) const {
        return start == other.start && end == other.end;
    }
};

float Vector2Distance(Vector2 v1, Vector2 v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return sqrt(dx*dx + dy*dy);
}

Color GetNodeColor(int level) {
    switch (level) {
        case 0: return (Color){ 116, 79, 198, 255 };   // 744fc6 (Root)
        case 1: return (Color){ 79, 134, 198, 255 };   // 4f86c6 (First level children)
        case 2: return (Color){ 79, 176, 198, 255 };   // 4fb0c6 (Second level children)
        case 3: return (Color){ 55, 147, 146, 255 };   // 379392 (Third level children)
        default: return (Color){ 23, 48, 28, 255 };    // 17301c (Fourth level and beyond)
    }
}

bool CheckCollisionPointOval(Vector2 point, Vector2 center, float width, float height, float expansion = 1.0f) {
    float a = width / 2 * expansion;
    float b = height / 2 * expansion;
    float x = point.x - center.x;
    float y = point.y - center.y;
    return (x*x)/(a*a) + (y*y)/(b*b) <= 1.0f;
}

Node* FindNodeAtPosition(Node* root, Vector2 position) {
    if (CheckCollisionPointOval(position, root->position, root->width, root->height, 1.2f)) {
        return root;
    }
    for (Node* child : root->children) {
        Node* found = FindNodeAtPosition(child, position);
        if (found) return found;
    }
    return nullptr;
}

bool IsPointOnLine(Vector2 point, Vector2 lineStart, Vector2 lineEnd, float threshold = 5.0f) {
    float d1 = Vector2Distance(point, lineStart);
    float d2 = Vector2Distance(point, lineEnd);
    float lineLen = Vector2Distance(lineStart, lineEnd);
    return (d1 + d2 >= lineLen - threshold) && (d1 + d2 <= lineLen + threshold);
}

void RemoveNodeAndConnections(Node* nodeToRemove, std::vector<Connection>& connections) {
    connections.erase(
        std::remove_if(connections.begin(), connections.end(),
            [nodeToRemove](const Connection& conn) {
                return conn.start == nodeToRemove || conn.end == nodeToRemove;
            }
        ),
        connections.end()
    );

    if (nodeToRemove->parent) {
        auto it = std::find(nodeToRemove->parent->children.begin(), nodeToRemove->parent->children.end(), nodeToRemove);
        if (it != nodeToRemove->parent->children.end()) {
            nodeToRemove->parent->children.erase(it);
        }
    }

    for (Node* child : nodeToRemove->children) {
        RemoveNodeAndConnections(child, connections);
    }

    delete nodeToRemove;
}

void UpdateNodeSize(Node* node) {
    Vector2 textSize = MeasureTextEx(GetFontDefault(), node->text.c_str(), 20, 1);
    node->width = std::max(textSize.x + 40, 100.0f);  // Minimum width of 100
    node->height = std::max(textSize.y + 20, 60.0f);  // Minimum height of 60
}

void DrawNodeAndChildren(Node* node, Node* hoverNode, Vector2 mousePos, bool* addPointClicked,
                         std::vector<Connection>& connections, Node** draggingNode, Node** potentialParent) {
    for (auto& conn : connections) {
        if (conn.start == node) {
            Color lineColor = conn.isHovered ? RED : GRAY;
            DrawLineBezier(conn.start->position, conn.end->position, 2, lineColor);

            if (conn.isHovered) {
                DrawCircleV(conn.start->position, 5, BLUE);
                DrawCircleV(conn.end->position, 5, BLUE);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointCircle(mousePos, conn.start->position, 5)) {
                        *draggingNode = conn.start;
                    } else if (CheckCollisionPointCircle(mousePos, conn.end->position, 5)) {
                        *draggingNode = conn.end;
                    }
                }
            }
        }
    }

    Color nodeColor = GetNodeColor(node->level);
    DrawEllipse(node->position.x, node->position.y, node->width/2, node->height/2, nodeColor);
    DrawText(node->text.c_str(), node->position.x - node->width/2 + 10, node->position.y - 10, 20, WHITE);

    if (node == hoverNode && !node->isEditing) {
        for (int i = 0; i < 4; i++) {
            float angle = i * (PI / 2);
            Vector2 circlePos = {
                node->position.x + cosf(angle) * node->width/2,
                node->position.y + sinf(angle) * node->height/2
            };
            Color pointColor = CheckCollisionPointCircle(mousePos, circlePos, 5) ? GREEN : RED;
            DrawCircleV(circlePos, 5, pointColor);

            if (CheckCollisionPointCircle(mousePos, circlePos, 5)) {
                DrawText("Add Child", mousePos.x + 10, mousePos.y + 10, 20, BLACK);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !*addPointClicked) {
                    Vector2 newPos = {
                        node->position.x + cosf(angle) * node->width,
                        node->position.y + sinf(angle) * node->height
                    };
                    Node* newNode = new Node(newPos, 100, 60, node, node->level + 1);
                    node->children.push_back(newNode);
                    connections.push_back(Connection(node, newNode));
                    *addPointClicked = true;
                }
            }
        }
    }

    for (Node* child : node->children) {
        DrawNodeAndChildren(child, hoverNode, mousePos, addPointClicked, connections, draggingNode, potentialParent);
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "Mindmap App");
    SetTargetFPS(60);

    Node* root = new Node({screenWidth / 2.0f, screenHeight / 2.0f}, 100, 60);
    Node* selectedNode = nullptr;
    Node* hoverNode = nullptr;
    Node* draggingNode = nullptr;
    Node* potentialParent = nullptr;
    Node* editingNode = nullptr;
    Vector2 offset;
    bool addPointClicked = false;
    std::vector<Connection> connections;

    char* editBuffer = new char[MAX_INPUT_CHARS + 1];
    editBuffer[0] = '\0';

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        hoverNode = FindNodeAtPosition(root, mousePos);

        for (auto& conn : connections) {
            conn.isHovered = IsPointOnLine(mousePos, conn.start->position, conn.end->position);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (editingNode) {
                editingNode->isEditing = false;
                editingNode->text = editBuffer;
                UpdateNodeSize(editingNode);
                editingNode = nullptr;
            }
            selectedNode = hoverNode;
            if (selectedNode) {
                offset = {mousePos.x - selectedNode->position.x, mousePos.y - selectedNode->position.y};
                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    selectedNode->isEditing = true;
                    editingNode = selectedNode;
                    strcpy(editBuffer, selectedNode->text.c_str());
                }
            }
            addPointClicked = false;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            if (draggingNode && potentialParent) {
                connections.erase(
                    std::remove_if(connections.begin(), connections.end(),
                        [draggingNode](const Connection& conn) {
                            return conn.end == draggingNode;
                        }
                    ),
                    connections.end()
                );
                connections.push_back(Connection(potentialParent, draggingNode));

                if (draggingNode->parent) {
                    auto it = std::find(draggingNode->parent->children.begin(), draggingNode->parent->children.end(), draggingNode);
                    if (it != draggingNode->parent->children.end()) {
                        draggingNode->parent->children.erase(it);
                    }
                }
                draggingNode->parent = potentialParent;
                potentialParent->children.push_back(draggingNode);
            }
            selectedNode = nullptr;
            draggingNode = nullptr;
            potentialParent = nullptr;
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            if (hoverNode && hoverNode != root) {
                RemoveNodeAndConnections(hoverNode, connections);
                hoverNode = nullptr;
            } else {
                for (auto& conn : connections) {
                    if (conn.isHovered) {
                        auto it = std::find(conn.start->children.begin(), conn.start->children.end(), conn.end);
                        if (it != conn.start->children.end()) {
                            conn.start->children.erase(it);
                        }
                        conn.end->parent = nullptr;
                        connections.erase(std::find(connections.begin(), connections.end(), conn));
                        break;
                    }
                }
            }
        }

        if (selectedNode) {
            selectedNode->position = {mousePos.x - offset.x, mousePos.y - offset.y};
        }

        if (draggingNode) {
            potentialParent = FindNodeAtPosition(root, mousePos);
            if (potentialParent == draggingNode) potentialParent = nullptr;
        }

        if (editingNode) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (strlen(editBuffer) < MAX_INPUT_CHARS)) {
                    int len = strlen(editBuffer);
                    editBuffer[len] = (char)key;
                    editBuffer[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(editBuffer);
                if (len > 0) editBuffer[len - 1] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER)) {
                editingNode->isEditing = false;
                editingNode->text = editBuffer;
                UpdateNodeSize(editingNode);
                editingNode = nullptr;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawNodeAndChildren(root, hoverNode, mousePos, &addPointClicked, connections, &draggingNode, &potentialParent);

        if (draggingNode && potentialParent) {
            DrawLineBezier(draggingNode->position, potentialParent->position, 2, GREEN);
        }

        if (editingNode) {
            DrawRectangle(editingNode->position.x - editingNode->width/2,
                          editingNode->position.y - editingNode->height/2,
                          editingNode->width, editingNode->height, WHITE);
            DrawText(editBuffer, editingNode->position.x - editingNode->width/2 + 10,
                     editingNode->position.y - 10, 20, BLACK);
        }

        EndDrawing();
    }

    delete[] editBuffer;
    CloseWindow();
    return 0;
}
