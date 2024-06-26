# mkdir -p build_py39

for py_version in 9; do
    echo build_py3${py_version}
    cd build_py3${py_version}
    #rm -rf *
    platform="$(uname -s)"
    echo "platform: ${platform}"
    # if [[ $platform == Linux* ]]; then

    # if
    export LD_LIBRARY_PATH=/home/users/xiongxinlei/opt/anaconda3/lib/:${LD_LIBRARY_PATH}
    export PATH=/home/users/xiongxinlei/opt/anaconda3/bin/:${PATH}
    export PATH=/home/users/xiongxinlei/opt/cmake-3.25.1-linux-x86_64/bin/:${PATH}
    core_num=`lscpu | grep "^CPU(s)"  | awk '{print $NF}'`
    echo "compile with ${core_num} cores"

    cmake ../ -DWITH_PYTHON=ON -DWITH_TESTING=OFF
    make -j${core_num}
    if [[ $? == 0 ]]; then
        echo "compile success"
    else
        echo "compile failed"
    fi
done