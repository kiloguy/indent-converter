# Indent Converter

A tool that convert indent style (spaces to tabs, or inversely) of all files under current directory.

### Usage
```
Usage: indent-converter {t2s | s2t} SPACES_PER_TAB EXT ...
       indent-converter --help

       {t2s | s2t}     # Mode selection. "Tabs to spaces" or "spaces to tabs".
       SPACES_PER_TAB  # Specify how many spaces should be transform to a tab. Should be a positive integer.
       EXT ...         # Specify file extensions to process. At least one extension should be specified. (ex: .js .html)
```

### Example
[Angular](https://angular.io)'s official CLI tool to generate template code files use 2 spaces indent as default.
To transform the indent of all files under Angular project folder to tabs.

```
$ cd /your-angular-project/
$ indent-converter s2t 2 .ts .js .html .css .json
```

(This example is also the purpose that why I made this tool.)
