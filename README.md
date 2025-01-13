# wLipSync
This is a port of the [uLipSync](https://github.com/hecomi/uLipSync) project for use on the web through WASM. Because it implements the same algorithm in the same way, the profiles made for/with uLipSync are compatible with wLipSync.

# Building
The project consists of a C part compiled to WASM and corresponding TypeScript code. Compiling the C code can be done using either `make` or `bun run build:wasm` (which just runs `make`). This requires `clang` and `wasm-ld` to be present in the path.

The typescript part can be compiled by running `bun run build:web`. When both the `wasm` and `web` targets are built, the example/dev page can be used. For publishing the type definitions need to be generated as well using `bun run build:types`.

To build everything in one go, simply run:
```
bun run build
```