
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-DUNICODE -D_UNICODE)


if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(installpath Debug)
else()
    set(installpath Release)
endif()

if(WIN32)
        set(installpath ${installpath}/win)            
else()
        set(installpath ${installpath}/linux)            
endif()
option(SIMPLYLIVE_GENERATE_TESTEXE "Enable simplylive generate test app" ON)

option(SIMPLYLIVE_INSTALL_PATH "Enable simplylive install path" ON)

option(SIMPLYLIVE_EXTAL_FIND "Enable FIND LIB IN ${CMAKE_INSTALL_PREFIX} " ON)

if(SIMPLYLIVE_INSTALL_PATH)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/submoduleExport/${installpath}  CACHE PATH "Installation Directory" FORCE)    
	message("CMAKE_INSTALL_PREFIX   ${CMAKE_INSTALL_PREFIX}")
    set(SIMPLYLIVE_EXTAL_FIND OFF)
endif()

option(SIMPLYLIVE_OUTPUT_PATH "Enable simplylive output path" OFF)

if(SIMPLYLIVE_OUTPUT_PATH)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../x64/Debug)
    else()
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../x64/Release)
    endif()   
    message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")    
endif()


if(NOT DEFINED VCPKG_TARGET_TRIPLET)        
    if(WIN32)
        #set(VCPKG_TARGET_TRIPLET "x64-windows-static-md" CACHE STRING "target triplet" FORCE)    
    endif()
    message(STATUS "Target triplet " ${VCPKG_TARGET_TRIPLET})
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

set(CMAKE_PREFIX_PATH   "${CMAKE_SOURCE_DIR}/submodule")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(CMAKE_STATIC_LIBRARY_SUFFIX  "_x86")
else()
    set(CMAKE_STATIC_LIBRARY_SUFFIX  "_x64")
endif()
set(CMAKE_DEBUG_POSTFIX d)

option(SIMPLYLIVE_DISABLE_AUTOUPDATESUBMODULE "Disable auto update submodule features entirely" OFF)

if(SIMPLYLIVE_DISABLE_AUTOUPDATESUBMODULE)
    find_package(Git QUIET)
    if(GIT_FOUND  )
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    else(GIT_FOUND)
        message(FATAL_ERROR "don't found git ")
    endif(GIT_FOUND)
endif()

message(STATUS "simplylive feature: Disable auto update submodule features - ${SIMPLYLIVE_DISABLE_AUTOUPDATESUBMODULE}")

function(auto_update_submodule)
     execute_process(COMMAND ${GIT_EXECUTABLE} submodule foreach git pull origin master
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
endfunction()



function(simplylive_findPack packageName)   
        if(SIMPLYLIVE_EXTAL_FIND)

            message(STATUS "simplylive feature: Enabled find package in ${CMAKE_INSTALL_PREFIX} features ")

            find_package(${packageName})
            if(${packageName}_FOUND)
                message(STATUS  "SIMPLYLIVE_EXTAL_FIND ${packageName} found")
            else(${packageName}_FOUND)
                  message(STATUS  "SIMPLYLIVE_EXTAL_FIND ${packageName} NOT found")
            endif()
        endif()
        if(${packageName}_FOUND)
        else()            
            find_package(${packageName} REQUIRED PATH_SUFFIXES ${installpath}/cmake)  
        endif()
endfunction()


include(CMakeParseArguments)
function(simplylive_install)
#########################################################
##install
# example::::
# simplylive_installX(NAME ${target_name} 
#			HEADERFILES resampleLib.h resampler.h 
#			HEADERDIRECTORY ${CMAKE_SOURCE_DIR}/include/)
#########################################################
 cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "NAME" # list of names of mono-valued arguments
        "HEADERFILES;HEADERDIRECTORY" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
	
	set(packageName ${PARSED_ARGS_NAME})
	string(TOLOWER ${packageName} lowerPackageName)

	install(TARGETS ${packageName} 
		EXPORT ${packageName}-targets 
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
	)
	install(EXPORT ${packageName}-targets
		FILE ${packageName}Config.cmake
		FILE ${lowerPackageName}-config.cmake
		DESTINATION cmake
	)
	 foreach(header ${PARSED_ARGS_HEADERFILES})
		install(FILES ${header} DESTINATION "include")
    endforeach(header)

	
	 foreach(dir ${PARSED_ARGS_HEADERDIRECTORY})		
		install(DIRECTORY ${dir}
				DESTINATION "include"
				FILES_MATCHING
				PATTERN "*.h"
		)
    endforeach(dir)	
endfunction()

