//
// Created by xiongxinlei on 7/4/24.
//
#include <iostream>
#include <Python.h>
#include <pybind11/pybind11.h>

int add(int i, int j) {
    return i + j;
}


PYBIND11_MODULE(core_tokenizers, m) {
    m.doc() = "pybind11 leomax tokenizer core module";
    m.def("add", &add, "A function which adds two number");
}