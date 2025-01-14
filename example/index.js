import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { VRMLoaderPlugin } from '@pixiv/three-vrm';
import { createWLipSyncNode } from 'wlipsync';

// Setup Three.js scene
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
camera.position.set(0, 1.40, 0.5);

const light = new THREE.DirectionalLight('white', 2.5);
light.position.set(2, 4, 3);
scene.add(light);

const renderer = new THREE.WebGLRenderer();
document.body.appendChild(renderer.domElement);

const resize = () => {
  renderer.setSize(window.innerWidth, window.innerHeight);
  camera.aspect = window.innerWidth / window.innerHeight;
  camera.updateProjectionMatrix();
}
window.addEventListener('resize', () => resize())
resize();

// Load avatar model
const loader = new GLTFLoader();
loader.register((parser) => {
  return new VRMLoaderPlugin(parser);
});
const gltf = await loader.loadAsync('./sample-avatar.vrm')
const vrm = gltf.userData.vrm;
const model = gltf.scene;
scene.add(model);

// Load wLipSync profile
const profile = await fetch('./profile.json');
const profileJson = await profile.json();

const audioContext = new AudioContext();
// Create a wLipSync audio node
const lipsyncNode = await createWLipSyncNode(audioContext, profileJson);

renderer.setAnimationLoop(() => {
  for(const key in lipsyncNode.weights) {
    // Apply detected phoneme weights to corresponding expressions (blend shapes)
    const weight = lipsyncNode.weights[key] * lipsyncNode.volume;

    switch(key) {
      case 'A':
        vrm.expressionManager.setValue('aa', weight);
        break;
      case 'O':
        vrm.expressionManager.setValue('oh', weight);
        break;
      case 'E':
        vrm.expressionManager.setValue('ee', weight);
        break;
      case 'U':
        vrm.expressionManager.setValue('ou', weight);
        break;
      case 'I':
        vrm.expressionManager.setValue('ih', weight);
        break;
    }
  }

  vrm.update(0.0);
  renderer.render(scene, camera);
});

// Wait for user interaction before enabling microphone
window.addEventListener('click', async _ => {
  if(audioContext.state === "suspended") {
    audioContext.resume();
  }

  const stream = await navigator.mediaDevices.getUserMedia({
    audio: true,
  });
  const source = audioContext.createMediaStreamSource(stream);

  // Connect the source to the wLipSync audio node.
  // NOTE: The lipsyncNode should not be connected to anything else.
  source.connect(lipsyncNode)
}, { passive: true, once: true })
