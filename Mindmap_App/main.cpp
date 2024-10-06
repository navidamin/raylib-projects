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

// struct for save options
struct SaveOptions {
    bool isOpen;
    int selectedSize;
    int selectedFormat;
    Rectangle saveButton;
    Rectangle sizeButtons[3];
    Rectangle formatButtons[2];

    SaveOptions() : isOpen(false), selectedSize(1), selectedFormat(0) {
        saveButton = (Rectangle){ screenWidth - 80, 10, 70, 40 };
        sizeButtons[0] = (Rectangle){ screenWidth - 180, 60, 50, 40 };
        sizeButtons[1] = (Rectangle){ screenWidth - 120, 60, 50, 40 };
        sizeButtons[2] = (Rectangle){ screenWidth - 60, 60, 50, 40 };
        formatButtons[0] = (Rectangle){ screenWidth - 180, 110, 80, 40 };
        formatButtons[1] = (Rectangle){ screenWidth - 90, 110, 80, 40 };
    }
};

Camera2D camera = { 0 };
float zoom = 1.0f;

void SaveScreenshot(int size, int format, Node* root, const std::vector<Connection>& connections);

Vector2 GetWorldMousePosition() {
    return GetScreenToWorld2D(GetMousePosition(), camera);
}

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

void UpdateNodeSize(Node* node) {
    Vector2 textSize = MeasureTextEx(GetFontDefault(), node->text.c_str(), 20, 1);
    node->width = std::max(textSize.x + 40, 100.0f);  // Minimum width of 100
    node->height = std::max(textSize.y + 20, 60.0f);  // Minimum height of 60
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

void DrawNodeAndChildren(Node* node, Node* hoverNode, Vector2 mousePos, bool* addPointClicked,
                         std::vector<Connection>& connections, Node** draggingNode, Node** potentialParent) {
    for (auto& conn : connections) {
        if (conn.start == node) {
            Color lineColor = conn.isHovered ? RED : GRAY;
            DrawLineBezier(conn.start->position, conn.end->position, 2 / zoom, lineColor);

            if (conn.isHovered) {
                DrawCircleV(conn.start->position, 5 / zoom, BLUE);
                DrawCircleV(conn.end->position, 5 / zoom, BLUE);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointCircle(mousePos, conn.start->position, 5 / zoom)) {
                        *draggingNode = conn.start;
                    } else if (CheckCollisionPointCircle(mousePos, conn.end->position, 5 / zoom)) {
                        *draggingNode = conn.end;
                    }
                }
            }
        }
    }

    Color nodeColor = GetNodeColor(node->level);
    DrawEllipse(node->position.x, node->position.y, node->width/2, node->height/2, nodeColor);

    // Center the text
    Vector2 textSize = MeasureTextEx(GetFontDefault(), node->text.c_str(), 20 / zoom, 1 / zoom);
    Vector2 textPosition = {
        node->position.x - textSize.x / 2,
        node->position.y - textSize.y / 2
    };
    DrawTextEx(GetFontDefault(), node->text.c_str(), textPosition, 20 / zoom, 1 / zoom, WHITE);

    if (node == hoverNode && !node->isEditing) {
        for (int i = 0; i < 4; i++) {
            float angle = i * (PI / 2);
            Vector2 circlePos = {
                node->position.x + cosf(angle) * node->width/2,
                node->position.y + sinf(angle) * node->height/2
            };
            Color pointColor = CheckCollisionPointCircle(mousePos, circlePos, 5 / zoom) ? GREEN : RED;
            DrawCircleV(circlePos, 5 / zoom, pointColor);

            if (CheckCollisionPointCircle(mousePos, circlePos, 5 / zoom)) {
                DrawText("Add Child", mousePos.x + 10 / zoom, mousePos.y + 10 / zoom, 20 / zoom, BLACK);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !*addPointClicked) {
                    Vector2 newPos = {
                        node->position.x + cosf(angle) * node->width,
                        node->position.y + sinf(angle) * node->height
                    };
                    Node* newNode = new Node(newPos, 100, 60, node, node->level + 1);
                    UpdateNodeSize(newNode);
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

void DrawSaveOptions(SaveOptions& options) {
    // Always draw the save button
    DrawRectangleRec(options.saveButton, BLUE);
    DrawText("Save", options.saveButton.x + 10, options.saveButton.y + 10, 20, WHITE);

    if (options.isOpen) {
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){ 0, 0, 0, 100 });

        for (int i = 0; i < 3; i++) {
            DrawRectangleRec(options.sizeButtons[i], options.selectedSize == i + 1 ? SKYBLUE : LIGHTGRAY);
            DrawText(TextFormat("%dX", i + 1), options.sizeButtons[i].x + 10, options.sizeButtons[i].y + 10, 20, BLACK);
        }

        for (int i = 0; i < 2; i++) {
            DrawRectangleRec(options.formatButtons[i], options.selectedFormat == i ? SKYBLUE : LIGHTGRAY);
            DrawText(i == 0 ? "PNG" : "JPG", options.formatButtons[i].x + 10, options.formatButtons[i].y + 10, 20, BLACK);
        }

        Rectangle saveConfirmButton = { screenWidth - 120, 160, 100, 40 };
        DrawRectangleRec(saveConfirmButton, GREEN);
        DrawText("Save", saveConfirmButton.x + 10, saveConfirmButton.y + 10, 20, BLACK);
    }
}

void CalculateNodeBounds(Node* node, Vector2& minBounds, Vector2& maxBounds) {
    minBounds.x = std::min(minBounds.x, node->position.x - node->width/2);
    minBounds.y = std::min(minBounds.y, node->position.y - node->height/2);
    maxBounds.x = std::max(maxBounds.x, node->position.x + node->width/2);
    maxBounds.y = std::max(maxBounds.y, node->position.y + node->height/2);

    for (Node* child : node->children) {
        CalculateNodeBounds(child, minBounds, maxBounds);
    }
}

void CalculateMindmapBounds(Node* root, Vector2& minBounds, Vector2& maxBounds) {
    minBounds = maxBounds = root->position;
    CalculateNodeBounds(root, minBounds, maxBounds);
}

void SaveScreenshot(int size, int format, Node* root, const std::vector<Connection>& connections) {
    // Calculate the bounds of the entire mindmap
    Vector2 minBounds, maxBounds;
    CalculateMindmapBounds(root, minBounds, maxBounds);

    // Calculate the dimensions of the mindmap
    float mindmapWidth = maxBounds.x - minBounds.x;
    float mindmapHeight = maxBounds.y - minBounds.y;

    // Add some padding
    float padding = 50.0f;
    mindmapWidth += 2 * padding;
    mindmapHeight += 2 * padding;

    // Calculate the aspect ratio of the mindmap and the screen
    float mindmapAspectRatio = mindmapWidth / mindmapHeight;
    float screenAspectRatio = (float)screenWidth / screenHeight;

    // Determine the dimensions of the render texture
    int textureWidth, textureHeight;
    if (mindmapAspectRatio > screenAspectRatio) {
        textureWidth = screenWidth * size;
        textureHeight = (int)(textureWidth / mindmapAspectRatio);
    } else {
        textureHeight = screenHeight * size;
        textureWidth = (int)(textureHeight * mindmapAspectRatio);
    }

    RenderTexture2D target = LoadRenderTexture(textureWidth, textureHeight);
    BeginTextureMode(target);
    ClearBackground(RAYWHITE);

    Camera2D saveCamera = { 0 };
    saveCamera.zoom = std::min((float)textureWidth / mindmapWidth, (float)textureHeight / mindmapHeight);
    saveCamera.offset = (Vector2){ textureWidth/2.0f, textureHeight/2.0f };
    saveCamera.target = (Vector2){ (minBounds.x + maxBounds.x) / 2, (minBounds.y + maxBounds.y) / 2 };

    BeginMode2D(saveCamera);

    // Render the mindmap
    DrawNodeAndChildren(root, nullptr, (Vector2){0, 0}, nullptr, const_cast<std::vector<Connection>&>(connections), nullptr, nullptr);

    EndMode2D();

    EndTextureMode();

    Image image = LoadImageFromTexture(target.texture);
    ImageFlipVertical(&image);

    if (format == 0) {
        ExportImage(image, "mindmap_screenshot.png");
    } else {
        ExportImage(image, "mindmap_screenshot.jpg");
    }

    UnloadImage(image);
    UnloadRenderTexture(target);
}


int main() {
    InitWindow(screenWidth, screenHeight, "Mindmap App");
    SetTargetFPS(60);

    camera.zoom = 1.0f;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.target = (Vector2){ 0, 0 };

    Node* root = new Node({0, 0}, 100, 60);
    UpdateNodeSize(root);
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

    SaveOptions saveOptions;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetWorldMousePosition();  // Use world coordinates for node interactions
        Vector2 screenMousePos = GetMousePosition(); // Use screen coordinates for UI interactions


        // Handle save options first (screen space)
        bool interactedWithUI = false;
        if (CheckCollisionPointRec(screenMousePos, saveOptions.saveButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            saveOptions.isOpen = !saveOptions.isOpen;
            interactedWithUI = true;
        }

        if (saveOptions.isOpen) {
            for (int i = 0; i < 3; i++) {
                if (CheckCollisionPointRec(screenMousePos, saveOptions.sizeButtons[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    saveOptions.selectedSize = i + 1;
                    interactedWithUI = true;
                }
            }
            for (int i = 0; i < 2; i++) {
                if (CheckCollisionPointRec(screenMousePos, saveOptions.formatButtons[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    saveOptions.selectedFormat = i;
                    interactedWithUI = true;
                }
            }
            Rectangle saveConfirmButton = { screenWidth - 120, 160, 100, 40 };
            if (CheckCollisionPointRec(screenMousePos, saveConfirmButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 SaveScreenshot(saveOptions.selectedSize, saveOptions.selectedFormat, root, connections);
                 saveOptions.isOpen = false;
                 interactedWithUI = true;
             }
        } // end (saveOptions.isOpen)

        if (!interactedWithUI) {

            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                camera.zoom += wheel * 0.05f;
                if (camera.zoom < 0.1f) camera.zoom = 0.1f;
                if (camera.zoom > 3.0f) camera.zoom = 3.0f;
            }

            if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
                Vector2 delta = GetMouseDelta();
                camera.target.x -= delta.x / camera.zoom;
                camera.target.y -= delta.y / camera.zoom;
            }
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
            } // end if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))

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
            } // end  (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))

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
            } // IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)

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
            } // if (editingNode)
        } // end if (!interactedWithUI)




        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        DrawNodeAndChildren(root, hoverNode, mousePos, &addPointClicked, connections, &draggingNode, &potentialParent);

        if (draggingNode && potentialParent) {
            DrawLineBezier(draggingNode->position, potentialParent->position, 2 / zoom, GREEN);
        }

        if (editingNode) {
            DrawRectangle(editingNode->position.x - editingNode->width/2,
                          editingNode->position.y - editingNode->height/2,
                          editingNode->width, editingNode->height, WHITE);

            Vector2 textSize = MeasureTextEx(GetFontDefault(), editBuffer, 20 / zoom, 1 / zoom);
            Vector2 textPosition = {
                editingNode->position.x - textSize.x / 2,
                editingNode->position.y - textSize.y / 2
            };
            DrawTextEx(GetFontDefault(), editBuffer, textPosition, 20 / zoom, 1 / zoom, BLACK);
        }

        EndMode2D();

        DrawText(TextFormat("Zoom: %.2f", camera.zoom), 10, 10, 20, BLACK);

        // Draw save options (including the always-visible save button)
        DrawSaveOptions(saveOptions);

        EndDrawing();
    }

    delete[] editBuffer;
    // TODO: Implement proper cleanup for all nodes

    CloseWindow();
    return 0;
}
