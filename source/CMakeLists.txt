cmake_minimum_required (VERSION 3.0)

project(myftp)
set(SERVER_NAME ${PROJECT_NAME}server)

add_definitions('-g')
add_definitions('-Wall')
add_definitions('-std=c++11')

set(SERVER_SRC_LIST server/${SERVER_NAME}.cpp
                    server/worker.cpp
                    server/connection.cpp
                    server/OptionParser.cpp
                    )

add_executable(${SERVER_NAME} ${SERVER_SRC_LIST})

#add_custom_target(${PROJECT_NAME}-symlink ALL ln --force -s ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/${PROJECT_NAME} DEPENDS ${PROJECT_NAME})
#set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME})
