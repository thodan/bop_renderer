#pragma once

#include "Pose.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iterator>
#include "dirent.h"
#include <algorithm>


bool readPosesYaml(const std::string filename, std::vector <Pose> &poseStack);

// Load a file and return it as std::string
std::string loadFile(const std::string file);
