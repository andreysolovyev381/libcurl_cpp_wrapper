//
// Created by Andrey Solovyev on 27/09/2021.
//

#pragma once

#include <vector>
#include <string>

#ifndef UTILS_H
#define UTILS_H

namespace utils {

  std::vector <std::string> splitIntoWords(const std::string &line, char delim);
}

#endif //UTILS_H

