declare module "clipboard-files" {
  /**
   * Read file paths from system clipboard
   * @returns Array of file paths
   */
  export function readFiles(): string[];

  /**
   * Write file paths to system clipboard
   * @param files Array of file paths to write to clipboard
   * 
   * @platform darwin,win32
   */
  export function writeFiles(filePaths: string[]): void;
}
