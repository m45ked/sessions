add_subdirectory(tests)

add_library(sqlite_ext STATIC
    sqlite/sqlite3.c
)
target_include_directories(sqlite_ext PUBLIC sqlite)

if (UNIX AND (CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
  set_target_properties(sqlite_ext PROPERTIES COMPILE_FLAGS "-fPIC")
  target_compile_definitions(sqlite_ext PUBLIC SQLITE_ENABLE_NORMALIZE)
endif()


add_library(database
    Connection.cpp
    Connection_fwd.h
    Connection.h
    Exceptions.h
    isTableExist.cpp
    isTableExist.h
    Query.cpp
    Query_fwd.h
    Query.h
)
target_link_libraries(database sqlite_ext spdlog::spdlog)
generate_export_header(database)
# install(TARGETS database DESTINATION ${LIBRARY_INSTALL_DIR})
