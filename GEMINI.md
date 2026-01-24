# RawSound Project Context

## Project Overview
**RawSound** is a C++ web server application built using the [Crow](https://github.com/CrowCpp/Crow) microframework. It is designed to serve static content and handle HTTP-to-HTTPS redirects.

The application is configured to:
- Run a redirection server on port 80 (redirects to `https://rawsound.com`).
- Run the main secure server on port 443.
- Serve static files (HTML, CSS, JS) from the `static/` directory.
- Use GZIP compression for responses.

## Environment & Dependencies
The project is set up for a macOS (`darwin`) environment using MacPorts.

*   **Language:** C++17
*   **Compiler:** GCC 15 (`g++-mp-15` via MacPorts)
*   **Libraries:**
    *   **Crow:** Included via `crow_all.h`.
    *   **Boost:** Version 1.88 (headers and libs required).
    *   **zlib:** For compression support.

## Building and Running

The project includes both a `Makefile` and `CMakeLists.txt`. The **`Makefile`** is the primary build method for the current environment as it contains specific include/library paths.

### Using Make (Recommended)

1.  **Build:**
    ```bash
    make
    ```
    This compiles the source and links against Boost and zlib, producing the `rawsound` executable.

2.  **Run:**
    ```bash
    ./rawsound
    ```
    *Note: Since the app binds to ports 80 and 443, you may need `sudo ./rawsound` if you lack permission to bind to privileged ports.*

3.  **Clean:**
    ```bash
    make clean
    ```

### CMake
A `CMakeLists.txt` exists but may require configuration to match the specific paths found in the `Makefile`.
```bash
mkdir build && cd build
cmake ..
make
```

## Key Files

*   **`main.cpp`**: The entry point. Configures the Crow app, routes, middleware (compression), and starts the server instances.
*   **`crow_all.h`**: The single-header distribution of the Crow framework.
*   **`Makefile`**: Custom build script with hardcoded paths for GCC 15 and Boost on macOS.
*   **`static/`**: Contains public-facing assets like `index.html`, `main.css`, and `favicon.ico`.
*   **`Crow_Reference.md`**: A local reference guide for Crow framework concepts and snippets.

## Development Conventions

*   **Routing:** Routes are defined using `CROW_ROUTE(app, "/path")`.
*   **Static Files:** The server defaults to serving `static/index.html` on the root `/` path.
*   **Threading:** The main server (port 443) is multithreaded, while the redirect server (port 80) runs asynchronously.
*   **Compression:** GZIP compression is enabled via `app.use_compression(...)`.
