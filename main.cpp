
#define HOST "rawsound.com"
#define CERT_CHAIN "/etc/letsencrypt/live/rawsound.com/fullchain.pem"
#define CERT_KEY "/etc/letsencrypt/live/rawsound.com/privkey.pem"

#include "precompile.hh"

int main() {
	crow::SimpleApp app_http, app_https;
	app_http.port(80).server_name(HOST);
	app_https.port(443).server_name(HOST);

	CROW_ROUTE(app_http, "/<path>")(
		[](std::string name) {
			std::string headerStr = "https://";
			headerStr             += HOST;
			headerStr             += "/";
			headerStr             += name;
			crow::response res(301, headerStr);
			return res;
		});

	CROW_CATCHALL_ROUTE(app_http)(
		[]() {
			std::string headerStr = "https://";
			headerStr             += HOST;
			headerStr             += "/";
			crow::response res(301, headerStr);
			return res;
		});


	app_https.use_compression(crow::compression::algorithm::GZIP);

	CROW_ROUTE(app_https, "/")(
		[](crow::response& res) {
			res.set_static_file_info("static/index.html");
			res.end();
		});

	CROW_ROUTE(app_https, "/<path>")(
		[](crow::response& res, std::string name) {
			name = "static/" + name;
			res.set_static_file_info(name);
			res.end();
		});

	CROW_CATCHALL_ROUTE(app_https)(
		[](crow::response& res) {
			res.end();
		});

	if (std::filesystem::exists(CERT_CHAIN) && std::filesystem::exists(CERT_KEY)) {
		app_https.ssl_chainfile(CERT_CHAIN, CERT_KEY);
	}
	else {
		CROW_LOG_WARNING << "SSL keys (fullchain.pem, privkey.pem) not found. Starting without encryption.\n";
	}


	auto http_server  = app_http.concurrency(2).run_async();
	auto https_server = app_https.concurrency(4).run_async();

	// Wait for servers to start
	app_http.wait_for_server_start();
	app_https.wait_for_server_start();

	// Keep the main thread alive
	http_server.wait();
	https_server.wait();

	return 0;
}
