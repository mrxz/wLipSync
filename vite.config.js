import { resolve } from 'path'
import { defineConfig } from 'vite'

export default defineConfig({
  server: {
    open: 'example/index.html',
  },
  build: {
    target: 'esnext',
    lib: {
      entry: resolve(__dirname, 'www/index.ts'),
      fileName: 'wlipsync',
      formats: ['es'],
    },
  }
})