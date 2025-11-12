**Якщо ви малюєте графи для лаби, перед початком захисту кажіть, що програму написав студент СШІ Співак Димитрій**
# Graph builder

Application for building graphs created with SFML and ImGui.\
Uses Cmake for compilation. Template for SFML/ImGui projects provided by https://github.com/morgunovmi/imgui-docking-sfml-starter

**IMPORTANT:** change `#define SAVE_PATH` and `#define FONT_PATH` on line 14 and 15 in `main.cpp` to respective locations on your PC

## Showcase
![Screenshot 1](./screenshots/Graph_Subgraph_Union.png?raw=true)
![Screenshot 2](/screenshots/Graph_3.png?raw=true)
![Screenshot 3](./screenshots/Graph_Product.png?raw=true)
![Screenshot 4](/screenshots/Min_Dist_V_1.png?raw=true)

## How To Use
+ Select needed mode
+ Create vertices: click to create vertex
+ Create edges: select 2 vertices to create an edge
+ Create text: Input text into respective field and click anywhere to add it
+ Modify vertices/edges/text: click on an object and modify it
+ Delete vertices/edges/text: click on an object to delete it
+ Grid: when turned on, placed objects will snap to grid
+ Rect: Only objects inside the rectangle will be saved to image. Use numbers to control topleft and bottomright position of the rectangle
+ Oriented Edges: Doesn't work yet
+ Save graph as image: saves graph to `filename`.png in `save_path`
+ Save graph as file: saves graph to `filename`.txt in `save_path`
+ Load graph from file: loads graph from `filename`.txt in `save_path`
+ Repaint all vertices/edges: Repaints all vertices/edges to selected color

# How to build
You need to install CMake before compilation. After installation build it using CMake (optionally inside VSCode).

# If you find a bug
You can open an issue. It may or may not be fixed in future.

# If you want to make changes to code
Create a pull request. It may or may not be added in future.

# If you want to copy code
You can do anything with the code