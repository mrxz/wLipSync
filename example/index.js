import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { createWLipSyncNode } from 'wlipsync';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 1000 );
camera.position.set(0, 1.65, 0.5);
const loader = new GLTFLoader();

const renderer = new THREE.WebGLRenderer();
renderer.setSize( window.innerWidth, window.innerHeight );
document.body.appendChild( renderer.domElement );

const ambientLight = new THREE.AmbientLight('white', 1.0);
scene.add(ambientLight);

const gltf = await loader.loadAsync('./6784e76a9d91f0cde7abf269-2.glb')
const model = gltf.scene;
scene.add(model);

const teeth = model.getObjectByName('Wolf3D_Teeth');
const head = model.getObjectByName('Wolf3D_Head');
const morphTargetDictionary = head.morphTargetDictionary;
const morphTargetInfluences = head.morphTargetInfluences;
teeth.morphTargetInfluences = morphTargetInfluences;

// wLipSync
const profile = await fetch('./profile.json');
const profileJson = await profile.json();

const audioContext = new AudioContext();
const lipsyncNode = await createWLipSyncNode(audioContext, profileJson);

renderer.setAnimationLoop(() => {
  // Update blend shapes
  const maxBlendShapeValue = 1.0;
  const bsMaxWeight = 1.0;

  for(const key in morphTargetDictionary) {
    morphTargetInfluences[morphTargetDictionary[key]] = 0.0;
  }

  for(const key in lipsyncNode.weights) {
    const weight = lipsyncNode.weights[key] * bsMaxWeight * lipsyncNode.volume * maxBlendShapeValue

    switch(key) {
      case 'A':
        morphTargetInfluences[morphTargetDictionary['viseme_aa']] = weight;
        break;
      case 'O':
        morphTargetInfluences[morphTargetDictionary['viseme_O']] = weight;
        break;
      case 'E':
        morphTargetInfluences[morphTargetDictionary['viseme_E']] = weight;
        break;
      case 'U':
        morphTargetInfluences[morphTargetDictionary['viseme_U']] = weight;
        break;
      case 'I':
        morphTargetInfluences[morphTargetDictionary['viseme_I']] = weight;
        break;
      default:
        break;
    }
  }

  renderer.render(scene, camera);
});

window.addEventListener('click', async _ => {
  if (audioContext.state === "suspended") {
    audioContext.resume();
  }

  const stream = await navigator.mediaDevices.getUserMedia({
    audio: true,
  });
  const source = audioContext.createMediaStreamSource(stream);
  source.connect(lipsyncNode)
}, { passive: true, once: true })

