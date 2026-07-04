# C HTTP Server

A small HTTP server written from scratch in C for learing sockets, processes, http parsing, routing and file serving.

## Features

- TCP socket server
- Handles HTTP GET requests
- Static file serving from public/
- MIME type detection
- Basic routing
- JSON API endpoints
- 400 Bad Request
- 403 Forbidden
- 404 Not Found
- 405 Method Not Allowed
- 414 URI Too Long
- Fork-based client handling
- SIGCHLD cleanup
- SIGINT graceful shutdown
- Valgrind-testad utan errors
- Logging
- Robust write_all() helper

## Build

make

## Run

./http-server

## Test


curl -i http://localhost:8080/
curl -i http://localhost:8080/about
curl -i http://localhost:8080/api/time
curl -i http://localhost:8080/api/status
curl -i http://localhost:8080/api/info
curl -i -X POST http://localhost:8080/
curl --path-as-is -i http://localhost:8080/../secret
