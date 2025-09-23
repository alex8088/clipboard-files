# clipboard-files

A cross-platform Node.js native addon for reading or writing file paths to the clipboard. Supports Windows, MacOS and Linux.

## Install

```bash
npm i clipboard-files
```

## Usage

```js
const clipboard = require("clipboard-files");

// Read file paths from clipboard
const files = clipboard.readFiles();
console.log("Files in clipboard:", files);

// Write file paths to clipboard
clipboard.writeFiles(["/path/to/file1.txt", "/path/to/file2.txt"]);
```

## API

### readFiles()

Returns an array of strings containing file paths from the clipboard.

### writeFiles(files: string[])

Writes an array of file paths to the clipboard.

- `files`: Array of strings, each representing a file path

## Platform Support

- Windows
- macOS
- Linux (X11)
  - Requires X11 development libraries
  - Install on Ubuntu/Debian: `sudo apt-get install libx11-dev`
  - Install on Fedora: `sudo dnf install libX11-devel`

## License

MIT
