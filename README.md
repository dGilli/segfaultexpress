# segfaultexpress

A low level http learning experience in C.

Run server:

```sh
make server && ./server
```

Test:

```sh
curl -v http://localhost:8080
```

## Roadmap:

- [x] Get basic TCP sockets running (listen on ports, accept connections, send/receive data).
- [ ] Build a buffered reader to read requests line-by-line (using CRLF as a delimiter).
- [ ] Parse the first request line to get the method, version, and path.
- [ ] Handle headers in "Header: value" format, including unfolding folded headers.
- [ ] Check method, content type, and size to know how/if to read the body.
- [ ] Decode content based on the content type.
- [ ] Support HTTP/1.1 features like "100 Continue," keep-alive, and chunked transfer.
- [ ] Add security, like handling incomplete requests and limiting client connections.
