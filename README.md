# hinawa

<figure>
  <img src="data/hinawa.png" alt="hinawa" width="60px" height="auto"/>
  <figcaption>Hinawa from Mother 3, after whom this project is named</figcaption>
</figure>

A hobby browser engine to learn how web technologies work.

## Building Hinawa
Before being able to compile Hinawa, you must have a local build of the [skia](https://skia.org/) library. Once you have this, create a file in the root directory of Hinawa called `skia_path.txt` and add the path to your skia build (see `skia_path_example.txt` for an example). Hinawa will use that path during the build process to find necessary libraries and headers.

Once `skia` is built and installed, the other build requirements are:
- cmake
- [fmt](https://github.com/fmtlib/fmt)
- [flex](http://gnu.ist.utl.pt/software/flex/flex.html)

### Building in debug mode
1. `mkdir debug`
2. `cd debug`
3. `cmake ..`
4. `cmake --build . -j $(nproc)`

### Building in release mode
1. `mkdir build`
2. `cd build`
3. `cmake -DCMAKE_BUILD_TYPE=Release ..`
4. `cmake --build . -j $(nproc)`

## JavaScript Engine
The JavaScript engine can additionally be built as a standalone app or library.

### Building in debug mode
1. `mkdir debug`
2. `cd debug`
3. `cmake ../js`
4. `cmake --build . -j $(nproc)`

### Building in release mode
1. `mkdir build`
2. `cd build`
3. `cmake -DCMAKE_BUILD_TYPE=Release ../js`
4. `cmake --build . -j $(nproc)`

For more information about JavaScript, see `js/README.md`

### Screenshots

Default Apache landing page on Debian
![Default apache landing page](data/apache-default.png)

Rendering a raytracer JavaScript HTML canvas demo from [nerget.com](https://nerget.com/rayjs/rayjs.html)
![Raytracer](data/hinawa_raytracer.png)
