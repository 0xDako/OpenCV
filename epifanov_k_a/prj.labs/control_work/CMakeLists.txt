project(control_work)
add_executable(control_work control_work.cpp)
target_link_libraries(control_work ${OpenCV_LIBS})

file(READ control_work.cpp cpp_source)
# prepare report md file
#configure_file(control_work.report.md.in.txt ${CMAKE_CURRENT_BINARY_DIR}/lab04.report.md @ONLY)
#add_custom_command(TARGET ${PROJECT_NAME}
#  POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/control_work.report.md" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")