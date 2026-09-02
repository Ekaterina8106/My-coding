project(RecipeServerLib)

# Исходные файлы библиотеки
set(LIB_SOURCES
    src/server.cpp
)
add_definitions(-DLIBRARY_EXPORT)
# Создание динамической библиотеки, да, какой ужас
add_library(RecipeServer SHARED ${LIB_SOURCES})

# Экспорт символов
target_compile_definitions(RecipeServer PRIVATE LIBRARY_EXPORT)

target_compile_options(RecipeServer PRIVATE -pthread)

target_include_directories(RecipeServer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/ext/nlohmann
    ${CMAKE_SOURCE_DIR}/ext/httplib
)

set_target_properties(RecipeServer PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
