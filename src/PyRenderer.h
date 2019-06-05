#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "Renderer.h"

namespace py = pybind11;


class PyRenderer {
public:
  PyRenderer();

  ~PyRenderer();

  bool init(int width, int height);

  void setLight(std::array<float, 3> lightCamPos,
                std::array<float, 3> lightColor,
                float lightAmbientWeight, float lightDiffuseWeight,
                float lightSpecularWeight, float lightSpecularShininess);

  bool addObject(unsigned int objId, const std::string &filename);

  void removeObject(unsigned int handle);

  void renderObject(unsigned int handle,
                    std::array<float, 9> R,
                    std::array<float, 3> t,
                    float fx, float fy, float cx, float cy,
                    float skew, float xZero, float yZero);

  py::array getColorImage(unsigned int handle);

  py::array getDepthImage(unsigned int handle);

private:
  Renderer renderer;
};


// Interface for Python.
PYBIND11_MODULE(bop_renderer, m) {
  py::class_<PyRenderer>(m, "PyRenderer")
    .def (py::init())
    .def("init", &PyRenderer::init)
    .def("set_light", &PyRenderer::setLight)
    .def("add_object", &PyRenderer::addObject)
    .def("remove_object", &PyRenderer::removeObject)
    .def("render_object", &PyRenderer::renderObject,
      py::arg("obj_id"),
      py::arg("R"),
      py::arg("t"),
      py::arg("fx"),
      py::arg("fy"),
      py::arg("cx"),
      py::arg("cy"),
      py::arg("skew")=0.0f,
      py::arg("x_xero")=0.0f,
      py::arg("y_zero")=0.0f)
    .def("get_depth_image", &PyRenderer::getDepthImage)
    .def("get_color_image", &PyRenderer::getColorImage);
}
