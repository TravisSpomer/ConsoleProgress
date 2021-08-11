# ConsoleProgress

Allows you to show the progress of your scripts in the taskbar, like native Windows file copy operations do.

## Usage

```
Usage:
        ConsoleProgress [ reset | indeterminate | flash | [ normal | warning | error ] [ 0-100 ] ]

Usage examples:

        ConsoleProgress reset         - returns the console to normal
        ConsoleProgress indeterminate - sets the state to indeterminate
        ConsoleProgress normal 25     - sets the state to normal and the progress to 25%
        ConsoleProgress warning       - sets the state to warning without changing the progress
        ConsoleProgress 100           - sets the progress to 100% without changing the state
        ConsoleProgress flash         - flash to get the user's attention
```

## Building

Open the solution file in Visual Studio and press F5. Building from the command line is left as an exercise for the reader.

## Windows Terminal and other virtual terminals

This tool works with the default console host in all versions of Windows through Windows 10. **It does not work with Windows Terminal or other virtual terminals.**

---
© 2014-2021 Travis Spomer. [MIT license](License.txt).
