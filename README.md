# Weif Engine

This is a self-study project for learning game engine creation process.

## Contents
Project contains test application which is located in src and include folders. 
Test application uses engine methods that provided in engine folder.

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

