# RawSound

A minimal static file web server built with [Crow](https://crowcpp.org/) (C++20). Serves files from the `static/` directory over HTTPS with automatic HTTP-to-HTTPS redirection.

## Building

Requires C++20, Boost, and OpenSSL.

## Deployment on Debian 13

### Prerequisites

Install dependencies:

```bash
apt install g++ libboost-all-dev libssl-dev zlib1g-dev
```

### Let's Encrypt Certificates

Follow the [Certbot instructions](https://certbot.eff.org/instructions) to obtain certificates for your domain.

Certificates are expected at:
- `/etc/letsencrypt/live/rawsound.com/fullchain.pem`
- `/etc/letsencrypt/live/rawsound.com/privkey.pem`

### Installation

Clone the repository:

```bash
git clone https://github.com/diskerror/RawSound.git
cd RawSound
```

My domain/host name is hard coded. Edit `main.cp`, `Makefile`, and `rawsound.service` to replace "rawsound" with 
your domain/host name, and update the paths to your certificate files.

Build:

```bash
make
```

### Running

Copy or make a symbolic link of `rawsound.service` into `/etc/systemd/system/`.


```bash
systemctl daemon-reload
systemctl enable rawsound
systemctl start rawsound
```

The service runs as `www-data` with security hardening (read-only filesystem, capability-bounded port binding). Be
sure the certificate files are readable by `www-data`.

## License

[Boost Software License 1.0](https://www.boost.org/LICENSE_1_0.txt)
