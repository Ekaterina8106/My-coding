project(ClientApp)

# Исходные файлы
set(CLIENT_SOURCES
    src/client.cpp
    src/main_client.cpp
)

# Основной клиент
add_executable(client ${CLIENT_SOURCES})
target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/ext/nlohmann
    ${CMAKE_SOURCE_DIR}/ext/httplib
)
target_link_libraries(client PRIVATE pthread)

# Клиент для самотестирования
add_executable(client_selftest ${CLIENT_SOURCES})
target_compile_options(client_selftest PRIVATE -g --coverage)
target_link_libraries(client_selftest PRIVATE pthread gcov)

# Копирование тестовых данных
add_custom_command(TARGET client_selftest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_data
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/data/data.json
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_data/
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/tests/test_commands.txt
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    COMMENT "Copying test data"
)

# Установка выходной директории
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
