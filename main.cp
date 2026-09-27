#include "precompile.h"

constexpr std::string_view HOST       = "rawsound.com";
constexpr std::string_view WEB_ROOT   = "/var/www/rawsound/static/";
constexpr std::string_view CERT_CHAIN = "/etc/letsencrypt/live/rawsound.com/fullchain.pem";
constexpr std::string_view CERT_KEY   = "/etc/letsencrypt/live/rawsound.com/privkey.pem";

int main() {
	crow::SimpleApp app_http;
	crow::SimpleApp app_https;

	app_http.port(80).server_name(std::string{HOST});
	app_https.port(443).server_name(std::string{HOST});

	// HTTP: Redirect all traffic to HTTPS
	CROW_ROUTE(app_http, "/<path>")(
		[](const std::string &path) {
			if (path.find("..") != std::string::npos) {
				return crow::response(400);
			}
			auto           location = std::format("https://{}/{}", HOST, path);
			crow::response res(301);
			res.add_header("Location", location);
			return res;
		});

	CROW_CATCHALL_ROUTE(app_http)(
		[]() {
			auto           location = std::format("https://{}/", HOST);
			crow::response res(301);
			res.add_header("Location", location);
			return res;
		});

	// HTTPS: Serve static files with compression
	app_https.use_compression(crow::compression::algorithm::GZIP);

	CROW_ROUTE(app_https, "/")(
		[](crow::response &res) {
			res.set_static_file_info_unsafe(std::string{WEB_ROOT} + "index.html");
			res.end();
		});

	CROW_ROUTE(app_https, "/<path>")(
		[](crow::response &res, const std::string &path) {
			if (path.find("..") != std::string::npos) {
				res.code = 400;
				res.end();
				return;
			}
			std::string fixablePath = path;
			crow::utility::sanitize_filename(fixablePath);
			res.set_static_file_info_unsafe(std::string{WEB_ROOT} + fixablePath);
			res.end();
		});

	CROW_CATCHALL_ROUTE(app_https)(
		[](crow::response &res) {
			res.code = 404;
			res.end();
		});

	// Configure SSL if certificates exist
	if (std::filesystem::exists(CERT_CHAIN) && std::filesystem::exists(CERT_KEY)) {
		app_https.ssl_chainfile(std::string{CERT_CHAIN}, std::string{CERT_KEY});
	}
	else {
		CROW_LOG_WARNING << "SSL certificates not found. Starting without encryption.";
	}

	// Start servers
	auto http_future  = app_http.concurrency(2).run_async();
	auto https_future = app_https.concurrency(4).run_async();

	app_http.wait_for_server_start();
	app_https.wait_for_server_start();

	http_future.wait();
	https_future.wait();

	return 0;
}
