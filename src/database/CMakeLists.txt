add_subdirectory(tests)

add_library(sqlite_ext STATIC
    sqlite/sqlite3.c
)
target_include_directories(sqlite_ext PUBLIC sqlite)

add_library(database
    Connection.cpp
    Query.cpp
)
target_link_libraries(database sqlite_ext fmt::fmt)
generate_export_header(database)
install(TARGETS database DESTINATION ${LIBRARY_INSTALL_DIR})
