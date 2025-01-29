# wLipSync
[![npm version](https://img.shields.io/npm/v/wlipsync.svg?style=flat-square)](https://www.npmjs.com/package/wlipsync)
[![npm version](https://img.shields.io/npm/l/wlipsync.svg?style=flat-square)](https://www.npmjs.com/package/wlipsync)
[![github](https://flat.badgen.net/badge/icon/github?icon=github&label)](https://github.com/mrxz/wlipsync/)
[![twitter](https://flat.badgen.net/badge/twitter/@noerihuisman/blue?icon=twitter&label)](https://x.com/noerihuisman)
[![mastodon](https://flat.badgen.net/badge/mastodon/@noerihuisman@arvr.social/blue?icon=mastodon&label)](https://arvr.social/@noerihuisman)
[![ko-fi](https://img.shields.io/badge/ko--fi-buy%20me%20a%20coffee-ff5f5f?style=flat-square)](https://ko-fi.com/fernsolutions)

A MFCC-based lip sync library for WebAudio using WASM. This is a port of the [uLipSync](https://github.com/hecomi/uLipSync) project.

<img alt="A-Painter" target="_blank" src="https://github.com/user-attachments/assets/e1df1cef-0e53-4651-88af-e771be454530" width="40%">
<img alt="A-Painter" target="_blank" src="https://github.com/user-attachments/assets/f615d53b-0e71-497c-8c22-f3f8b7dae19b" width="40%">

# Usage
Either install the package from [npm](https://www.npmjs.com/package/wlipsync) or load it using import maps:
```HTML
<script type="importmap">
  {
    "imports": {
      "wlipsync": "https://cdn.jsdelivr.net/npm/wlipsync/dist/wlipsync-single.js"
    }
  }
</script>
```

The main entrypoint is a single file which has the WASM binary and audio worklet processor code inlined, meaning no further steps are required to initialize the library. See the following code on how to use the library:
```JS
// Import wLipSync
import { createWLipSyncNode } from 'wlipsync';

// Create Audio context
const audioContext = new AudioContext();

// Load wLipSync profile
const profile = await fetch('./profile.json').then(resp => resp.json());

// Create lip sync node
const lipsyncNode = await createWLipSyncNode(audioContext, profile);

// Connect audio source (e.g. audio file, microphone, etc...) to lipsync node
source.connect(lipsyncNode);

// ...

// Read current phoneme state in update/animation loop
console.log(lipsyncNode.weights, lipsyncNode.volume);
```

For a full example using [Three.js](https://threejs.org/) and a VRM avatar, see the [`./example/`](https://github.com/mrxz/wLipSync/tree/main/example) folder.

> [!IMPORTANT]  
> Since the project uses AudioWorklets it requires a secure context (either `localhost` or `https://`).

## Creating a profile
A profile is required for **wLipSync** to work. This project does not come with a way to create these. Instead they can be made in Unity using uLipSync by following their [Calibration instructions](https://github.com/hecomi/uLipSync#calibration).

## Using binary profiles
By default the profiles are stored in JSON format. While these files tend to compress nicely, a more compact binary representation is also supported by **wLipSync**. This format not only packs the data, it also stores precomputed values which would otherwise be computed at runtime when loading a profile.

To convert a JSON profile into a binary profile, the `json2bin` utility in the [`tools/`](https://github.com/mrxz/wLipSync/tree/main/tools) directory can be used. This script is also included in the published NPM package, meaning you can run it from your project using the following command:
```sh
node ./node_modules/wlipsync/tools/json2bin.js path/to/profile.json profile.bin
```

To load a binary profile, the following change needs to be made to the code sample above:
```diff
// Import wLipSync
-import { createWLipSyncNode } from 'wlipsync';
+import { createWLipSyncNode, parseBinaryProfile } from 'wlipsync';

// ...

// Load wLipSync profile
-const profile = await fetch('./profile.json').then(resp => resp.json());
+const binaryProfile = await fetch('./profile.bin').then(resp => resp.arrayBuffer());
+const profile = parseBinaryProfile(binaryProfile);

// Create lip sync node
const lipsyncNode = await createWLipSyncNode(audioContext, profile);
```

# Building
The project consists of a C part compiled to WASM and corresponding TypeScript code. Everything can be compiled by running `bun run build` as this will execute all relevant commands in the right order. The following tools are expected to be present on the system:

* clang
* wasm-ld
* wasm-opt (from [binaryen](https://github.com/WebAssembly/binaryen))
* make

# Development
For testing during development, perform a full build:
```
bun run build
```

Now you can start the dev server, which should listen for changes to any file:
```
bun run
```

> [!NOTE]  
> Currently the dev server shows the example which uses the `wlipsync-single.js` file. As this file inlines the minimized `audio-processor.js` and optimized `wlipsync.wasm` a full build is required to observe changes.
