#-----------------------------------------
# CMakeLists Skeleton 2020/03/21
#-----------------------------------------
cmake_minimum_required(VERSION 3.11.4)
project(turing_machine_solver LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 20)

set(MY_SOURCE_FILES
        include/ask.h
        include/candidate.h
        include/checker_func.h
        include/checker_if.h
        include/potential_checkers.h
        include/solver.h
        include/checker_base.h
   )


if(${ENABLE_CUDA_CODE})
    set (MY_CPP_FLAGS -DENABLE_CUDA_CODE)
    set (MY_SOURCE_FILES ${MY_SOURCE_FILES}
         src/CUDA_backtracker.cu
         src/CUDA_common_glutton.cu
         src/CUDA_glutton_max.cu
         src/CUDA_glutton_wide.cu
         src/CUDA_piece_position_info_base.cu
        )
else(${ENABLE_CUDA_CODE})
    set (MY_CPP_FLAGS -pedantic)
endif(${ENABLE_CUDA_CODE})

# List header directories in project
set(MY_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
   )

#Create dependancy list
set(DEPENDANCY_LIST "")
LIST(APPEND DEPENDANCY_LIST "quicky_exception")
LIST(APPEND DEPENDANCY_LIST "quicky_utils")
LIST(APPEND DEPENDANCY_LIST "combinatorics")

#------------------------------
#- Generic part
#------------------------------
# Search for CUDA files
foreach(SOURCE_FILE IN ITEMS ${MY_SOURCE_FILES})
    if(${SOURCE_FILE} MATCHES "(.*\).cu$")
        set(CMAKE_CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})
        enable_language(CUDA)
        include(CheckLanguage)
        check_language(CUDA)
        if(NOT DEFINED CMAKE_CUDA_STANDARD)
            set(CMAKE_CUDA_STANDARD 11)
            set(CMAKE_CUDA_STANDARD_REQUIRED ON)
            set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --keep --keep-dir=${CMAKE_BINARY_DIR}/${CMAKE_FILES_DIRECTORY} --source-in-ptx --ptxas-options=-v -arch=sm_75 -gencode=arch=compute_75,code=sm_75")
            if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		# With some versions of nvcc -G give internal compiler error
		set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -G")
		#set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} ")
            else()
                set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -lineinfo")
            endif()
        endif()
    endif()
endforeach(SOURCE_FILE)

set(GENERATED_FILES "")

# Search for Bison files
foreach(SOURCE_FILE IN ITEMS ${MY_SOURCE_FILES})
    if(${SOURCE_FILE} MATCHES "(.*\).ypp$")
        get_filename_component(BISON_NAME ${CMAKE_MATCH_1} NAME)
        message("Bison parser detected: " ${BISON_NAME})
        set(BISON_OUTPUT ${BISON_NAME}.hpp ${BISON_NAME}.cpp)
        string(FIND "$GENERATED_FILES" location.hh GENERIC_BISON_OK)
        if(GENERIC_BISON_OK EQUAL -1)
            set(BISON_OUTPUT ${BISON_OUTPUT} location.hh position.hh stack.hh)
            message("Add generic bison files")
        endif()
        add_custom_command(OUTPUT ${BISON_OUTPUT}
                           COMMAND bison -o ${BISON_NAME}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}
                           DEPENDS ${SOURCE_FILE}
                           COMMENT "Generate Bison parser for \"${SOURCE_FILE}\""
                           )
        set(GENERATED_FILES ${GENERATED_FILES} ${BISON_OUTPUT})
    endif()
endforeach(SOURCE_FILE)

#Search for flex files
foreach(SOURCE_FILE IN ITEMS ${MY_SOURCE_FILES})
    if(${SOURCE_FILE} MATCHES "(.*\).ll$")
        get_filename_component(FLEX_NAME ${CMAKE_MATCH_1} NAME)
        message("Flex lexer detected: " ${FLEX_NAME})
        set(FLEX_OUTPUT ${FLEX_NAME}.h ${FLEX_NAME}.cpp)
        add_custom_command(OUTPUT ${FLEX_OUTPUT}
                           COMMAND flex -o${FLEX_NAME}.cpp --header-file=${FLEX_NAME}.h ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}
                           DEPENDS ${SOURCE_FILE}
                           COMMENT "Generate Flex lexer for \"${SOURCE_FILE}\""
                           )
        set(GENERATED_FILES ${GENERATED_FILES} ${FLEX_OUTPUT})
    endif()
endforeach(SOURCE_FILE)

if(GENERATED_FILES)
    set(MY_INCLUDE_DIRECTORIES
        ${MY_INCLUDE_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}
       )
endif()

set(DEPENDANCY_OBJECTS "")
set(LINKED_LIBRARIES "-lpthread")
#iterate on dependancies
foreach(DEPENDANCY_ITEM IN ITEMS ${DEPENDANCY_LIST})
    message("${PROJECT_NAME} need dependancy ${DEPENDANCY_ITEM}")
    set(IS_IMPORTED FALSE)
    #Get information from dependancies if not already done
    if(NOT TARGET ${DEPENDANCY_ITEM})
        set(SUB_DEPENDANCY_OBJECTS "")
        set(SUB_LINKED_LIBRARIES "")
        add_subdirectory($ENV{QUICKY_REPOSITORY}/${DEPENDANCY_ITEM} ${CMAKE_CURRENT_BINARY_DIR}/${DEPENDANCY_ITEM})
        get_target_property(IS_IMPORTED ${DEPENDANCY_ITEM} IMPORTED)
        if(NOT ${IS_IMPORTED})
            list(APPEND DEPENDANCY_OBJECTS "$<TARGET_OBJECTS:${DEPENDANCY_ITEM}>" ${SUB_DEPENDANCY_OBJECTS})
        else()
            list(APPEND LINKED_LIBRARIES ${DEPENDANCY_ITEM})
        endif()
        list(APPEND LINKED_LIBRARIES ${SUB_LINKED_LIBRARIES})
    endif()

    # Complete include directories with directories coming from dependancy because of transitivity issues with Object libraries
    if(NOT ${IS_IMPORTED})
        set(MY_INCLUDE_DIRECTORIES ${MY_INCLUDE_DIRECTORIES} $<TARGET_PROPERTY:${DEPENDANCY_ITEM},INCLUDE_DIRECTORIES>)
    else(NOT ${IS_IMPORTED})
        set(MY_INCLUDE_DIRECTORIES ${MY_INCLUDE_DIRECTORIES} $<TARGET_PROPERTY:${DEPENDANCY_ITEM},INTERFACE_INCLUDE_DIRECTORIES>)
    endif()

endforeach(DEPENDANCY_ITEM)


#Prepare targets
get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(IS_DIRECTORY ${HAS_PARENT})
    message("Declare library ${PROJECT_NAME}")
    add_library(${PROJECT_NAME} OBJECT ${MY_SOURCE_FILES} ${GENERATED_FILES})
    set(SUB_DEPENDANCY_OBJECTS ${DEPENDANCY_OBJECTS} PARENT_SCOPE)
    set(SUB_LINKED_LIBRARIES ${LINKED_LIBRARIES} PARENT_SCOPE)
else()
    #set(CMAKE_VERBOSE_MAKEFILE ON)
#    string(REPLACE " " ";" DEPENDANCY_OBJECTS ${DEPENDANCY_OBJECTS})
    add_executable(${PROJECT_NAME} ${MY_SOURCE_FILES} ${DEPENDANCY_OBJECTS} src/main.cpp)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -D$<UPPER_CASE:${PROJECT_NAME}>_SELF_TEST)
    message(Linked librarries ${LINKED_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${LINKED_LIBRARIES})
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall $<$<CONFIG:Debug>:-O0> ${MY_CPP_FLAGS})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${MY_INCLUDE_DIRECTORIES})

foreach(DEPENDANCY_ITEM IN ITEMS ${DEPENDANCY_LIST})
    add_dependencies(${PROJECT_NAME} ${DEPENDANCY_ITEM})
endforeach(DEPENDANCY_ITEM)

#Force use of -std=c++11 instead of -std=gnu++11
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

if(DEFINED CMAKE_CUDA_HOST_COMPILER)
    set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
    function(CUDA_CONVERT_FLAGS EXISTING_TARGET)
        get_property(old_flags TARGET ${EXISTING_TARGET} PROPERTY COMPILE_OPTIONS)
        if(NOT "${old_flags}" STREQUAL "")
            get_property(old_cxx_standard TARGET ${EXISTING_TARGET} PROPERTY CXX_STANDARD)
            string(REPLACE ";" "," CUDA_flags "${old_flags}")
            # C++ standard set to 11 and not propagated due to issues when compiling with -std=c++17
            set_property(TARGET ${EXISTING_TARGET} PROPERTY COMPILE_OPTIONS
		                 "$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CXX>>:${old_flags}>$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=${CUDA_flags}$<$<BOOL:${old_cxx_standard}>:,-std=c++11>$<$<CONFIG:Debug>:,-g>>"
                        )
        endif()
    endfunction()
    CUDA_CONVERT_FLAGS(${PROJECT_NAME})
endif(DEFINED CMAKE_CUDA_HOST_COMPILER)
#EOF
