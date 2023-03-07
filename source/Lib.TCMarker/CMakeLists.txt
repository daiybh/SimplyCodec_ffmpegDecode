

file(GLOB source_cpp "./*.cpp")
add_library (tcMarker ${source_cpp} "../Lib.Base/locker.cpp" )



target_include_directories(tcMarker 
			PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
			PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/includes/
			PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/ajaapi/
			PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/classes/

)