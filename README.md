# Modern Durak Client
## Client which allows connecting to a Modern Durak Server
- Create an account
- Create a game in the lobby
- Play the game

## How to build the Mordern Durak Client 
Note: 
- Only tested on linux
### Prerequisite
- Conan Package Manager - "https://conan.io/downloads.html"
- Cmake - "https://cmake.org/download/"
- Emsdk (version 2.0.10) - "https://emscripten.org/docs/getting_started/downloads.html"
- Something to run WebAssembly (I use phyton to serve the files and chrome to execute)
### Test Prerequisite
Test the installation by running the following commands:
- 'cmake --version'
- 'conan --version'
- 'em++ --version'

If you get something like "command not found" please check your installation.
### Setup Conan Package Manager
Follow the instructions in this README "https://gitlab.com/werto87/conan-the-example"
### Build the Modern Durak Client
- Clone the repo 'git clone https://github.com/werto87/game_01_client.git'
- Change into cloned repo 'cd game_01_client/'
- Create build folder and change into it 'mkdir build && cd build'
- Get the dependencies 'conan install .. --profile:build=clang --profile:host=emscripten --build missing  -s build_type=Debug'
- Configure cmake 'emcmake cmake .. -DCMAKE_BUILD_TYPE=Debug'
- Build Modern Durak Client 'cmake --build .'
## How to run Modern Durak Client
### Prerequisite
You should have a Modern Durak Server running at localhost:55555
### Run Modern Durak Client
You need to run projects.html in the build folder with something which can run WebAssembly.
Tested way of running Modern Durak Client:
- Serve the bin folder, for example with 'python -m http.server' (Python 3.9.5)
- Use a browser to open project.html (tested with Google Chrome 91.0.4472.77)


