# clipboard-files

A nodejs addon, read or write file paths for clipboard, supports win32 and mac osx.

## Install

```bash
npm i clipboard-files
```

## Building for Electron

```bash
npm i clipboard-files --build-from-source --runtime=electron --target=24.0.0 --target_arch=ia32 --dist-url=https://electronjs.org/headers
```

## Usage

```js
const clipboard = require('clipboard-files')

// for mac
clipboard.writeFiles(['/Users/Alex/Download/helloWorld.js'])

// for win32
clipboard.writeFiles(['C:\\Users\\Alex\\Documents\\helloWorld.js'])

let fileNames = clipboard.readFiles()
```
