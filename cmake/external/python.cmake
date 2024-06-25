include(python_module)

find_package(PythonInterp ${PY_VERSION} REQUIRED)
find_package(PythonLibs ${PY_VERSION} REQUIRED)
message(STATUS "Python interpreter: ${PY_VERSION}")
