#include <iostream>
#include "Renderer.h"
#include "Pose.h"
#include "UtilIO.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

int main(int argc, char **argv) {

  Renderer renderer;

  int width = 640;
  int height = 480;
  float fx = 572.41140f;
  float fy = 573.57043f;
  float cx = 325.26110f;
  float cy = 242.04899f;

  const std::string modelPath = "/local/datasets/bop/hinterstoisser/models/obj_01.ply";

  if (!renderer.init(width, height)) {
    return -1;
  }

  unsigned int objId = 0;
  renderer.addObject(objId, modelPath);

  std::vector <Pose> poseStack;
  readPosesYaml("data/0000_01_2.yml", poseStack);

  glm::vec3 lightCamPos = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
  float lightAmbientWeight = 0.5f;
  float lightDiffuseWeight = 1.0f;
  float lightSpecularWeight = 1.0f;
  float lightSpecularShininess = 32.0f;

  renderer.setLight(lightCamPos, lightColor, lightAmbientWeight,
                    lightDiffuseWeight, lightSpecularWeight,
                    lightSpecularShininess);
  renderer.renderObject(objId, poseStack[0], fx, fy, cx, cy);

//    png::image<png::gray_pixel_16> depthImage = renderer.getDepthImage(objId);
//    depthImage.write("output/depth.png");

  png::image <png::rgb_pixel> rgbImage = renderer.getColorImage(objId);
  rgbImage.write("output/rgb.png");

//     renderer.saveImage(handle, OA_LOCALPOS, height, width, 1, "coord.png");
//     renderer.saveImage(handle, OA_NORMALS, height, width, 1, "normals.png");
//     renderer.saveImage(handle, OA_COLORS, height, width, 1, "rgb.png");
//     renderer.saveImage(handle, OA_TEXTURED, height, width, 1, "tex.png");
//     renderer.saveImage(handle, OA_DEPTH, height, width, 1, "depth.png");

  // Remove object when no longer needed
  renderer.removeObject(objId);

  return 0;
}
