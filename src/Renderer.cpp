#include <iostream>
#include <string>
#include <fstream>

#include "glutils/gl_core_3_3.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "PlyLoader.h"
#include "Renderer.h"


Renderer::Renderer() :
    width(0), height(0), fovy(0.0f),
    lightCamPos(glm::vec3(0.0f, 0.0f, 0.0f)),
    lightColor(glm::vec3(1.0f, 1.0f, 1.0f)),
    lightAmbientWeight(0.5f),
    lightDiffuseWeight(1.0f),
    lightSpecularWeight(0.0f),
    lightSpecularShininess(0.0f) {
}

Renderer::~Renderer() {
//    std::cout << "Destructor..." << std::endl;
  deinit();
}

void Renderer::setLight(glm::vec3 lightCamPos, glm::vec3 lightColor,
                        float lightAmbientWeight, float lightDiffuseWeight,
                        float lightSpecularWeight, float lightSpecularShininess) {
  // OpenCV -> OpenGL coordinate system (flipping Y and Z axis)
  glm::vec3 lightCamPosGl = glm::vec3(
      lightCamPos[0], -lightCamPos[1], -lightCamPos[2]);

  this->lightCamPos = lightCamPosGl;
  this->lightColor = lightColor;
  this->lightAmbientWeight = lightAmbientWeight;
  this->lightDiffuseWeight = lightDiffuseWeight;
  this->lightSpecularWeight = lightSpecularWeight;
  this->lightSpecularShininess = lightSpecularShininess;
}

bool Renderer::init(int width, int height) {
  this->width = width;
  this->height = height;

  if (!context.init(width, height)) {
    std::cerr << "Error initializing GLFW: " << context.getError() << std::endl;
    return false;
  }

  // Init openGL function pointers
  context.makeCurrent();
  if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED) {
    std::cerr << "Error initializing OpenGL functions" << std::endl;
    return false;
  }

  return true;
}

void Renderer::deinit() {
  for (auto &buf: renderers) {
    buf.second->releaseMesh();
    buf.second->releaseResources();
    delete buf.second;
  }
  renderers.clear();
  fileToHandle.clear();

  context.deinit();
}

int Renderer::getWidth() {
  return width;
}

int Renderer::getHeight() {
  return height;
}

bool Renderer::addObject(unsigned int objId, const std::string &filename) {
  // Load ply
  PlyLoader loader;
  if (!loader.loadFile(filename)) {
    std::cerr << "Error loading file " << filename << ": "
              << loader.getError() << std::endl;
    return false;
  }

  GPUBuffer *obj = new GPUBuffer();

  // Initialize renderer resources
  std::string rendererError;
  if (!obj->initResources(width, height, rendererError)) {
    std::cerr << "Error starting renderer: " << rendererError << std::endl;
    return false;
  }
  obj->initMesh(loader.getGeometry());

//    ++handleCounter;
  renderers[objId] = obj;
  fileToHandle[filename] = objId;

  return true;
}

void Renderer::removeObject(unsigned int handle) {
  auto iter = renderers.find(handle);
  if (iter == renderers.end())
    return;

  context.makeCurrent();

  iter->second->releaseMesh();
  delete iter->second;
  renderers.erase(iter);

  // remove element from lookup maps as well
  for (auto iter = fileToHandle.begin(); iter != fileToHandle.end(); ++iter)
    if (iter->second == handle) {
      fileToHandle.erase(iter);
      break;
    }
}

void Renderer::renderObject(
    unsigned int handle, Pose &pose, float fx, float fy, float cx, float cy,
    float skew, float xZero, float yZero) {
  auto iter = renderers.find(handle);
  if (iter == renderers.end()) {
    std::cerr << "Error: Renderer not found." << std::endl;
    return;
  }

  context.makeCurrent();
  glm::mat4 modelTrans = glm::mat4();
  glm::mat4 viewTrans = pose.getPoseHomogen();
  iter->second->render(modelTrans, viewTrans, fx, fy, cx, cy,
                       skew, xZero, yZero, lightCamPos, lightColor,
                       lightAmbientWeight, lightDiffuseWeight,
                       lightSpecularWeight, lightSpecularShininess);
}

void Renderer::getBuffer(unsigned int handle, ObjectAttribute attr, float *data) {
  auto iter = renderers.find(handle);
  if (iter == renderers.end())
    return;

  context.makeCurrent();
  iter->second->getPixelData(attr, data);
}

float *Renderer::getBuffer(unsigned int handle, ObjectAttribute attr) {
  int numComp = 3;
  if (attr == OA_DEPTH) {
    numComp = 1;
  }
  float *data = new float[width * height * numComp];
  getBuffer(handle, attr, data);
  return data;
}

png::image <png::gray_pixel_16> Renderer::getDepthImage(unsigned int handle) {
  float *data = getBuffer(handle, OA_DEPTH);

  png::image <png::gray_pixel_16> grayImage16;
  grayImage16.resize(width, height);

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      if (x < 0 || y < 0 || x > (width - 1) || y > (height - 1)) {
        grayImage16.set_pixel(x, y, png::gray_pixel_16(0));
      } else {
        int pixelIndex = x + width * y;
        grayImage16.set_pixel(x, y, png::gray_pixel_16(
            static_cast<const unsigned short>(data[pixelIndex])));
      }
    }
  }
  return grayImage16;
}

png::image <png::rgb_pixel> Renderer::getColorImage(unsigned int handle) {
  float *data = getBuffer(handle, OA_COLORS);

  png::image <png::rgb_pixel> rgbImage;
  rgbImage.resize(width, height);

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      if (x < 0 || y < 0 || x > (width - 1) || y > (height - 1)) {
        rgbImage.set_pixel(x, y, png::rgb_pixel(0, 0, 0));
      } else {
        int pixelIndex = (x + width * y) * 3;
        rgbImage.set_pixel(x, y, png::rgb_pixel(
            static_cast<png::byte>(data[pixelIndex] * 255.0f),
            static_cast<png::byte>(data[pixelIndex + 1] * 255.0f),
            static_cast<png::byte>(data[pixelIndex + 2] * 255.0f)));
      }
    }
  }
  return rgbImage;
}

void Renderer::debugSave(unsigned int handle, ObjectAttribute attr,
                         const std::string &filename, bool convertToByte) {
  float *data = getBuffer(handle, attr);

  int dataSize = width * height;
  if (attr != OA_DEPTH)
    dataSize *= 3;

  std::ofstream outfile(filename, std::ios::binary);

  if (convertToByte) {
    std::vector<unsigned char> toBytes(dataSize);
    for (int i = 0; i < dataSize; ++i)
      toBytes[i] = static_cast<unsigned char>(data[i] * 255.0f);
    outfile.write(reinterpret_cast<const char *>(toBytes.data()), dataSize);
  } else {
    outfile.write(reinterpret_cast<const char *>(data),
                  dataSize * sizeof(float));
  }

  outfile.close();
  delete[] data;
}

void Renderer::saveImage(unsigned int handle, ObjectAttribute attr,
                         int height, int width, int objectID,
                         const std::string &filename) {
  float *data;

  if (attr == OA_SEG)
    data = getBuffer(handle, OA_DEPTH);
  else
    data = getBuffer(handle, attr);

  if (attr == OA_COLORS || attr == OA_TEXTURED) {
    png::image <png::rgb_pixel> rgbImage;
    rgbImage.resize(width, height);

    for (int x = 0; x < width; ++x)
      for (int y = 0; y < height; ++y) {
        int pixelIndex = (x + width * y) * 3;
        rgbImage.set_pixel(x, y, png::rgb_pixel(
            static_cast<png::byte>(data[pixelIndex] * 255.0f),
            static_cast<png::byte>(data[pixelIndex + 1] * 255.0f),
            static_cast<png::byte>(data[pixelIndex + 2] * 255.0f)));
      }

    rgbImage.write(filename);
    return;
  }

  if (attr == OA_SEG) {
    png::image <png::rgb_pixel> rgbImage;
    rgbImage.resize(width, height);

    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        int pixelIndex = x + width * y;

        auto pixelValue = static_cast<unsigned short>(
            data[pixelIndex] * 1000.0f);
        if (pixelValue == 0) {
          rgbImage.set_pixel(x, y, png::rgb_pixel(
              static_cast<png::byte>(0),
              static_cast<png::byte>(0),
              static_cast<png::byte>(0)));
        } else {
          rgbImage.set_pixel(x, y, png::rgb_pixel(
              static_cast<png::byte>(objectID),
              static_cast<png::byte>(0),
              static_cast<png::byte>(0)));
        }
      }
    }
    rgbImage.write(filename);
    return;
  }

  if (attr == OA_DEPTH) {
    png::image <png::gray_pixel_16> grayImage16;
    grayImage16.resize(width, height);

    for (int x = 0; x < width; ++x)
      for (int y = 0; y < height; ++y) {
        int pixelIndex = x + width * y;
        grayImage16.set_pixel(x, y, png::gray_pixel_16(
            static_cast<const unsigned short>(data[pixelIndex] * 1000.0f)));
      }

    grayImage16.write(filename);
    return;
  }

  if (attr == OA_NORMALS) {
    png::image <png::rgb_pixel_16> rgbImage16;
    rgbImage16.resize(width, height);

    for (int x = 0; x < width; ++x)
      for (int y = 0; y < height; ++y) {
        int pixelIndex = (x + width * y) * 3;
        rgbImage16.set_pixel(x, y, png::rgb_pixel_16(
            static_cast<const unsigned short>(data[pixelIndex] * 1000.0f),
            static_cast<const unsigned short>(data[pixelIndex + 1] * 1000.0f),
            static_cast<const unsigned short>(data[pixelIndex + 2] * 1000.0f)));
      }

    rgbImage16.write(filename);
    return;
  }

  if (attr == OA_LOCALPOS) {
    int channels = 3;

    png::image <png::rgb_pixel_16> rgbImage16;
    rgbImage16.resize(width, height);

    for (int x = 0; x < width; ++x)
      for (int y = 0; y < height; ++y) {
        int pixelIndex = (x + width * y) * 3;
        rgbImage16.set_pixel(x, y, png::rgb_pixel_16(
            static_cast<const unsigned short>(data[pixelIndex] * 1000.0f),
            static_cast<const unsigned short>(data[pixelIndex + 1] * 1000.0f),
            static_cast<const unsigned short>(data[pixelIndex + 2] * 1000.0f)));
      }

    rgbImage16.write(filename);
    return;
  }
}


bool Renderer::isTextureAvailable(unsigned int objId)
{
    return renderers[objId]->isTextureAvailable();
}
