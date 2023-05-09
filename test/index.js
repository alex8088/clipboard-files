const { join } = require('path')
const clipboard = require(`../lib`)
if (process.platform === 'darwin') {
    clipboard.writeFiles(['/Users/Alex/Download/helloWorld.js']); // rewrite an existing path
} else if (process.platform === 'win32') {
    clipboard.writeFiles(join(__dirname, "helloWorld.js")); // rewrite an existing path
}
console.log("files:", clipboard.readFiles());