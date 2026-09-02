project(ServerApp)

# Исходные файлы приложения
set(APP_SOURCES
    src/main_server.cpp
)

# Основной сервер
add_executable(server ${APP_SOURCES})
target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/server/lib/inc
    ${CMAKE_SOURCE_DIR}/ext/nlohmann
    ${CMAKE_SOURCE_DIR}/ext/httplib
)
target_link_libraries(server PRIVATE
    RecipeServer
    pthread
)

# Сервер для самотестирования
add_executable(server_selftest ${APP_SOURCES})
target_compile_options(server_selftest PRIVATE -g --coverage)
target_link_libraries(server_selftest PRIVATE
   RecipeServer
    gcov
    pthread
)

# Копирование данных для основного сервера
add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/data/data.json
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/data.json
    COMMENT "Copying data.json"
)

# Копирование тестовых данных для самотестирования
add_custom_command(TARGET server_selftest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_data
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/data/data.json
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_data/
    COMMENT "Copying test data for selftest"
)
