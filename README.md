# Weif Engine

This is a self-study project for learning game engine creation process.


## Contents
Engine uses glfw for window managment and OpenGL for rendering.

### Demo
![](https://github.com/AlexanderKostyuk/Weif-Engine/blob/main/gifs/demo.gif)

### Implemented
- ECS for gameobject managment. 
- Basic rendering system. 
    - Lighting.
    - Texture loading.
- Basic input system.
- Mesh normals generation from vertices.

## Requirements

- GCC 12+
- GLFW3
- Assimp
- Make

### Windows
You need to have [MinGW](https://winlibs.com/) and setted up %PATH% enviroment variable to MinGW binaries. 
For GLFW linking you need to add [libglfw3.a](https://www.glfw.org/download.html) file to lib folder of MinGW.
For Assimp linking you need to add [libassimp.a](https://github.com/assimp/assimp) file to lib folder of MinGW.

After setting up enviroment you can run command "mingw32-make run" to build and run project.

### Ubuntu 
Install GCC and Make with command:
```
sudo apt-get install build-essential
```
Install glfw libraries with command:
```
sudo apt-get install libglfw3
```
Install assimp libraries with command:
```
sudo apt-get install libassimp-dev
```

After installing all requirements run command in root folder.
```
make run
```

## Make commands
| Command | Result |
| ------- | ------ |
| run | builds test application and runs it |
| clean | deletes build folder |
| debug | outputs found src, and include paths, also returns generated obj file paths |

