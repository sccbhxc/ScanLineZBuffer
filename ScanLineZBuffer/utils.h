#ifndef __UTILS_H
#define __UTILS_H

#include<cmath>
#include <io.h>
#include <string>
#include <vector>
#include <iostream>



const float EPS = 1e-6;

bool isEqualf(float a, float b);

std::vector<std::string> getListFiles(std::string path, std::string suffix);


#endif