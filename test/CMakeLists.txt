find_package(GTest CONFIG REQUIRED)

enable_testing()

configure_file(../source/resources/external_control.script ${PROJECT_BINARY_DIR}/test/ COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/example/resource/input_recipe.txt ${PROJECT_BINARY_DIR}/test/ COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/example/resource/output_recipe.txt ${PROJECT_BINARY_DIR}/test/ COPYONLY)

file(GLOB SOURCES *.cpp)

foreach(SOURCE ${SOURCES})
    get_filename_component(ELITE_SDK_TEST_NAME ${SOURCE} NAME_WE)
    add_executable(${ELITE_SDK_TEST_NAME} ${SOURCE})
    target_include_directories(
        ${ELITE_SDK_TEST_NAME} 
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
        ${PROJECT_SOURCE_DIR}/include/Common
        ${PROJECT_SOURCE_DIR}/include/Elite
        ${PROJECT_SOURCE_DIR}/include/Control
    )
    target_link_libraries(
        ${ELITE_SDK_TEST_NAME}
        elite-cs-series-sdk::static
        GTest::gtest
        ${SYSTEM_LIB}
    )
    target_link_directories(
        ${ELITE_SDK_TEST_NAME} 
        PRIVATE ${CMAKE_BINARY_DIR}
    )
endforeach()
