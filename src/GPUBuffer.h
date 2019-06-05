#pragma once

#include <string>
#include "glutils/gl_core_3_3.h"
#include "glutils/GLSLProgram.h"
#include "glutils/FrameBufferObject.h"

#include "glm/glm.hpp"
#include "Geometry.h"

enum ObjectAttribute {
  OA_LOCALPOS,
  OA_NORMALS,
  OA_COLORS,
  OA_TEXTURED,
  OA_DEPTH,
  OA_SEG
};

class GPUBuffer {
public:
  GPUBuffer();

  ~GPUBuffer();

  void initMesh(Geometry *mesh);

  void releaseMesh();

  void render(const glm::mat4 &modelTrans, const glm::mat4 &viewTrans,
              float fx, float fy, float cx, float cy,
              float skew, float xZero, float yZero,
              glm::vec3 lightCamPos, glm::vec3 lightColor,
              float lightAmbientWeight, float lightDiffuseWeight,
              float lightSpecularWeight, float lightSpecularShininess);

  void getPixelData(ObjectAttribute attr, float *data);

//    static bool initResources(int width, int height, std::string &errorString);
//    static void releaseResources();
  bool initResources(int width, int height, std::string &errorString);

  void releaseResources();

private:
//    static GLSLProgram renderProg;
//    static FrameBufferObject fbo;
  GLSLProgram renderProg;
  FrameBufferObject fbo;

  Geometry *mesh;
  std::vector <GLuint> vbos;
  GLuint vao;
  GLuint texture;

  glm::mat4 calculateProjectionMatrix(
      float fx, float fy, float cx, float cy, float skew,
      float xZero, float yZero, const glm::mat4 &mv);
};
