//
// Created by xiongxinlei on 5/26/24.
//

#ifndef PYBIND_TOKEN_H
#define PYBIND_TOKEN_H
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace leomax_tokenizer {
namespace pybind {
class PyToken {
public:
  void tokenizer(const std::string& tokens) {
    std::cout << "PyModel tokenizer" << std::endl;
  }

};

}
}
#endif  // PYBIND_TOKEN_H
