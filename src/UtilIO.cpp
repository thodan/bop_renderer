#include "UtilIO.h"
#include "yaml-cpp/yaml.h"


bool readPosesYaml(const std::string filename, std::vector <Pose> &poseStack) {
  YAML::Node n = YAML::LoadFile(filename);
  YAML::Node ests = n["ests"];
  if (ests) {
    for (std::size_t i = 0; i < ests.size(); i++) {
      glm::mat3 R;
      glm::vec3 t;
      float score;

      if (ests[i]["score"] && ests[i]["R"] && ests[i]["t"]) {
        score = ests[i]["score"].as<float>();
        for (unsigned x = 0; x < 3; x++) {
          for (unsigned y = 0; y < 3; y++) {
            R[x][y] = ests[i]["R"][y * 3 + x].as<float>();
          }
        }
        for (unsigned x = 0; x < 3; x++) {
          t[x] = ests[i]["t"][x].as<float>();
        }
      } else {
        std::cerr << "Error: Bad format of a pose estimate." << std::endl;
        return false;
      }

      Pose pose;
      pose.setPose(R, t, score);
      poseStack.push_back(pose);
    }
  } else {
    std::cerr << "Error: The YAML file has no ests field." << std::endl;
    return false;
  }

  return true;
}


std::string loadFile(const std::string file) {
  std::ifstream stream(file.c_str());
  if (stream.fail()) throw std::runtime_error(std::string("Cannot open \'") + file + "\'");
  return std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
}
