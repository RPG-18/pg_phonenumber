cmake_minimum_required(VERSION 3.13)
project(pg_phonenumber)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Phonenumber REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Postgres REQUIRED)

include_directories(${PHONENUMBER_INCLUDE_PATH}
        ${PostgreSQL_INCLUDE_DIRS}
)

add_library(phonenumber SHARED src/phonenumber.cpp src/EnumHelper.cpp src/EnumHelper.h)
target_link_libraries(phonenumber ${PHONENUMBER_LIB})

string(REPLACE "/usr/" "" PostgreSQL_SHAREDIR ${PostgreSQL_SHAREDIRS})
string(REPLACE "/usr/" "" PostgreSQL_PKGLIBDIR ${PostgreSQL_PKGLIBDIRS})

install(TARGETS phonenumber
        LIBRARY DESTINATION ${PostgreSQL_PKGLIBDIRS})

install(FILES sql/phonenumber--0.1.sql sql/phonenumber.control
        DESTINATION ${PostgreSQL_SHAREDIR}/extension)