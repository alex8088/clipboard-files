# clipboard-files

A nodejs addon, read file path from clipboard, support win32 and mac osx.

## build for nwjs

```shell
npm i clipboard-files --target=0.14.0 --arch=ia32
```
## build for electron

```shell
npm i clipboard-files --target=2.0.5
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
