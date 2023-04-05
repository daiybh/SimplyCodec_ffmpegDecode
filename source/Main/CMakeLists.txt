set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)
set(CMAKE_CUDA_ARCHITECTURES 60)


find_package(CUDA REQUIRED)
simplylive_findPack(DNxGPU)
simplylive_findPack(videoFrame)
simplylive_findPack(videoFrameGPU)
simplylive_findPack(dveGPU)
simplylive_findPack(DnxFrame)


include(FetchFFmpeg)
find_package(FFmpeg REQUIRED)

if(FFMPEG_FOUND)
message(status "ffmpeg found  ${FFMPEG_INCLUDE_DIRS}")
endif()

add_executable(test "ffmpegDecoder.h" "mainX.cpp" "../Lib.TCMarker/picoPNG_test.cpp")

target_link_libraries(test  PUBLIC ${FFMPEG_LIBRARIES} 
                        PUBLIC $<BUILD_INTERFACE:DNxGPU>  
                        PUBLIC $<BUILD_INTERFACE:videoFrame>    
                        PUBLIC $<BUILD_INTERFACE:videoFrameGPU>  
                        PUBLIC $<BUILD_INTERFACE:DVEGPU>    
                        PUBLIC $<BUILD_INTERFACE:DnxFrame>  
                        PUBLIC $<BUILD_INTERFACE:${CUDA_CUDA_LIBRARY}>     
                        PUBLIC tcMarker
						PRIVATE FFmpeg::FFmpeg
)


target_include_directories(test PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
$<BUILD_INTERFACE:${FFMPEG_INCLUDE_DIRS}>)



install(TARGETS test
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
	)
	message("ppppp png ${PROJECT_SOURCE_DIR}/PNG")
		install(DIRECTORY ${PROJECT_SOURCE_DIR}/PNG
				DESTINATION bin
				FILES_MATCHING
				PATTERN "*.png"
		)
