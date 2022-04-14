declare const clipboardFiles: {
  writeFiles(paths: string[]): void;
  readFiles(): string[];
};

export = clipboardFiles;