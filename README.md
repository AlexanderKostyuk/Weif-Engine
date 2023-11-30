# Weif Engine

This is a self-study project for learning game engine creation process.

## Contents
Engine uses glfw for window managment and OpenGL for rendering.

### Demo
![](https://github.com/WoINBoBrA/Weif-Engine/blob/main/gifs/demo.gif)

### Implemented
- ECS for gameobject managment. 
- Basic rendering system. 
    - Lighting
    - Texture loading
- Basic input system.
- Mesh normals generation from vertices


## Requirements
Right now project requires MinGW compiler, and compiles with Makefiles through mingw32-make. 

## Make commands
| Command | Result |
| ------- | ------ |
| run | builds test application and runs it |
| clean | deletes build folder |
| debug | outputs found src, and include paths, also returns generated obj file paths |

