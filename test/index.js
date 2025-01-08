const path = require("path");
const clipboard = require("../lib");
clipboard.writeFiles([
  path.resolve(__dirname, "..", "build", "Release", "binding.node"),
]);
console.log("files:", clipboard.readFiles());
