# Opixen Graphics Demo

This project is a simple graphics demonstration using the custom "Opixen" engine. It displays a moving reticle (circle) that changes color on the screen.

## Dependencies

*   GLFW [included]
*   GLAD [included]
*   GLM [included]
*   Opixen (custom engine) [included]

## Building

This project uses a `build.bat` script for compilation on Windows. Ensure u have `g++` installed and configured correctly,

I have already included necessary libraries (GLFW, GLAD, GLM).
Update them **if u know what u are doing.**

To build the project, run the following command in the project's root directory:

```
./build.bat
```

## Bugs 
 - Have To Alias Quotation content in params (Havn't Done That Yet.).
 - Surface Haven't yet created properly, Not Even initialized

## Running

After successful compilation, the executable (`HelloWorld.exe`) will be located in the `release` directory. Navigate to the `release` directory and run the executable:

```
cd release
start HelloWorld
```

## Controls

*   **Escape:** Close the application window.

## Features

The Opixen engine provides the following features, as demonstrated in this demo:

*   **Window Creation:** Creates a window with a specified title, width, and height.
*   **Clear Frame:** Clears the frame buffer.
*   **Set Clear Color:** Sets the background color (clear color) of the window.
*   **Set Color:** Sets the color of individual pixels.
*   **End Frame:** Handles end-of-frame operations (likely buffer swapping).
*   **Error Handling:** Provides error handling and reporting.
*   **String Operations:** Provides substantial level of string operations.
*   **GLFW Integration:** Uses GLFW for window management and input handling.
*   **GLM Integration:** Uses GLM for vector and matrix mathematics.
