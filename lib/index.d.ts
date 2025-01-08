declare module "clipboard-files" {
  /**
   * Read file paths from system clipboard
   * @returns Array of file paths
   */
  export function readFiles(): string[];

  /**
   * Write file paths to system clipboard
   * @param files Array of file paths to write to clipboard
   * @throws {TypeError} If files is not an array
   */
  export function writeFiles(filePaths: string[]): void;
}
