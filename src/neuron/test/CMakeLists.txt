#INCLUDE(Test)
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 
SET (TARGET_NAME NEURON_TEST)

ADD_EXECUTABLE (${TARGET_NAME}
    main.cpp
    neural_gas_neuron_test.cpp
    soinn_neuron_test.cpp
)

TARGET_LINK_LIBRARIES (${TARGET_NAME}
    weight_vector
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

ADD_TEST (NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
#                  DEPENDS ${TARGET_NAME})
