#include "crow_all.h"

int main()
{
	// HTTP app on port 80
	crow::SimpleApp http_app;
	CROW_ROUTE(http_app, "/")([](){
		return "HTTP response";
	});

	// HTTPS app on port 443
	crow::SimpleApp https_app;
	CROW_ROUTE(https_app, "/")([](){
		return "HTTPS response";
	});

	// Start both servers asynchronously
	auto http_server = http_app.bindaddr("0.0.0.0").port(80).run_async();
	auto https_server = https_app.bindaddr("0.0.0.0")
								   .port(443)
								   .ssl_file("cert.crt", "key.key")
								   .run_async();

	// Wait for servers to start
	http_app.wait_for_server_start();
	https_app.wait_for_server_start();

	// Keep the main thread alive
	http_server.wait();
	https_server.wait();
}