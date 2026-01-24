#define CROW_USE_BOOST
#define CROW_DISABLE_STATIC_DIR
#define CROW_ENABLE_COMPRESSION
#define CROW_ENABLE_SSL
#define CROW_STATIC_DIRECTORY "static/"

#define SITE "rawsound.com"

#include <iostream>
#include <string>
#include <filesystem>

#include "crow_all.h"

int main() {
	crow::SimpleApp app_rd, app;

	CROW_ROUTE(app_rd, "/<path>")(
		[](std::string name) {
			std::string headerStr = "https://";
			headerStr             += SITE;
			headerStr             += "/";
			headerStr             += name;
			crow::response res(301, headerStr);
			return res;
		});

	CROW_CATCHALL_ROUTE(app_rd)(
		[](crow::response& res) {
			res.end();
		});

	auto _rd = app_rd.port(80).run_async();



	app.use_compression(crow::compression::algorithm::GZIP);

	CROW_ROUTE(app, "/")(
		[](crow::response& res) {
			res.set_static_file_info("static/index.html");
			res.end();
		});

	CROW_ROUTE(app, "/<path>")(
		[](crow::response& res, std::string name) {
			name = "static/" + name;
			res.set_static_file_info(name);
			res.end();
		});

	CROW_CATCHALL_ROUTE(app)(
		[](crow::response& res) {
			res.end();
		});

	if (std::filesystem::exists("cert.crt") && std::filesystem::exists("key.pem")) {
		app.ssl_file("cert.crt", "key.pem");
	} else {
		std::cerr << "Warning: SSL keys (cert.crt, key.pem) not found. Starting without encryption.\n";
	}

	app.port(443).multithreaded().run();
	return 0;
}
