cmake_minimum_required(VERSION 2.8.3)
project(aiui_test)
add_definitions(-std=c++11)
SET(CMAKE_C_COMPILER "/usr/bin/gcc")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -g -D_GLIBCXX_USE_CXX11_ABI=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -D_GLIBCXX_USE_CXX11_ABI=0")
string(REGEX REPLACE "(.*)/(.*)/(.*)" "\\1" PROJECT_INIT_PATH  ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_INIT_PATH}/bin)

include_directories(
  src
  ../../include
  ../../include/hid
  /usr/include/json-c                      
  /usr/lib/libusb/include/libusb-1.0       
)
link_directories(
  wheeltec_robot/src/xf_mic_asr_offine/lib/arm64
)

# add_library(aiui_demo_lib SHARED
#   src/AIUITest.cpp
#   src/FileUtil.cpp
#   src/jsoncpp/json_reader.cpp
#   src/jsoncpp/json_writer.cpp
#   src/jsoncpp/json_value.cpp
# )
# target_link_libraries(aiui_demo_lib aiui msc pthread hid_lib asound)

# # 
add_executable(aiui_sample src/aiuiMain.cpp)
target_link_libraries(aiui_sample AIUITester) # hid_lib

# audio_player_test
# add_executable(audio_player_test src/audio_player_test.cpp)
# target_link_libraries(audio_player_test audio_player) 

# audio_player     
add_library(audio_player SHARED
  src/AudioPlayer.cpp
)
target_link_libraries(audio_player asound)

## audio_player_test
# add_executable(audio_player_test src/audio_player_test.cpp)
# target_link_libraries(audio_player_test audio_player) 

# ## audio_recorder
add_library(audio_recorder SHARED
  src/AudioRecorder.cpp
)
target_link_libraries(audio_recorder hid_lib msc)
# ## audio_recorder_test
# add_executable(audio_recorder_test src/audio_recorder_test.cpp)
# target_link_libraries(audio_recorder_test audio_recorder) 


# add_library(WriteAudioThread SHARED
#   src/WriteAudioThread.cpp
# )
# target_link_libraries(WriteAudioThread aiui pthread)

# add_library(TestListener SHARED
#   src/TestListener.cpp
# )
# target_link_libraries(TestListener aiui)


# add_library(AIUITester SHARED
#   src/AIUITester.cpp
# )
# target_link_libraries(AIUITester aiui TestListener WriteAudioThread audio_recorder)
add_library(AIUITester SHARED
  # src/AudioRecorder.cpp
  src/AIUITester.cpp
  src/TestListener.cpp
  src/WriteAudioThread.cpp
  src/FileUtil.cpp
)
target_link_libraries(AIUITester aiui audio_recorder audio_player)

