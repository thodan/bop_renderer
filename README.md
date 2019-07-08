# BOP Renderer

A simple C++ renderer with Python bindings utilized in the BOP Toolkit.

The renderer is based on OSMesa, an off-screen rendering library, which makes it suitable for rendering on servers.

### Dependences

The BOP Renderer depends on [OSMesa](https://www.mesa3d.org/osmesa.html) which requires [LLVM](https://llvm.org/). You can use the provided script *osmesa-install/osmesa-install.sh* to install these libraries (it is a modified version of [osmesa-install](https://github.com/devernay/osmesa-install) -- the changes are documented in *osmesa-install/README.md*).

The installation locations can be set by *osmesaprefix* and *llvmprefix*  in *osmesa-install.sh*. If you do not want to install LLVM, set *buildllvm* to 0 in osmesa-install.sh.

To install the libraries, run (from folder *bop_renderer*):

```
mkdir osmesa-install/build
cd osmesa-install/build
../osmesa-install.sh
```

You will also need to install [yaml-cpp](https://github.com/jbeder/yaml-cpp/).

Moreover, the BOP Renderer depends on the following header-only libraries, which are provided in the folder *3rd* (no installation is required for these libraries): [glm](https://glm.g-truc.net/0.9.9/index.html), [png++](https://www.nongnu.org/pngpp/), [pybind11](https://github.com/pybind/pybind11), [RPly](http://w3.impa.br/~diego/software/rply/).

### Compilation

1. If necessary, modify library paths in *CMakeLists.txt*.
2. Compile by: ```mkdir build; cd build; cmake .. -DCMAKE_BUILD_TYPE=Release; make```

Note: The BOP Renderer was tested on Linux only.

### Samples

- *samples/renderer_minimal.py* - A minimal example on how to use the Python bindings of the BOP Renderer.
- *samples/renderer_test.py* - Comparison of the BOP Renderer and the Python renderer from the BOP Toolkit.
