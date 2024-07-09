mkdir -p build_py39

for py_version in 9; do
    echo build_py3${py_version}
    cd build_py3${py_version}
    #rm -rf *
    platform="$(uname -s)"
    echo "platform: ${platform}"

    export LD_LIBRARY_PATH=/Users/admin/opt/anaconda3/lib/:${LD_LIBRARY_PATH}
    export PATH=/Users/admin/opt/anaconda3/bin:${PATH}
    if [[ $platform == Linux* ]]; then
      core_num=`lscpu | grep "^CPU(s)"  | awk '{print $NF}'`
    else
      core_num=`sysctl -n hw.logicalcpu`
    fi

    echo "compile with ${core_num} cores"

    cmake ../ -DWITH_PYTHON=ON -DWITH_TESTING=OFF -DCMAKE_BUILD_TYPE=Debug
    make -j${core_num}
    if [[ $? == 0 ]]; then
        echo "compile success"
    else
        echo "compile failed"
    fi
done