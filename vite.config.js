import { resolve } from 'path'
import { defineConfig } from 'vite'

export default defineConfig({
  server: {
    open: 'example/index.html',
  },
  build: {
    target: 'esnext',
    minify: true,
    lib: {
      entry: resolve(import.meta.dirname, 'www/index.ts'),
      fileName: 'wlipsync',
      formats: ['es'],
    },
  },
  publicDir: 'www/public/'
})