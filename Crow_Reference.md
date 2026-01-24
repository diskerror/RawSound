# Crow Framework Reference Guide

This guide digests the core concepts, syntax, and patterns of the Crow C++ microframework. It is designed to be a single-source reference for building new applications, specifically organized to support the development of modern web services like AI chat interfaces.

## 1. Setup & Build

### CMake Configuration
Crow is a C++17 framework. Ensure your `CMakeLists.txt` is configured correctly.

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyCrowApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies (Asio is required)
find_package(Threads REQUIRED)
find_package(asio REQUIRED) # Or Boost::asio

add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE Crow::Crow Threads::Threads asio::asio)
```

### Includes
In your C++ files:
```cpp
#include "crow.h"
// Or if using the all-in-one header:
// #include "crow_all.h"
```

## 2. Basic Application Structure

A minimal Crow application consists of an `App` instance, routes, and a run command.

```cpp
#include "crow.h"

int main()
{
    crow::SimpleApp app; // The main application instance

    // Define a simple route
    CROW_ROUTE(app, "/")([](){
        return "Hello, World!";
    });

    // Run the server on port 8080
    app.port(8080).multithreaded().run();
}
```

## 3. Routing

Routes map URLs to C++ lambda functions (handlers).

### Basic Route
```cpp
CROW_ROUTE(app, "/path")
([](){
    return "Response Content";
});
```

### URL Parameters
Crow uses compile-time tag matching or runtime validation for parameters.
Supported types: `<int>`, `<uint>`, `<double>`, `<string>`, `<path>` (matches rest of URL).

```cpp
// Matches /add/1/2
CROW_ROUTE(app, "/add/<int>/<int>")
([](int a, int b){
    return std::to_string(a + b);
});

// Matches /hello/Reid
CROW_ROUTE(app, "/hello/<string>")
([](std::string name){
    return "Hello " + name;
});
```

### HTTP Methods
Default is `GET`. Chain `.methods(...)` to specify others.

```cpp
CROW_ROUTE(app, "/submit")
    .methods("POST"_method)
    ([](const crow::request& req){
        return "Post received";
    });
```

## 4. HTTP Requests & Responses

### Accessing the Request
Add `const crow::request& req` as the first argument to your handler.

```cpp
CROW_ROUTE(app, "/info")
([](const crow::request& req){
    auto user_agent = req.get_header_value("User-Agent");
    auto query_param = req.url_params.get("id"); // ?id=5
    auto body = req.body;
    return "Read request data";
});
```

### Customizing the Response
Return `crow::response` to control status codes and headers, or add `crow::response& res` as an argument (requires explicit `res.end()`).

**Return Style:**
```cpp
CROW_ROUTE(app, "/fail")
([](){
    return crow::response(400, "Bad Request");
});
```

**Argument Style (Async/Complex):**
```cpp
CROW_ROUTE(app, "/async")
([](const crow::request& req, crow::response& res){
    // Do work...
    res.write("Done");
    res.end();
});
```

## 5. JSON Handling

Crow has a built-in JSON parser and emitter, essential for API backends.

### Writing JSON (`wvalue`)
Used to send JSON responses.
```cpp
CROW_ROUTE(app, "/api/status")
([](){
    crow::json::wvalue x;
    x["status"] = "online";
    x["users"] = 55;
    x["list"][0] = 1;
    x["list"][1] = 2;
    
    // Returns Content-Type: application/json automatically
    return x; 
});
```

### Reading JSON (`rvalue`)
Used to parse incoming request bodies.
```cpp
CROW_ROUTE(app, "/api/data")
    .methods("POST"_method)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400); // Parse error
        
        std::string name = x["name"].s();
        int value = x["value"].i();
        
        return crow::response(200);
    });
```

## 6. WebSockets (Real-Time)

Crucial for chat applications. WebSocket routes use a specific macro and event chain.

```cpp
CROW_WEBSOCKET_ROUTE(app, "/ws")
    .onopen([&](crow::websocket::connection& conn){
        CROW_LOG_INFO << "New connection";
    })
    .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
        if (is_binary) return;
        // Echo back
        conn.send_text("You said: " + data);
    })
    .onclose([&](crow::websocket::connection& conn, const std::string& reason){
        CROW_LOG_INFO << "Closed: " << reason;
    });
```

## 7. Serving Content

### Static Files
Place files in a `static/` directory (relative to CWD). Crow serves them automatically at `/static/...`.
To serve explicitly or from root:
```cpp
CROW_ROUTE(app, "/style.css")
([](crow::response& res){
    res.set_static_file_info("static/style.css");
    res.end();
});
```

### Templates (Mustache)
Place HTML files in `templates/`.
```cpp
CROW_ROUTE(app, "/profile/<string>")
([](std::string name){
    crow::mustache::context ctx;
    ctx["user"] = name;
    
    // Loads templates/profile.html
    auto page = crow::mustache::load("profile.html");
    return page.render(ctx);
});
```

## 8. Middleware

Middleware intercepts requests before/after handlers. Useful for Auth, CORS, or Logging.

```cpp
struct CORSMiddleware {
    struct context {}; // Request-local storage

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // No-op
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Access-Control-Allow-Origin", "*");
    }
};

// Register in App
int main() {
    crow::App<CORSMiddleware> app;
    // ...
}
```

## 9. Blueprints (Modularization)

Organize large apps into modules.

```cpp
// Create blueprint
crow::blueprint bp("api");

// Define routes on blueprint
CROW_BP_ROUTE(bp, "/version")
([](){ return "v1.0"; });

// Register blueprint (results in /api/version)
app.register_blueprint(bp);
```

## 10. SSL/HTTPS

Enable secure connections by providing certificate and key files.

```cpp
// .crt and .key files
app.ssl_file("cert.crt", "key.key");

// OR .pem file
// app.ssl_file("cert.pem");

// OR chain file
// app.ssl_chainfile("chain.crt", "key.key");

app.port(443).run();
```
*Note: Requires `CROW_ENABLE_SSL` to be defined in CMake.*

## 11. Multipart Requests (File Uploads)

Handle `multipart/form-data` for file uploads using `crow::multipart::message`.

```cpp
CROW_ROUTE(app, "/upload")
    .methods("POST"_method)
    ([](const crow::request& req){
        crow::multipart::message msg(req);
        
        auto& part = msg.parts[0];
        auto filename = part.get_header_object("Content-Disposition").params["filename"];
        
        // Access body of the part
        std::string content = part.body;
        
        return "File uploaded: " + filename;
    });
```

## 12. Application Configuration

Fine-tune the application behavior before running.

```cpp
crow::SimpleApp app;

app.loglevel(crow::LogLevel::Warning) // Debug, Info, Warning, Error, Critical
   .bindaddr("127.0.0.1")             // Default is 0.0.0.0
   .port(8080)
   .concurrency(4)                    // Number of threads
   .timeout(10)                       // Connection timeout in seconds
   .server_name("MyServer");          // Custom Server header
```

## 13. Catch-All Routes (Custom 404)

Handle requests that don't match any defined route.

```cpp
CROW_CATCHALL_ROUTE(app)
([](const crow::request& req, crow::response& res){
    // Custom 404 logic
    res.code = 404;
    res.write("Resource not found");
    res.end();
});
```

## 14. Compression

Enable automatic HTTP compression (Gzip/Deflate).

```cpp
// Requires CROW_ENABLE_COMPRESSION in CMake
app.use_compression(crow::compression::algorithm::GZIP);
// OR
// app.use_compression(crow::compression::algorithm::DEFLATE);
```
