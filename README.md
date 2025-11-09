# Graph builder

Application for building graphs.\
Uses Cmake for compilation. Template for SFML/ImGui projects provided by https://github.com/morgunovmi/imgui-docking-sfml-starter

**IMPORTANT:** to start using this you have to change `#define save_path` on line 16 in `main.cpp` to desired output location

## How To Use
### Grid
When turned on, placed objects will snap to grid
### Rect
Displays the ractangle, that is responsible for saving the screenshot. Only objects inside this rectangle will be visible on the screenshot.
### Oriented Edges
Doesn't work yet
### Create Vertices mode
Click anywhere to create a vertex of a graph
### Create Edges mode
Click on the first vertex, then on the second vertex to create an edge between them
### Delete Vertices mode
Click on the vertex to delete it and incident edges
### Delete Edges mode
Click on the respective mark to delete an edge
### Modify Vertices mode
Click on a vertex to select it. Right click to deselect. You can change its position and color
### Modify edges mode
Similar to previous mode. It is not possible to change incident vertices.
### Save graph as image
Saves graph to `filename`.png in `save_path`
### Save graph as file
Saves graph to `filename`.txt in `save_path`
### Load graph from file
Loads graph from `filename`.txt in `save_path`
### Place in a circle
Places vertices in a circle inside of rect. Doesn't move the edges, this has to be done manually
### Opposite edges
Does nothing currently