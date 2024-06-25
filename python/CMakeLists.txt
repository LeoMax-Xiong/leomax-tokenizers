# 将 python 的 tokenizer 目录拷贝到二进制目录

message(STATUS "source directory: ${CMAKE_SOURCE_DIR}/python")
message(status "target directory: ${CMAKE_BINARY_DIR}/python")
add_custom_target(copy_python_tokenizers ALL 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/python ${CMAKE_BINARY_DIR}/python DEPENDS core_tokenizers)


# 拷贝主目录下的 setup.py
add_custom_target(copy_setup ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/setup.py ${CMAKE_BINARY_DIR}/setup.py)


# 将 core_tokenizer.so 拷贝到 tokenizers 目录
set(TOKENIZER_CORE_NAME "core_tokenizers")
set(TOKENIZER_DST_DIR ${CMAKE_BINARY_DIR}/python/leomax_tokenizer)
set(TOKENIZER_SRC_DIR ${CMAKE_BINARY_DIR}/leomax_tokenizer)
set(TOKENIZER_THIRD_PARTY_DIR ${CMAKE_BINARY_DIR}/third_party)

add_custom_target(copy_shared_library ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${TOKENIZER_SRC_DIR}/${TOKENIZER_CORE_NAME}.so ${TOKENIZER_DST_DIR}
        DEPENDS copy_python_tokenizers)