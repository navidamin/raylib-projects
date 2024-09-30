#include "raylib.h"
#include <vector>
#include <string>
#include <cmath>

const int screenWidth = 800;
const int screenHeight = 600;

struct Node {
    Vector2 position;
    float radius;
    std::vector<Node*> children;
    Node* parent;
    int level;

    Node(Vector2 pos, float r, Node* p = nullptr, int lvl = 0)
        : position(pos), radius(r), parent(p), level(lvl) {}
};

Color GetNodeColor(int level) {
    switch (level) {
        case 0: return (Color){ 116, 79, 198, 255 };   // 744fc6 (Root)
        case 1: return (Color){ 79, 134, 198, 255 };   // 4f86c6 (First level children)
        case 2: return (Color){ 79, 176, 198, 255 };   // 4fb0c6 (Second level children)
        case 3: return (Color){ 55, 147, 146, 255 };   // 379392 (Third level children)
        default: return (Color){ 23, 48, 28, 255 };    // 17301c (Fourth level and beyond)
    }
}

bool CheckCollisionPointOval(Vector2 point, Vector2 center, float radiusH, float radiusV, float expansion = 1.0f) {
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    return (dx * dx) / ((radiusH * expansion) * (radiusH * expansion)) +
           (dy * dy) / ((radiusV * expansion) * (radiusV * expansion)) <= 1.0f;
}

Node* FindNodeAtPosition(Node* root, Vector2 position) {
    if (CheckCollisionPointOval(position, root->position, root->radius, root->radius * 0.75f, 1.2f)) {
        return root;
    }
    for (Node* child : root->children) {
        Node* found = FindNodeAtPosition(child, position);
        if (found) return found;
    }
    return nullptr;
}

void DrawNodeAndChildren(Node* node, Node* hoverNode, Vector2 mousePos, bool* addPointClicked) {
    // Draw connections
    for (Node* child : node->children) {
        DrawLineBezier(node->position, child->position, 2, GRAY);
    }

    // Draw node
    Color nodeColor = GetNodeColor(node->level);
    DrawEllipse(node->position.x, node->position.y, node->radius, node->radius * 0.75f, nodeColor);
    DrawText(node->level == 0 ? "Root" : "Child", node->position.x - 25, node->position.y - 10, 20, WHITE);

    // Draw add points and check for hover
    if (node == hoverNode) {
        for (int i = 0; i < 4; i++) {
            float angle = i * (PI / 2);
            Vector2 circlePos = {
                node->position.x + cosf(angle) * node->radius,
                node->position.y + sinf(angle) * node->radius * 0.75f
            };
            Color pointColor = CheckCollisionPointCircle(mousePos, circlePos, 5) ? GREEN : RED;
            DrawCircleV(circlePos, 5, pointColor);

            if (CheckCollisionPointCircle(mousePos, circlePos, 5)) {
                DrawText("Add Child", mousePos.x + 10, mousePos.y + 10, 20, BLACK);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !*addPointClicked) {
                    Vector2 newPos = {
                        node->position.x + cosf(angle) * node->radius * 2,
                        node->position.y + sinf(angle) * node->radius * 1.5f
                    };
                    Node* newNode = new Node(newPos, node->radius * 0.8f, node, node->level + 1);
                    node->children.push_back(newNode);
                    *addPointClicked = true;
                }
            }
        }
    }

    // Recursively draw children
    for (Node* child : node->children) {
        DrawNodeAndChildren(child, hoverNode, mousePos, addPointClicked);
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "Mindmap App");
    SetTargetFPS(60);

    Node* root = new Node({screenWidth / 2.0f, screenHeight / 2.0f}, 60);
    Node* selectedNode = nullptr;
    Node* hoverNode = nullptr;
    Vector2 offset;
    bool addPointClicked = false;

    while (!WindowShouldClose()) {
        // Update
        Vector2 mousePos = GetMousePosition();

        // Find the node under the mouse
        hoverNode = FindNodeAtPosition(root, mousePos);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedNode = hoverNode;
            if (selectedNode) {
                offset = {mousePos.x - selectedNode->position.x, mousePos.y - selectedNode->position.y};
            }
            addPointClicked = false;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            selectedNode = nullptr;
        }

        if (selectedNode) {
            selectedNode->position = {mousePos.x - offset.x, mousePos.y - offset.y};
        }

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawNodeAndChildren(root, hoverNode, mousePos, &addPointClicked);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
