# Plot

A data visualization project. It uses `ImPlot` and `ZeroMQ` to plot data in real-time.

## How it Works

The project follows a client-server architecture:

*   **`plot` (Shared Library):** This is the client library that you link against your C++ application. It provides a simple `Plot` class with methods like `plot()`, `bode()`, and `pzmap()`. When you call one of these methods, the library sends the plot data over a ZeroMQ (ZMQ) socket.

*   **`Plotting_App` (Executable):** This is the server application that runs as a separate process. It listens for incoming data on a ZMQ socket. When it receives data from the `plot` library, it uses `ImPlot`, `SDL3`, and `OpenGL` to render the visualization in a window.

This decoupled design allows you to add plotting capabilities to any C++ application without integrating the entire graphics stack directly into your project.

## Development Environment

*   **Language:** C, C++17
*   **Compiler:** Clang
*   **Build System:** CMake (minimum version 3.25)

## Dependencies

*   `ImPlot` (included as a submodule or directory)
*   `zmq` (ZeroMQ)
*   `SDL3`
*   `OpenGL`

### Installing Dependencies

Here's how to install the required dependencies on different operating systems:

#### Windows (using MSYS2/MinGW or vcpkg)
If you are using MSYS2:
```bash
pacman -S mingw-w64-x86_64-clang
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-zeromq
pacman -S mingw-w64-x86_64-SDL3
pacman -S mingw-w64-x86_64-mesa # For OpenGL
```
*(Alternatively, you can use [vcpkg](https://vcpkg.io/) to install dependencies: `vcpkg install zeromq sdl3 opengl`)*

> **Note on Visual Studio:** This project is primarily developed and tested with Clang on Windows (via MSYS2/MinGW). While it may be possible to build with Visual Studio's MSVC compiler, this configuration has not been tested.

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install clang cmake libzmq3-dev libsdl3-dev libgl1-mesa-dev
```

## Build and Run

You can build the project from the command line using CMake.

1.  First, launch the plotting application, which will run in the background:
    ```bash
    # From the build directory
    ./plotting_app/Plotting_App
    ```
2.  In a separate terminal, you can run your own application that uses the `plot` library.

### Building the Project
1.  Create a build directory and navigate into it:
    ```bash
    mkdir build
    cd build
    ```
2.  Configure the project with CMake (specifying Clang as the compiler):
    ```bash
    cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
    ```
3.  Build the project:
    ```bash
    cmake --build .
    ```

## Install

To install the library and the executable to your system:

```bash
cmake --install .
```

## License

This project is licensed under the [Apache License 2.0](LICENSE).
