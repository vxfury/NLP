PROJECT(NLP)
CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

IF (NOT TARGET spdlog)
  # Stand-alone build
  FIND_PACKAGE(spdlog REQUIRED)
ENDIF ()

INCLUDE_DIRECTORIES(
  /usr/local/include/ /usr/local/Cellar/spdlog/1.10.0_1/include
)
LINK_DIRECTORIES(/usr/local/lib/ /usr/local/Cellar/spdlog/1.10.0_1/lib)

LINK_LIBRARIES(spdlog::spdlog gtest)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/tokenize ${PROJECT_SOURCE_DIR}/tokenize/jieba
)

ADD_COMPILE_OPTIONS(-O0 -g -std=c++17)

ENABLE_TESTING()
ADD_SUBDIRECTORY(tests/jieba)
