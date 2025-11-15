#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define PI 3.1415926
#define SAVE_PATH "C:/graphs/"
#define FONT_PATH "C:/Windows/Fonts/arial.ttf"

using std::cout, std::cin, std::endl, std::min;
using std::fstream, std::stringstream;
using std::vector, std::string, std::map;
using vec2 = sf::Vector2f;
using scan = sf::Keyboard::Scancode;
using col = sf::Color;

vec2 lerp(vec2 p1, vec2 p2, float t);
float len(vec2 input);
vec2 norm(vec2 input);
vec2 perp(vec2 input);
bool inside(vec2 input, vec2 center, float l) {
    return pow(input.x - center.x, 2) + pow(input.y - center.y, 2) < l * l;
}
vec2 div(vec2 a, vec2 b) {
    return vec2(a.x * b.x + a.y * b.y, a.y * b.x - a.x * b.y) / (b.x * b.x + b.y * b.y);
}
vec2 mul(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

void drawGrid(sf::RenderTarget& target);

enum Mode {
    CreateVertices,
    CreateEdges,
    CreateText,
    DeleteVertices,
    DeleteEdges,
    DeleteText,
    ModVertices,
    ModEdges,
    ModText,
};

struct Mark {
    vec2 pos;
    sf::CircleShape mark;

    Mark(vec2 pos) : pos(pos), mark(12) {
        mark.setOrigin(12, 12);
        mark.setPosition(pos);
        mark.setFillColor(col::Cyan);
        mark.setOutlineColor(col::White);
    }

    Mark(int x, int y) : pos(vec2(x, y)), mark(12) {
        mark.setOrigin(12, 12);
        mark.setPosition(pos);
        mark.setFillColor(col::Cyan);
        mark.setOutlineColor(col::White);
    }
};

struct Vertex {
    vec2 pos;
    col color;
    sf::CircleShape shape;

    Vertex(vec2 pos, col color = col::Black) : pos(pos), color(color) {
        shape.setRadius(20);
        shape.setOrigin(20, 20);
        shape.setPointCount(20);
        shape.setPosition(pos);
        shape.setOutlineThickness(3.f);
    }

    vec2 xy() const {
        return pos;
    }

    bool operator==(Vertex v1) const {
        return &v1 == this;
    }
};

struct Edge {
    Vertex *v1, *v2;
    Mark* p3;
    col color;
    sf::VertexArray points;
    sf::VertexArray arrow;
    sf::VertexArray anchors;
    bool oriented = false;

    Edge(Vertex* v1, Vertex* v2, Mark* p3, col color = col::Black, bool oriented = false)
        : v1(v1),
          v2(v2),
          color(color),
          p3(p3),
          points(sf::PrimitiveType::TriangleStrip, 42),
          arrow(sf::TriangleFan, 3),
          anchors(sf::LineStrip, 3),
          oriented(oriented) {
        anchors[0].color = col::Cyan;
        anchors[1].color = col::Cyan;
        anchors[2].color = col::Cyan;
        updatePoints();
    }

    void updatePoints() {
        anchors[0].position = v1->pos;
        anchors[1].position = p3->pos;
        anchors[2].position = v2->pos;
        if (v1 == v2) {
            vec2 cen = (v1->pos + p3->pos) / 2.f;
            float rad = len(cen);
            for (int i = 0; i < 21; i++) {
                points[i * 2].color = color;
                points[i * 2 + 1].color = color;
                points[i * 2].position = cen + vec2(rad * cos(PI * i / 15), rad * sin(PI * i / 15));
                points[i * 2 + 1].position = cen + vec2((rad - 1.5) + cos(PI * i / 15), (rad - 1.5) * sin(PI * i / 15));
            }
            return;
        }
        vec2 nextPoint = vec2(), prevPoint = v1->xy(), dv = vec2();
        points[0].position = v1->xy() + vec2(1, 1);
        points[0].color = color;
        points[1].position = v1->xy();
        points[1].color = color;
        for (int i = 1; i < 21; i++) {
            float t = (float)i / 20;
            nextPoint = lerp(lerp(v1->xy(), p3->pos, t), lerp(p3->pos, v2->xy(), t), t);
            dv = perp(norm(nextPoint - prevPoint));
            points[i * 2].color = color;
            points[i * 2 + 1].color = color;
            points[i * 2].position = nextPoint + dv * 1.5f;
            points[i * 2 + 1].position = nextPoint - dv * 1.5f;
            prevPoint = nextPoint;
        }
    }

    bool operator==(Edge other) const {
        return p3 == other.p3;
    }
};

struct Text {
    vec2 pos;
    sf::Text text;

    Text(string textString, sf::Font& font, vec2 pos) : pos(pos) {
        cout << "Created text " << textString << endl;
        text.setFont(font);
        text.setFillColor(col::Black);
        text.setOutlineColor(col::Black);
        text.setString(textString);
        text.setCharacterSize(30);
        text.setStyle(sf::Text::Regular);
        text.setOrigin(text.getLocalBounds().getSize() / 2.f);
        text.setPosition(pos);
    }
};

void saveGraphAsImage(char path[128], vec2 topleft, vec2 bottomright, vector<Vertex*>& vertices, vector<Edge*>& edges,
                      vector<Text*>& textObjs);
void saveGraphAsFile(char path[128], vector<Vertex*>& vertices, vector<Edge*>& edges, vector<Text*>& textObjs,
                     const vector<col>& colors);
void loadGraphFromFile(char path[128], vector<Vertex*>& vertices, vector<Edge*>& edges, vector<Text*>& textObjs,
                       const vector<col>& colors, sf::Font& font);

void placeInCircle(vec2 topleft, vec2 bottomright, vector<Vertex*>& vertices, vector<Edge*>& edges);
void oppositeEdges(vector<Vertex*>& vertices, vector<Edge*>& edges);
void updateEdgesWithVertex(vector<Edge*>& edges, Vertex* selectedVertex, vec2 new_pos);

int main() {
    const vector<string> names = {"Black", "Blue", "Yellow", "Red", "Green", "White"};
    const vector<col> colors = {col::Black, col::Blue, col::Yellow, col::Red, col::Green, col::White};
    // Global Mode
    // Edge and Vertex Colors
    int vCol = 0;
    int eCol = 0;
    // Rect for saving
    vec2 topleft = vec2();
    vec2 bottomRight = vec2(1000, 1000);
    int tl[2] = {0, 0};
    int br[2] = {1000, 1000};
    bool showRect = false;
    sf::RectangleShape rect;
    rect.setFillColor(col::Transparent);
    rect.setOutlineColor(col::Red);
    rect.setOutlineThickness(1);
    rect.setPosition(topleft);
    rect.setSize(bottomRight - topleft);
    // Selected Vertices
    int selectedVCol = 0;
    Vertex* selectedV1 = nullptr;
    Vertex* selectedV2 = nullptr;
    // Selected Edge
    int selectedECol = 0;
    Edge* selectedE = nullptr;
    bool orientEdges = false;
    // Selected Mark
    bool showMarks = false;
    // Selected Text
    static sf::Font font;
    if (!font.loadFromFile(FONT_PATH)) {
        cout << "ERROR: Failed to load font " << FONT_PATH << endl;
    }
    static char text_string[128] = "";
    static char text_string_2[128] = "";
    Text* selectedText = nullptr;
    // Mode
    int mode = Mode::CreateVertices;
    bool lockEdges = true;
    // Grid for snapping
    bool showGrid = false;
    static char filename[128] = "";
    // Graphics
    // Window
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Graph Builder", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // Storage
    // vector<Mark> marks{};
    vector<Vertex*> vertices{};
    vector<Edge*> edges{};
    vector<Text*> textObjs{};

    // ImGui
    sf::Clock clock;
    if (!ImGui::SFML::Init(window)) {
        cout << "Failed to init IMGUI!!!" << endl;
        return -4;
    }

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            ImGui::SFML::ProcessEvent(window, e);
            if (e.type == sf::Event::Closed) {
                window.close();
            } else if (e.type == sf::Event::KeyPressed) {
                switch (e.key.scancode) {
                    case scan::Space:
                        saveGraphAsImage(filename, topleft, bottomRight, vertices, edges, textObjs);
                        break;
                    case scan::T:
                        if (mode == CreateText) {
                            mode = ModText;
                        } else if (mode == ModText) {
                            mode = DeleteText;
                        } else {
                            mode = CreateText;
                        }
                        break;
                    case scan::E:
                        if (mode == CreateEdges) {
                            mode = ModEdges;
                        } else if (mode == ModEdges) {
                            mode = DeleteEdges;
                        } else {
                            mode = CreateEdges;
                        }
                        break;
                    case scan::V:
                        if (mode == CreateVertices) {
                            mode = ModVertices;
                        } else if (mode == ModVertices) {
                            mode = DeleteVertices;
                        } else {
                            mode = CreateVertices;
                        }
                        break;
                    default:
                        break;
                }
            } else if (e.type == sf::Event::Resized) {
                window.setSize({e.size.width, e.size.height});
            } else if (e.type == sf::Event::MouseButtonPressed) {
                if (ImGui::GetIO().WantCaptureMouse)
                    continue;
                if (e.mouseButton.button == sf::Mouse::Button::Left) {
                    if (mode == CreateVertices) {
                        int x = e.mouseButton.x, y = e.mouseButton.y;
                        if (showGrid) {
                            x -= x % 50;
                            y -= y % 50;
                        }
                        vertices.emplace_back(new Vertex{vec2(x, y), colors[vCol]});
                    } else if (mode == CreateEdges) {
                        Vertex* clicked = nullptr;
                        for (Vertex* v : vertices) {
                            if (inside(v->pos, vec2(e.mouseButton.x, e.mouseButton.y), 20)) {
                                clicked = v;
                                break;
                            }
                        }
                        if (!clicked) {
                            continue;
                        }
                        if (!selectedV1) {
                            selectedV1 = clicked;
                            continue;
                        }
                        if (!selectedV2) {
                            selectedV2 = clicked;
                        }
                        Mark* to_add = new Mark(selectedV1 == selectedV2 ? (selectedV1->pos + vec2(50, 50))
                                                                         : (selectedV1->pos + selectedV2->pos) / 2.f);
                        edges.emplace_back(new Edge{selectedV1, selectedV2, to_add, colors[eCol],
                                                    (selectedV1 == selectedV2 ? false : orientEdges)});
                        selectedV1 = nullptr;
                        selectedV2 = nullptr;
                    } else if (mode == CreateText) {
                        int x = e.mouseButton.x, y = e.mouseButton.y;
                        if (showGrid) {
                            x -= x % 50;
                            y -= y % 50;
                        }
                        textObjs.emplace_back(new Text{string(text_string), font, vec2(x, y)});
                    } else if (mode == DeleteEdges) {
                        int to_delete = -1;
                        for (int i = 0; i < edges.size(); i++) {
                            if (inside(edges[i]->p3->pos, vec2(e.mouseButton.x, e.mouseButton.y), 12)) {
                                to_delete = i;
                                break;
                            }
                        }
                        if (to_delete == -1) {
                            continue;
                        }
                        delete edges[to_delete]->p3;
                        delete edges[to_delete];
                        edges.erase(edges.begin() + to_delete);
                    } else if (mode == DeleteVertices) {
                        int to_delete = -1;
                        for (int i = 0; i < vertices.size(); i++) {
                            if (inside(vertices[i]->pos, vec2(e.mouseButton.x, e.mouseButton.y), 20)) {
                                to_delete = i;
                                selectedV1 = vertices[i];
                                break;
                            }
                        }
                        if (to_delete == -1) {
                            selectedV1 = nullptr;
                            break;
                        }
                        for (int i = 0; i < edges.size(); i++) {
                            if (edges[i]->v1 == selectedV1 || edges[i]->v2 == selectedV1) {
                                delete edges[i]->p3;
                                delete edges[i];
                                edges.erase(edges.begin() + i);
                                i--;
                            }
                        }
                        delete vertices[to_delete];
                        vertices.erase(vertices.begin() + to_delete);
                    } else if (mode == DeleteText) {
                        int to_delete = -1;
                        for (int i = 0; i < textObjs.size(); i++) {
                            if (textObjs[i]->text.getGlobalBounds().intersects(
                                    sf::FloatRect{(float)e.mouseButton.x - 2, (float)e.mouseButton.y - 2, 4, 4})) {
                                to_delete = i;
                                break;
                            }
                        }
                        if (to_delete == -1) {
                            break;
                        }
                        delete textObjs[to_delete];
                        textObjs.erase(textObjs.begin() + to_delete);
                    } else if (mode == ModEdges) {
                        if (selectedE) {
                            int x = e.mouseButton.x, y = e.mouseButton.y;
                            if (showGrid) {
                                x -= x % 50;
                                y -= y % 50;
                            }
                            selectedE->p3->pos = vec2(x, y);
                            continue;
                        }
                        for (Edge* edge : edges) {
                            if (inside(edge->p3->pos, vec2(e.mouseButton.x, e.mouseButton.y), 12)) {
                                selectedE = edge;
                                selectedECol = find(colors.begin(), colors.end(), selectedE->color) - colors.begin();
                                break;
                            }
                        }
                    } else if (mode == ModVertices) {
                        if (selectedV1) {
                            int x = e.mouseButton.x, y = e.mouseButton.y;
                            if (showGrid) {
                                x -= x % 50;
                                y -= y % 50;
                            }
                            if (lockEdges) {
                                updateEdgesWithVertex(edges, selectedV1, vec2(x, y));
                            }
                            selectedV1->pos = vec2(x, y);
                            selectedV1->shape.setPosition(vec2(x, y));
                            continue;
                        }
                        for (Vertex* v : vertices) {
                            if (inside(v->pos, vec2(e.mouseButton.x, e.mouseButton.y), 20)) {
                                selectedV1 = v;
                                selectedVCol = find(colors.begin(), colors.end(), selectedV1->color) - colors.begin();
                                break;
                            }
                        }
                    } else {
                        if (selectedText) {
                            int x = e.mouseButton.x, y = e.mouseButton.y;
                            if (showGrid) {
                                x -= x % 50;
                                y -= y % 50;
                            }
                            selectedText->pos = vec2(x, y);
                            selectedText->text.setPosition(vec2(x, y));
                            continue;
                        }
                        for (Text* t : textObjs) {
                            if (t->text.getGlobalBounds().intersects(
                                    sf::FloatRect{(float)e.mouseButton.x - 2, (float)e.mouseButton.y - 2, 4, 4})) {
                                selectedText = t;
                                strcpy(text_string_2, t->text.getString().toAnsiString().c_str());
                                break;
                            }
                        }
                    }
                } else if (e.mouseButton.button == sf::Mouse::Button::Right) {
                    if (mode == Mode::ModVertices) {
                        selectedV1 = nullptr;
                        selectedV2 = nullptr;
                    } else if (mode == Mode::ModEdges) {
                        selectedE = nullptr;
                    } else if (mode == Mode::ModText) {
                        selectedText = nullptr;
                    }
                }
            }
        }

        ImGui::SFML::Update(window, clock.restart());

        ImGui::Begin("Parameters");

        ImGui::Checkbox("Show grid", &showGrid);
        ImGui::Checkbox("Show rect", &showRect);
        ImGui::Checkbox("Lock edges to vertices", &lockEdges);
        ImGui::InputInt2("Rect Top Left", tl);
        ImGui::InputInt2("Rect Bottom Right", br);
        ImGui::Checkbox("Create oriented edges", &orientEdges);
        ImGui::Text("Mode");
        ImGui::RadioButton("Create edges", &mode, Mode::CreateEdges);
        ImGui::RadioButton("Create vertices", &mode, Mode::CreateVertices);
        ImGui::RadioButton("Create text", &mode, Mode::CreateText);
        ImGui::RadioButton("Delete Vertices", &mode, Mode::DeleteVertices);
        ImGui::RadioButton("Delete Edges", &mode, Mode::DeleteEdges);
        ImGui::RadioButton("Delete Text", &mode, Mode::DeleteText);
        ImGui::RadioButton("Modify Vertices", &mode, Mode::ModVertices);
        ImGui::RadioButton("Modify Edges", &mode, Mode::ModEdges);
        ImGui::RadioButton("Modify Text", &mode, Mode::ModText);

        if (ImGui::TreeNode("Vertex color")) {
            for (int i = 0; i < names.size(); i++) {
                ImGui::RadioButton(names[i].c_str(), &vCol, i);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Edge color")) {
            for (int i = 0; i < names.size() - 1; i++) {
                ImGui::RadioButton(names[i].c_str(), &eCol, i);
            }
            ImGui::TreePop();
        }

        ImGui::InputTextWithHint("Text to add", "Input text to add as object", text_string, 128);

        ImGui::InputTextWithHint("File name", "Input filename without the extension", filename, 128);

        if (ImGui::Button("Save graph as image")) {
            saveGraphAsImage(filename, topleft, bottomRight, vertices, edges, textObjs);
        }
        if (ImGui::Button("Save graph as file")) {
            saveGraphAsFile(filename, vertices, edges, textObjs, colors);
        }
        if (ImGui::Button("Load graph from file")) {
            loadGraphFromFile(filename, vertices, edges, textObjs, colors, font);
        }
        // Place vertices in a circle
        if (ImGui::Button("Place in a circle")) {
            placeInCircle(topleft, bottomRight, vertices, edges);
        }
        // Create opposite edges
        // if (ImGui::Button("Opposite edges")) {
        //     oppositeEdges(vertices, edges);
        // }
        if (ImGui::Button("Repaint all vertices")) {
            for (auto& v : vertices) {
                v->color = colors[vCol];
            }
        }
        if (ImGui::Button("Repaint all edges")) {
            for (auto& e : edges) {
                e->color = colors[eCol];
            }
        }

        ImGui::End();

        if (mode == ModVertices && selectedV1) {
            ImGui::Begin("Modify vertex");
            ImGui::Text("Vertex color:");
            for (int i = 0; i < names.size(); i++) {
                ImGui::RadioButton(names[i].c_str(), &selectedVCol, i);
            }
            if (ImGui::Button("Change Color")) {
                selectedV1->color = colors[selectedVCol];
            }
            ImGui::End();
        } else if (mode == ModEdges && selectedE) {
            ImGui::Begin("Modify edge");
            ImGui::Checkbox("Oriented", &selectedE->oriented);
            ImGui::Text("Edge color:");
            for (int i = 0; i < names.size(); i++) {
                ImGui::RadioButton(names[i].c_str(), &selectedECol, i);
            }
            if (ImGui::Button("Change Color")) {
                selectedE->color = colors[selectedECol];
            }
            ImGui::End();
        } else if (mode == ModText && selectedText) {
            ImGui::Begin("Modify text");
            ImGui::InputText("Modify string", text_string_2, 128);
            if (ImGui::Button("Change text")) {
                if (text_string_2 != "") {
                    selectedText->text.setString(string(text_string_2));
                }
            }
            ImGui::End();
        }

        // Reset selected objects
        if (mode != ModVertices && mode != CreateEdges) {
            selectedV1 = nullptr;
            selectedV2 = nullptr;
        }
        if (mode != ModText) {
            selectedText = nullptr;
        }
        if (mode != ModEdges) {
            selectedE = nullptr;
        }
        topleft = vec2(tl[0], tl[1]);
        bottomRight = vec2(br[0], br[1]);

        // Graphics
        window.clear(col::White);
        if (showGrid) {
            drawGrid(window);
        }
        if (showRect) {
            rect.setPosition(topleft);
            rect.setSize(bottomRight - topleft);
            window.draw(rect);
        }
        // Draw edges
        for (Edge* e : edges) {
            e->updatePoints();
            window.draw(e->points);
            // Draw edge marks and anchors if necessary
            if (mode == ModEdges || mode == DeleteEdges) {
                window.draw(e->anchors);
                e->p3->mark.setPosition(e->p3->pos);
                window.draw(e->p3->mark);
            }
            if (e->oriented) {
                // Draw arrows if an edge is oriented
                window.draw(e->arrow);
            }
        }
        for (Vertex* v : vertices) {
            v->shape.setFillColor(v->color);
            if (v->color != col::White) {
                v->shape.setOutlineColor(v->color);
            } else {
                v->shape.setOutlineColor(col::Black);
            }
            if (v == selectedV1 || v == selectedV2) {
                v->shape.setOutlineColor(col::Cyan);
            }
            window.draw(v->shape);
        }
        for (Text* t : textObjs) {
            t->text.setOutlineColor(col::Black);
            if (mode == ModText) {
                t->text.setOutlineColor(col::Cyan);
            }
            if (t == selectedText) {
                t->text.setOutlineColor(col::Red);
            }
            window.draw(t->text);
        }

        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}

vec2 lerp(vec2 p1, vec2 p2, float t) {
    return p1 + t * (p2 - p1);
}
float len(vec2 input) {
    return sqrt(input.x * input.x + input.y * input.y);
}
vec2 norm(vec2 input) {
    return input / len(input);
}
vec2 perp(vec2 input) {
    return {input.y, -input.x};
}

void drawGrid(sf::RenderTarget& target) {
    sf::RectangleShape rect(vec2(3, target.getSize().y));
    rect.setFillColor(col(125, 125, 125, 125));
    for (int x = 50; x < target.getSize().x; x += 50) {
        rect.setPosition(x - 1, 0);
        target.draw(rect);
    }
    rect.setSize(vec2(target.getSize().x, 3));
    for (int y = 50; y < target.getSize().y; y += 50) {
        rect.setPosition(0, y - 1);
        target.draw(rect);
    }
}

void saveGraphAsImage(char path[128], vec2 topleft, vec2 bottomright, vector<Vertex*>& vertices, vector<Edge*>& edges,
                      vector<Text*>& textObjs) {
    string path_cpp(path);
    path_cpp = SAVE_PATH + path_cpp;
    path_cpp += ".png";

    cout << "Saving to " << path_cpp << endl;

    sf::Image toSave;
    sf::RenderTexture toRender;
    toRender.create((bottomright - topleft).x, (bottomright - topleft).y);
    // toSave.create((bottomright - topleft).x, (bottomright - topleft).y);

    for (Vertex* v : vertices) {
        v->pos -= topleft;
    }
    for (Edge* e : edges) {
        e->p3->pos -= topleft;
        e->updatePoints();
        toRender.draw(e->points);
        e->p3->pos += topleft;
    }
    for (Vertex* v : vertices) {
        v->shape.setFillColor(v->color);
        if (v->color == col::White) {
            v->shape.setOutlineColor(col::Black);
        } else {
            v->shape.setOutlineColor(v->color);
        }
        v->shape.setPosition(v->xy());
        toRender.draw(v->shape);
        v->pos += topleft;
        v->shape.setPosition(v->xy());
    }
    for (Text* t : textObjs) {
        t->text.move(-topleft);
        toRender.draw(t->text);
        t->text.move(topleft);
    }
    toSave = toRender.getTexture().copyToImage();
    toSave.flipVertically();
    toSave.saveToFile(path_cpp);
}
void saveGraphAsFile(char path[128], vector<Vertex*>& vertices, vector<Edge*>& edges, vector<Text*>& textObjs,
                     const vector<col>& colors) {
    string path_cpp(path);
    path_cpp = SAVE_PATH + path_cpp;
    path_cpp += ".txt";

    cout << "Saving to " << path_cpp << endl;

    fstream outputFile;
    outputFile.open(path_cpp, std::ios_base::out | std::ios_base::trunc);
    if (!outputFile.is_open()) {
        cout << "ERROR: Failed to open file" << endl;
        return;
    }
    vector<Vertex*> pointers{};
    // Save vertices
    // V pos.X pos.Y col
    for (int i = 0; i < vertices.size(); i++) {
        outputFile << "V " << i << " " << vertices[i]->pos.x << " " << vertices[i]->pos.y << " "
                   << find(colors.begin(), colors.end(), vertices[i]->color) - colors.begin() << endl;
        pointers.emplace_back(vertices[i]);
    }
    // Save edges
    // E v1i v2i mark.pos.x mark.pos.y col or
    for (int i = 0; i < edges.size(); i++) {
        outputFile << "E " << find(pointers.begin(), pointers.end(), edges[i]->v1) - pointers.begin() << " "
                   << find(pointers.begin(), pointers.end(), edges[i]->v2) - pointers.begin() << " "
                   << (int)edges[i]->p3->pos.x << " " << (int)edges[i]->p3->pos.y << " "
                   << find(colors.begin(), colors.end(), edges[i]->color) - colors.begin() << " " << edges[i]->oriented
                   << endl;
    }
    // Save textObjs
    // T string x y
    for (int i = 0; i < textObjs.size(); i++) {
        outputFile << "T " << textObjs[i]->text.getString().toAnsiString() << " " << textObjs[i]->pos.x << " "
                   << textObjs[i]->pos.y << endl;
    }
    outputFile.close();
}
void loadGraphFromFile(char path[128], vector<Vertex*>& vertices, vector<Edge*>& edges, vector<Text*>& textObjs,
                       const vector<col>& colors, sf::Font& font) {
    string path_cpp(path);
    path_cpp = SAVE_PATH + path_cpp;
    path_cpp += ".txt";

    cout << "Opening graph from " << path_cpp << endl;

    fstream inputFile;
    inputFile.open(path_cpp, std::ios_base::in);
    if (!inputFile.is_open()) {
        cout << "ERROR: Failed to open file" << endl;
        return;
    }

    int initial_size = vertices.size();
    string line;
    stringstream input;
    while (getline(inputFile, line)) {
        input << line;
        cout << line << endl;
        char type;
        input >> type;
        if (type == 'V') {
            cout << "Add vertex" << endl;
            int i, x, y, c;
            input >> i >> x >> y >> c;
            vertices.emplace_back(new Vertex{vec2(x, y), colors[c]});
        } else if (type == 'E') {
            cout << "Add edge" << endl;
            int v1i, v2i, mx, my, c;
            bool oriented;
            input >> v1i >> v2i >> mx >> my >> c >> oriented;
            v1i += initial_size;
            v2i += initial_size;
            cout << v1i << " " << v2i << " " << mx << " " << my << " " << c << " " << oriented << endl;
            Mark* m = new Mark(mx, my);
            edges.emplace_back(new Edge{vertices[v1i], vertices[v2i], m, colors[c], oriented});
        } else if (type == 'T') {
            cout << "Add text" << endl;
            string t;
            int x, y;
            input >> t >> x >> y;
            textObjs.emplace_back(new Text{t, font, vec2(x, y)});
        } else {
            cout << "ERROR: Invalid file format" << endl;
        }
        input.clear();
    }
}

void placeInCircle(vec2 topleft, vec2 bottomright, vector<Vertex*>& vertices, vector<Edge*>& edges) {
    float radius = min(bottomright.x - topleft.x, bottomright.y - topleft.y) / 2 - 60;
    vec2 center = (topleft + bottomright) / 2.f;
    float dphi = 2 * PI / vertices.size();
    for (int i = 0; i < vertices.size(); i++) {
        updateEdgesWithVertex(edges, vertices[i], center + radius * vec2(sin(dphi * i), cos(dphi * i)));
        vertices[i]->pos = center + radius * vec2(sin(dphi * i), cos(dphi * i));
        vertices[i]->shape.setPosition(vertices[i]->pos);
    }
}
void oppositeEdges(vector<Vertex*>& vertices, vector<Edge*>& edges) {
}

void updateEdgesWithVertex(vector<Edge*>& edges, Vertex* selectedVertex, vec2 new_pos) {
    for (Edge* e : edges) {
        if (e->v1 != selectedVertex && e->v2 != selectedVertex)
            continue;
        Vertex* other = e->v1 == selectedVertex ? e->v2 : e->v1;
        vec2 z_prime = div(new_pos - other->pos, selectedVertex->pos - other->pos);
        e->p3->pos = other->pos + mul(e->p3->pos - other->pos, z_prime);
    }
}
