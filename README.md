# clipboard-files

A nodejs addon, read file path from clipboard, support win32 and mac osx.

## build for nwjs

```shell
npm i clipboard-files --build-from-source --runtime=node-webkit --target=0.42.0 --target_arch=x64
```
## build for electron

```shell
npm i clipboard-files --build-from-source --runtime=electron --target=7.1.2 --target_arch=ia32 --dist-url=https://atom.io/download/electron
```

## build for nodejs

```shell
npm i clipboard-files
```

## example

```js
const clipboard = require('clipboard-files')
let fileNames = clipboard.readFiles()
```
