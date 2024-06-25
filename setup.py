import os
from setuptools import setup

def get_version():
    f = open(os.path.join("python", "leomax_tokenizer", "__init__.py"))
    lines = f.readlines()
    version = ""
    for line in lines:
        if line.startswith("__version__"):
            version = line.split("=")[1]
            version = version.strip().replace('"', "")
            break
    return version

if os.name != "nt":
    package_data = {"leomax_tokenizer": ["core_tokenizers.so"]}
    package_data["leomax_tokenizer.libs"] = []

long_description = "leomax-tokenizer Tokenizer Library written in C++ "
setup(
    name="lexomax_tokenizer-python",
    version=get_version(),
    author="xionghaha",
    description=long_description,
    long_description=long_description,
    zip_safe=False,
    package_dir={"": "python"},
    python_requires=">=3.9",
    license="Apache 2.0",
    packages=[
        "leomax_tokenizer",
        "leomax_tokenizer.tokenizer_impl",
        "leomax_tokenizer.models",
        "leomax_tokenizer.normalizers",
        "leomax_tokenizer.pretokenizers"
        ],
    package_data=package_data,
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Operating System :: OS Independent",
        "Intended Audience :: Developers",
        "Intended Audience :: Education",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
    ],
)
