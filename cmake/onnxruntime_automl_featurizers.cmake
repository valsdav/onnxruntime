# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.
# This source code should not depend on the onnxruntime and may be built independently

file(GLOB onnxruntime_automl_featurizers_srcs CONFIGURE_DEPENDS
    "${ONNXRUNTIME_ROOT}/core/automl/featurizers/src/FeaturizerPrep/*.h"
    "${ONNXRUNTIME_ROOT}/core/automl/featurizers/src/FeaturizerPrep/Featurizers/*.h"
    "${ONNXRUNTIME_ROOT}/core/automl/featurizers/src/FeaturizerPrep/Featurizers/*.cpp"
)

source_group(TREE ${ONNXRUNTIME_ROOT}/core/automl/ FILES ${onnxruntime_automl_featurizers_srcs})

add_library(onnxruntime_automl_featurizers ${onnxruntime_automl_featurizers_srcs})

target_include_directories(onnxruntime_automl_featurizers PRIVATE ${ONNXRUNTIME_ROOT} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(onnxruntime_automl_featurizers PROPERTIES FOLDER "AutoMLFeaturizers")

# Individual featurizers unit tests added at bulk
file(GLOB onnxruntime_automl_featurizers_tests_srcs
    "${ONNXRUNTIME_ROOT}/core/automl/featurizers/src/FeaturizerPrep/Featurizers/UnitTests/*.cpp"
)

list(APPEND onnxruntime_automl_featurizers_tests_srcs
    "${ONNXRUNTIME_ROOT}/core/automl/featurizers/src/FeaturizerPrep/UnitTests/Traits_UnitTests.cpp"
    "${ONNXRUNTIME_ROOT}/core/automl/featurizers/src/FeaturizerPrep/UnitTests/Featurizer_UnitTest.cpp"
    "${ONNXRUNTIME_ROOT}/core/automl/featurizers/src/FeaturizerPrep/UnitTests/test_main.cpp"
)

add_executable(featurizers_unittests ${onnxruntime_automl_featurizers_tests_srcs})
add_dependencies(featurizers_unittests onnxruntime_automl_featurizers)
target_link_libraries(featurizers_unittests PRIVATE gtest onnxruntime_automl_featurizers)
source_group(TREE ${ONNXRUNTIME_ROOT}/core/automl/ FILES ${onnxruntime_automl_featurizers_tests_srcs})
set_target_properties(featurizers_unittests PROPERTIES FOLDER "AutoMLFeaturizers")
add_test(NAME featurizers_unittests
    COMMAND featurizers_unittests
    WORKING_DIRECTORY $<TARGET_FILE_DIR:featurizers_unittests>
)


if (WIN32)
    # Add Code Analysis properties to enable C++ Core checks. Have to do it via a props file include.
    set_target_properties(onnxruntime_automl_featurizers PROPERTIES VS_USER_PROPS ${PROJECT_SOURCE_DIR}/ConfigureVisualStudioCodeAnalysis.props)
endif()
