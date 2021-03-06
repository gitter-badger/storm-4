add_custom_target(resources)
add_custom_target(test-resources)

set(STORM_3RDPARTY_SOURCE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty)
set(STORM_3RDPARTY_BINARY_DIR ${PROJECT_BINARY_DIR}/resources/3rdparty)


#############################################################
##
##	l3pp
##
#############################################################

# Do not take a branch, needs internet connection.
ExternalProject_Add(
        l3pp_ext
        GIT_REPOSITORY https://github.com/hbruintjes/l3pp.git
        GIT_TAG e4f8d7fe6c328849aff34d2dfd6fd592c14070d5
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/l3pp
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_INSTALL ON
)
ExternalProject_Get_Property(l3pp_ext source_dir)
set(l3pp_INCLUDE "${source_dir}/")
add_imported_library_interface(l3pp "${l3pp_INCLUDE}")
list(APPEND STORM_DEP_TARGETS l3pp)
add_dependencies(l3pp l3pp_ext)

#############################################################
##
##	gmm
##
#############################################################

add_imported_library_interface(gmm "${PROJECT_SOURCE_DIR}/resources/3rdparty/gmm-5.2/include")
list(APPEND STORM_DEP_TARGETS gmm)

#############################################################
##
##	Eigen
##
#############################################################

add_imported_library_interface(StormEigen33 "${PROJECT_SOURCE_DIR}/resources/3rdparty/eigen-3.3-beta1")
list(APPEND STORM_DEP_TARGETS StormEigen33)


#############################################################
##
##	Boost
##
#############################################################

# Boost Option variables
set(Boost_USE_STATIC_LIBS        ${USE_BOOST_STATIC_LIBRARIES})
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.61.0 QUIET REQUIRED COMPONENTS filesystem system)
if (NOT Boost_FOUND)
    if (Boost_VERSION)
	message(FATAL_ERROR "The required Boost version is 1.61.0 or newer, however, only ${Boost_VERSION} was found.")
    else ()
   	message(FATAL_ERROR "Boost was not found.")
    endif ()
endif ()
if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
    set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
endif ()

set(CNTVAR 1)
foreach(BOOSTLIB ${Boost_LIBRARIES})
    add_imported_library(target-boost-${CNTVAR} SHARED ${BOOSTLIB} ${Boost_INCLUDE_DIRS})
    list(APPEND STORM_DEP_TARGETS target-boost-${CNTVAR}_SHARED)
    MATH(EXPR CNTVAR "${CNTVAR}+1")
endforeach()
message(STATUS "Storm - Using boost ${Boost_VERSION} (library version ${Boost_LIB_VERSION}).")
# set the information for the config header
set(STORM_BOOST_INCLUDE_DIR "${Boost_INCLUDE_DIRS}")

#############################################################
##
##	ExprTk
##
#############################################################

# Use the shipped version of ExprTK
message (STATUS "Storm - Including ExprTk.")
add_imported_library_interface(ExprTk "${PROJECT_SOURCE_DIR}/resources/3rdparty/exprtk")
list(APPEND STORM_DEP_TARGETS ExprTk)

#############################################################
##
##	Sparsepp
##
#############################################################

# Use the shipped version of Sparsepp
message (STATUS "Storm - Including Sparsepp.")
set(SPARSEPP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/sparsepp/sparsepp")
file(GLOB SPARSEPP_HEADERS "${SPARSEPP_INCLUDE_DIR}/*.h")

# Add the sparsepp headers to the headers that are copied to the include directory in the build directory.
set(SPARSEPP_BINDIR_DIR ${CMAKE_BINARY_DIR}/include/resources/3rdparty/sparsepp)
include_directories("${SPARSEPP_BINDIR_DIR}")
foreach(HEADER ${SPARSEPP_HEADERS})
    string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
    string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
    string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
    add_custom_command(
        OUTPUT ${SPARSEPP_BINDIR_DIR}/sparsepp/${HEADER_FILENAME}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SPARSEPP_BINDIR_DIR}/sparsepp
        COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${SPARSEPP_BINDIR_DIR}/sparsepp/${HEADER_FILENAME}
        DEPENDS ${SPARSEPP_INCLUDE_DIR}/${HEADER_FILENAME}
    )
    list(APPEND SPARSEPP_BINDIR_HEADERS ${SPARSEPP_BINDIR_DIR}/sparsepp/${HEADER_FILENAME})
endforeach()

#############################################################
##
##	ModernJSON
##
#############################################################

#use the shipped version of modernjson
message (STATUS "Storm - Including ModernJSON.")
add_imported_library_interface(ModernJSON "${PROJECT_SOURCE_DIR}/resources/3rdparty/modernjson/src/")
list(APPEND STORM_DEP_TARGETS ModernJSON)

#############################################################
##
##	Z3 (optional)
##
#############################################################

find_package(Z3 QUIET)

# Z3 Defines
set(STORM_HAVE_Z3 ${Z3_FOUND})

if(Z3_FOUND)
    # Check whether the version of z3 supports optimization
    if (Z3_EXEC)
        execute_process (COMMAND ${Z3_EXEC} -version
                OUTPUT_VARIABLE z3_version_output
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        if (z3_version_output MATCHES "([0-9]*\\.[0-9]*\\.[0-9]*)")
            set(Z3_VERSION "${CMAKE_MATCH_1}")
            if(NOT "${Z3_VERSION}" VERSION_LESS "4.5.0")
                set(STORM_HAVE_Z3_OPTIMIZE ON)
            endif()
        endif()
    endif()

    if(STORM_HAVE_Z3_OPTIMIZE)
        message (STATUS "Storm - Linking with Z3. (Z3 version supports optimization)")
    else()
        message (STATUS "Storm - Linking with Z3. (Z3 version does not support optimization)")
    endif()

    add_imported_library(z3 SHARED ${Z3_LIBRARIES} ${Z3_INCLUDE_DIRS})
    list(APPEND STORM_DEP_TARGETS z3_SHARED)
else()
    message (WARNING "Storm - Z3 not found. Building of Prism/JANI models will not be supported.")
endif(Z3_FOUND)

#############################################################
##
##	glpk
##
#############################################################

include(${STORM_3RDPARTY_SOURCE_DIR}/include_glpk.cmake)

#############################################################
##
##	Gurobi (optional)
##
#############################################################

if (STORM_USE_GUROBI)
    find_package(Gurobi QUIET REQUIRED)
    set(STORM_HAVE_GUROBI ${GUROBI_FOUND})
    if (GUROBI_FOUND)
        message (STATUS "Storm - Linking with Gurobi.")
        add_imported_library(Gurobi SHARED ${GUROBI_LIBRARY} ${GUROBI_INCLUDE_DIRS})
		list(APPEND STORM_DEP_TARGETS Gurobi_SHARED)
    endif()
else()
    set(STORM_HAVE_GUROBI OFF)
endif()

#############################################################
##
##	CUDD
##
#############################################################
include(${STORM_3RDPARTY_SOURCE_DIR}/include_cudd.cmake)

#############################################################
##
##      carl
##
#############################################################

set(STORM_HAVE_CARL OFF)
set(CARL_MINYEAR 17)
set(CARL_MINMONTH 08)
set(CARL_MINPATCH 0)
if (NOT STORM_FORCE_SHIPPED_CARL)
    if (NOT "${STORM_CARL_DIR_HINT}" STREQUAL "")
		find_package(carl QUIET PATHS ${STORM_CARL_DIR_HINT} NO_DEFAULT_PATH)
    endif()
	if (NOT carl_FOUND)
		find_package(carl QUIET)
    endif()
endif()
if(carl_FOUND AND NOT STORM_FORCE_SHIPPED_CARL)
    get_target_property(carlLOCATION lib_carl LOCATION)
    if(${carlLOCATION} STREQUAL "carlLOCATION-NOTFOUND")
        message(SEND_ERROR "Library location for carl is not found, did you build carl?")
    elseif(EXISTS ${carlLOCATION})
        #empty on purpose
    else()
        message(SEND_ERROR "File ${carlLOCATION} does not exist, did you build carl?")
    endif()
    if("${carl_MINORYEARVERSION}" STREQUAL "" OR "${carl_MINORMONTHVERSION}" STREQUAL "" OR "${carl_MAINTENANCEVERSION}" STREQUAL "")
        # don't have detailed version information, probably an old version of carl
        message(FATAL_ERROR "Carl at ${carlLOCATION} outdated, require ${CARL_MINYEAR}.${CARL_MINMONTH}.${CARL_MINPATCH}, have ${carl_VERSION}")
    endif()
    if(${carl_MINORYEARVERSION} LESS ${CARL_MINYEAR})
        message(SEND_ERROR "Carl outdated, require ${CARL_MINYEAR}.${CARL_MINMONTH}.${CARL_MINPATCH}, have ${carl_VERSION}")
    elseif(${carl_MINORYEARVERSION} EQUAL ${CARL_MINYEAR})
        if(${carl_MINORMONTHVERSION} LESS ${CARL_MINMONTH})
            message(SEND_ERROR "Carl outdated, require ${CARL_MINYEAR}.${CARL_MINMONTH}.${CARL_MINPATCH}, have ${carl_VERSION}")
         elseif(${carl_MINORMONTHVERSION} EQUAL ${CARL_MINMONTH})
            if(${carl_MAINTENANCEVERSION} LESS ${CARL_MINPATCH})
                message(SEND_ERROR "Carl outdated, require ${CARL_MINYEAR}.${CARL_MINMONTH}.${CARL_MINPATCH}, have ${carl_VERSION}")
            endif()
        endif()
    endif()

    set(STORM_SHIPPED_CARL OFF)
    set(STORM_HAVE_CARL ON)
    message(STATUS "Storm - Use system version of carl.")
    message(STATUS "Storm - Linking with preinstalled carl ${carl_VERSION} (include: ${carl_INCLUDE_DIR}, library ${carl_LIBRARIES}, CARL_USE_CLN_NUMBERS: ${CARL_USE_CLN_NUMBERS}, CARL_USE_GINAC: ${CARL_USE_GINAC}).")
    set(STORM_HAVE_CLN ${CARL_USE_CLN_NUMBERS})
    set(STORM_HAVE_GINAC ${CARL_USE_GINAC})
else()
	set(STORM_SHIPPED_CARL ON)
	# The first external project will be built at *configure stage*
	message("START CARL CONFIG PROCESS")
	file(MAKE_DIRECTORY ${STORM_3RDPARTY_BINARY_DIR}/carl_download) 
	execute_process(
	COMMAND ${CMAKE_COMMAND} ${STORM_3RDPARTY_SOURCE_DIR}/carl "-DSTORM_3RDPARTY_BINARY_DIR=${STORM_3RDPARTY_BINARY_DIR}" "-DBoost_LIBRARY_DIRS=${Boost_LIBRARY_DIRS}" "-DBoost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}"
	WORKING_DIRECTORY ${STORM_3RDPARTY_BINARY_DIR}/carl_download
	OUTPUT_VARIABLE carlconfig_out
	RESULT_VARIABLE carlconfig_result)
								
	if(NOT carlconfig_result)
		message("${carlconfig_out}")
	endif()
	execute_process(
		COMMAND ${CMAKE_COMMAND} --build . --target carl-config
		WORKING_DIRECTORY ${STORM_3RDPARTY_BINARY_DIR}/carl_download
		OUTPUT_VARIABLE carlconfig_out
		RESULT_VARIABLE carlconfig_result
		)
		if(NOT carlconfig_result)
		message("${carlconfig_out}")
	endif()
    message("END CARL CONFIG PROCESS")
    	
    message(STATUS "Storm - Using shipped version of carl.")
    ExternalProject_Add(
            carl
            SOURCE_DIR ${STORM_3RDPARTY_BINARY_DIR}/carl
            CONFIGURE_COMMAND ""
            BUILD_IN_SOURCE 1
            BUILD_COMMAND make lib_carl
            INSTALL_COMMAND make install
            LOG_BUILD ON
			LOG_INSTALL ON
            BUILD_BYPRODUCTS ${STORM_3RDPARTY_BINARY_DIR}/carl/lib/libcarl${DYNAMIC_EXT}
    )
    include(${STORM_3RDPARTY_BINARY_DIR}/carl/carlConfig.cmake)

    set(STORM_HAVE_CLN ${CARL_USE_CLN_NUMBERS})
    set(STORM_HAVE_GINAC ${CARL_USE_GINAC})

	add_dependencies(resources carl)
    set(carl_INCLUDE_DIR "${STORM_3RDPARTY_BINARY_DIR}/carl/include/")
	set(carl_LIBRARIES ${STORM_3RDPARTY_BINARY_DIR}/carl/lib/libcarl${DYNAMIC_EXT})
    set(STORM_HAVE_CARL ON)

    message(STATUS "Storm - Linking with shipped carl ${carl_VERSION} (include: ${carl_INCLUDE_DIR}, library ${carl_LIBRARIES}, CARL_USE_CLN_NUMBERS: ${CARL_USE_CLN_NUMBERS}, CARL_USE_GINAC: ${CARL_USE_GINAC}).")
        
    # install the carl dynamic library if we built it
    install(FILES ${STORM_3RDPARTY_BINARY_DIR}/carl/lib/libcarl.${carl_VERSION}${DYNAMIC_EXT} DESTINATION lib)
endif()

if(STORM_USE_CLN_RF AND NOT STORM_HAVE_CLN)
	message(FATAL_ERROR "Cannot use CLN numbers if carl is build without.")
endif()
if(STORM_USE_CLN_RF AND NOT STORM_HAVE_GINAC)
    message(FATAL_ERROR "Cannot use CLN numbers if carl is build without ginac.")
endif()

# The library that needs symbols must be first, then the library that resolves the symbol.
list(APPEND STORM_DEP_IMP_TARGETS lib_carl)
if(STORM_USE_CLN_EA OR STORM_USE_CLN_RF)
    list(APPEND STORM_DEP_IMP_TARGETS GINAC_SHARED CLN_SHARED)
endif()
list(APPEND STORM_DEP_IMP_TARGETS GMPXX_SHARED GMP_SHARED)


#############################################################
##
##      SMT-RAT
##
#############################################################

set(STORM_HAVE_SMTRAT OFF)
if(USE_SMTRAT)
	find_package(smtrat QUIET REQUIRED)
	if(smtrat_FOUND)
		set(STORM_HAVE_SMTRAT ON)
		message(STATUS "Storm - Linking with smtrat.")
    	include_directories("${smtrat_INCLUDE_DIR}")
    	list(APPEND STORM_LINK_LIBRARIES ${smtrat_LIBRARIES})
	else()
		message(FATAL_ERROR "Storm - SMT-RAT was requested but not found")
	endif()
endif()


#############################################################
##
##      HyPro
##
#############################################################

set(STORM_HAVE_HYPRO OFF)
if(USE_HYPRO)
	find_package(hypro QUIET REQUIRED)
	if(hypro_FOUND)
		set(STORM_HAVE_HYPRO ON)
		message(STATUS "Storm - Linking with hypro.")
		include_directories("${hypro_INCLUDE_DIR}")
		list(APPEND STORM_LINK_LIBRARIES ${hypro_LIBRARIES})
	else()
		message(FATAL_ERROR "Storm - HyPro was requested but not found")
	endif()
endif()

#############################################################
##
##	gmp
##
#############################################################
#
#find_package(GMP QUIET REQUIRED)
#


get_target_property(GMPXX_LIB GMPXX_SHARED IMPORTED_LIB_LOCATION)
get_target_property(GMP_LIB GMP_SHARED IMPORTED_LIB_LOCATION)
get_target_property(GMPXX_INCLUDE_DIR GMPXX_SHARED INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(GMP_INCLUDE_DIR GMP_SHARED INTERFACE_INCLUDE_DIRECTORIES)
get_filename_component(GMP_LIB_LOCATION ${GMP_LIB} DIRECTORY)
get_filename_component(GMPXX_LIB_LOCATION ${GMPXX_LIB} DIRECTORY)



#message(STATUS "Storm - Linking with gmp.")
#include_directories("${GMP_INCLUDE_DIR}")
#list(APPEND STORM_LINK_LIBRARIES ${GMP_LIBRARY})

#############################################################
##
##	MathSAT (optional)
##
#############################################################

if ("${MSAT_ROOT}" STREQUAL "")
    set(ENABLE_MSAT OFF)
else()
    set(ENABLE_MSAT ON)
endif()

# MathSAT Defines
set(STORM_HAVE_MSAT ${ENABLE_MSAT})
if (ENABLE_MSAT)
    message (STATUS "Storm - Linking with MathSAT.")
    find_library(MSAT_LIB mathsat PATHS "${MSAT_ROOT}/lib")
    add_imported_library(msat SHARED ${MSAT_LIB} "${MSAT_ROOT}/include")
    list(APPEND STORM_DEP_TARGETS msat_SHARED)
endif(ENABLE_MSAT)

#############################################################
##
##	Xerces
##
#############################################################

include(${STORM_3RDPARTY_SOURCE_DIR}/include_xerces.cmake)

#############################################################
##
##	Sylvan
##
#############################################################

if(STORM_SHIPPED_CARL)
    set(sylvan_dep carl)
else()
    set(sylvan_dep lib_carl)
endif()

if (STORM_DEBUG_SYLVAN)
    set(SYLVAN_BUILD_TYPE "Debug")
else()
    set(SYLVAN_BUILD_TYPE "Release")
endif()

ExternalProject_Add(
        sylvan
        DOWNLOAD_COMMAND ""
        PREFIX "sylvan"
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/sylvan
        CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DGMP_LOCATION=${GMP_LIB_LOCATION}  -DGMP_INCLUDE=${GMP_INCLUDE_DIR}  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DSYLVAN_BUILD_DOCS=OFF -DSYLVAN_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=${SYLVAN_BUILD_TYPE} -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DUSE_CARL=ON -Dcarl_INCLUDE_DIR=${carl_INCLUDE_DIR} -DSYLVAN_PORTABLE=${STORM_PORTABLE} -Dcarl_LIBRARIES=${carl_LIBRARIES} -DBUILD_SHARED_LIBS=OFF -DSYLVAN_BUILD_TESTS=OFF
        BINARY_DIR ${STORM_3RDPARTY_BINARY_DIR}/sylvan
        BUILD_IN_SOURCE 0
        INSTALL_COMMAND ""
        INSTALL_DIR ${STORM_3RDPARTY_BINARY_DIR}/sylvan
        LOG_CONFIGURE ON
        LOG_BUILD ON
        DEPENDS ${sylvan_dep}
        BUILD_BYPRODUCTS ${STORM_3RDPARTY_BINARY_DIR}/sylvan/src/libsylvan${STATIC_EXT}
)

ExternalProject_Get_Property(sylvan source_dir)
ExternalProject_Get_Property(sylvan binary_dir)
set(Sylvan_INCLUDE_DIR "${source_dir}/src")
set(Sylvan_LIBRARY "${binary_dir}/src/libsylvan${STATIC_EXT}")
message(STATUS "Storm - Using shipped version of sylvan.")
message(STATUS "Storm - Linking with sylvan.")
add_imported_library(sylvan STATIC ${Sylvan_LIBRARY} ${Sylvan_INCLUDE_DIR})
add_dependencies(sylvan_STATIC sylvan)

list(APPEND STORM_DEP_TARGETS sylvan_STATIC)

find_package(Hwloc QUIET REQUIRED)
if(HWLOC_FOUND)
    message(STATUS "Storm - Linking with hwloc ${HWLOC_VERSION}.")
    add_imported_library(hwloc STATIC ${HWLOC_LIBRARIES} "")
    list(APPEND STORM_DEP_TARGETS hwloc_STATIC)
else()
    if(${OPERATING_SYSTEM} MATCHES "Linux")
        message(FATAL_ERROR "HWLOC is required on Linux but was not found.")
    endif()
endif()

#############################################################
##
##	cpptemplate
##
#############################################################

add_imported_library_interface(CppTemplate "${STORM_3RDPARTY_SOURCE_DIR}/cpptemplate/")
list(APPEND STORM_DEP_TARGETS CppTemplate)
list(APPEND STORM_3RDPARTY_SOURCES ${STORM_3RDPARTY_SOURCE_DIR}/cpptemplate/cpptempl.cpp)

#############################################################
##
##	Google Test gtest
##
#############################################################
set(GOOGLETEST_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/gtest-1.7.0)
ExternalProject_Add(
        googletest
        #For downloads (may be useful later!)
        #SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
        #TIMEOUT 10
        DOWNLOAD_COMMAND ""
        SOURCE_DIR "${STORM_3RDPARTY_SOURCE_DIR}/gtest-1.7.0"
        # Force the same output paths for debug and release builds so that
        # we know in which place the binaries end up when using the Xcode generator
        CMAKE_ARGS  -Dgtest_force_shared_crt=ON -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${GOOGLETEST_LIB_DIR} -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${GOOGLETEST_LIB_DIR}
        # Disable install step
        INSTALL_COMMAND ""
        BINARY_DIR ${GOOGLETEST_LIB_DIR}
        INSTALL_DIR ${GOOGLETEST_LIB_DIR}
        # Wrap download, configure and build steps in a script to log output
        LOG_CONFIGURE ON
        LOG_BUILD ON
        BUILD_BYPRODUCTS ${GOOGLETEST_LIB_DIR}/libgtest${STATIC_EXT} ${GOOGLETEST_LIB_DIR}/libgtest_main${STATIC_EXT}
)

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/include)
# Specify MainTest's link libraries
ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARIES ${binary_dir}/libgtest${STATIC_EXT} ${binary_dir}/libgtest_main${STATIC_EXT})

add_dependencies(test-resources googletest)
list(APPEND STORM_TEST_LINK_LIBRARIES ${GTEST_LIBRARIES})

#############################################################
##
##	Intel Threading Building Blocks (optional)
##
#############################################################

set(STORM_HAVE_INTELTBB OFF)
if (STORM_USE_INTELTBB)
    find_package(TBB QUIET REQUIRED)

    if (TBB_FOUND)
        message(STATUS "Storm - Found Intel TBB with interface version ${TBB_INTERFACE_VERSION}.")
        message(STATUS "Storm - Linking with Intel TBB in ${TBB_LIBRARY_DIRS}.")
        set(STORM_HAVE_INTELTBB ON)
        link_directories(${TBB_LIBRARY_DIRS})
        include_directories(${TBB_INCLUDE_DIRS})
        list(APPEND STORM_LINK_LIBRARIES tbb tbbmalloc)
    else(TBB_FOUND)
        message(FATAL_ERROR "Storm - TBB was requested, but not found.")
    endif(TBB_FOUND)
endif(STORM_USE_INTELTBB)

#############################################################
##
##	Threads
##
#############################################################

find_package(Threads QUIET REQUIRED)
if (NOT Threads_FOUND)
    message(FATAL_ERROR "Storm - Threads was requested, but not found.")
endif()
include_directories(${THREADS_INCLUDE_DIRS})
list(APPEND STORM_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
if (STORM_USE_COTIRE)
    target_link_libraries(storm_unity ${CMAKE_THREAD_LIBS_INIT})
endif(STORM_USE_COTIRE)

#############################################################
##
##	CUDA Library generation
##
#############################################################


if ("${CUDA_ROOT}" STREQUAL "")
    set(ENABLE_CUDA OFF)
else()
    set(ENABLE_CUDA ON)
endif()

# CUDA Defines
if (ENABLE_CUDA)
    set(STORM_CPP_CUDA_DEF "define")
else()
    set(STORM_CPP_CUDA_DEF "undef")
endif()


# CUDA Defines
set(STORM_CPP_CUDAFORSTORM_DEF "undef")


if(ENABLE_CUDA)
    # Test for type alignment
    try_run(STORM_CUDA_RUN_RESULT_TYPEALIGNMENT STORM_CUDA_COMPILE_RESULT_TYPEALIGNMENT
            ${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/cuda/CMakeAlignmentCheck.cpp"
            COMPILE_OUTPUT_VARIABLE OUTPUT_TEST_VAR
            )
    if(NOT STORM_CUDA_COMPILE_RESULT_TYPEALIGNMENT)
        message(FATAL_ERROR "Storm (CudaPlugin) - Could not test type alignment, there was an Error while compiling the file ${PROJECT_SOURCE_DIR}/cuda/CMakeAlignmentCheck.cpp: ${OUTPUT_TEST_VAR}")
    elseif(STORM_CUDA_RUN_RESULT_TYPEALIGNMENT EQUAL 0)
        message(STATUS "Storm (CudaPlugin) - Result of Type Alignment Check: OK.")
    else()
        message(FATAL_ERROR "Storm (CudaPlugin) - Result of Type Alignment Check: FAILED (Code ${STORM_CUDA_RUN_RESULT_TYPEALIGNMENT})")
    endif()

    # Test for Float 64bit Alignment
    try_run(STORM_CUDA_RUN_RESULT_FLOATALIGNMENT STORM_CUDA_COMPILE_RESULT_FLOATALIGNMENT
            ${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/cuda/CMakeFloatAlignmentCheck.cpp"
            COMPILE_OUTPUT_VARIABLE OUTPUT_TEST_VAR
            )
    if(NOT STORM_CUDA_COMPILE_RESULT_FLOATALIGNMENT)
        message(FATAL_ERROR "Storm (CudaPlugin) - Could not test float type alignment, there was an Error while compiling the file ${PROJECT_SOURCE_DIR}/cuda/CMakeFloatAlignmentCheck.cpp: ${OUTPUT_TEST_VAR}")
    elseif(STORM_CUDA_RUN_RESULT_FLOATALIGNMENT EQUAL 2)
        message(STATUS "Storm (CudaPlugin) - Result of Float Type Alignment Check: 64bit alignment active.")
        set(STORM_CUDAPLUGIN_FLOAT_64BIT_ALIGN_DEF "define")
    elseif(STORM_CUDA_RUN_RESULT_FLOATALIGNMENT EQUAL 3)
        message(STATUS "Storm (CudaPlugin) - Result of Float Type Alignment Check: 64bit alignment disabled.")
        set(STORM_CUDAPLUGIN_FLOAT_64BIT_ALIGN_DEF "undef")
    else()
        message(FATAL_ERROR "Storm (CudaPlugin) - Result of Float Type Alignment Check: FAILED (Code ${STORM_CUDA_RUN_RESULT_FLOATALIGNMENT})")
    endif()
    #
    # Make a version file containing the current version from git.
    #
    include(GetGitRevisionDescription)
    git_describe_checkout(STORM_GIT_VERSION_STRING)
    # Parse the git Tag into variables
    string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" STORM_CUDAPLUGIN_VERSION_MAJOR "${STORM_GIT_VERSION_STRING}")
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" STORM_CUDAPLUGIN_VERSION_MINOR "${STORM_GIT_VERSION_STRING}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" STORM_CUDAPLUGIN_VERSION_PATCH "${STORM_GIT_VERSION_STRING}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+)\\-.*" "\\1" STORM_CUDAPLUGIN_VERSION_COMMITS_AHEAD "${STORM_GIT_VERSION_STRING}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-([a-z0-9]+).*" "\\1" STORM_CUDAPLUGIN_VERSION_HASH "${STORM_GIT_VERSION_STRING}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-[a-z0-9]+\\-(.*)" "\\1" STORM_CUDAPLUGIN_VERSION_APPENDIX "${STORM_GIT_VERSION_STRING}")
    if ("${STORM_CUDAPLUGIN_VERSION_APPENDIX}" MATCHES "^.*dirty.*$")
        set(STORM_CUDAPLUGIN_VERSION_DIRTY 1)
    else()
        set(STORM_CUDAPLUGIN_VERSION_DIRTY 0)
    endif()
    message(STATUS "Storm (CudaPlugin) - Version information: ${STORM_CUDAPLUGIN_VERSION_MAJOR}.${STORM_CUDAPLUGIN_VERSION_MINOR}.${STORM_CUDAPLUGIN_VERSION_PATCH} (${STORM_CUDAPLUGIN_VERSION_COMMITS_AHEAD} commits ahead of Tag) build from ${STORM_CUDAPLUGIN_VERSION_HASH} (Dirty: ${STORM_CUDAPLUGIN_VERSION_DIRTY})")


    # Configure a header file to pass some of the CMake settings to the source code
    configure_file (
            "${PROJECT_SOURCE_DIR}/cuda/storm-cudaplugin-config.h.in"
            "${PROJECT_BINARY_DIR}/include/storm-cudaplugin-config.h"
    )

    #create library
    find_package(CUDA REQUIRED)
    set(CUSP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/resources/3rdparty/cusplibrary")
    find_package(Cusp REQUIRED)
    find_package(Thrust REQUIRED)

    set(STORM_CUDA_LIB_NAME "storm-cuda")

    file(GLOB_RECURSE STORM_CUDA_KERNEL_FILES ${PROJECT_SOURCE_DIR}/cuda/kernels/*.cu)
    file(GLOB_RECURSE STORM_CUDA_HEADER_FILES ${PROJECT_SOURCE_DIR}/cuda/kernels/*.h)

    source_group(kernels FILES ${STORM_CUDA_KERNEL_FILES} ${STORM_CUDA_HEADER_FILES})
    include_directories(${PROJECT_SOURCE_DIR}/cuda/kernels/)

    #set(CUDA_PROPAGATE_HOST_FLAGS OFF)
    set(CUDA_NVCC_FLAGS "-arch=sm_30")

    #############################################################
    ##
    ##	CUSP
    ##
    #############################################################
    if(CUSP_FOUND)
        include_directories(${CUSP_INCLUDE_DIR})
        cuda_include_directories(${CUSP_INCLUDE_DIR})
        message(STATUS "Storm (CudaPlugin) - Found CUSP Version ${CUSP_VERSION} in location ${CUSP_INCLUDE_DIR}.")
    else()
        message(FATAL_ERROR "Storm (CudaPlugin) - Could not find CUSP.")
    endif()

    #############################################################
    ##
    ##	Thrust
    ##
    #############################################################
    if(THRUST_FOUND)
        include_directories(${THRUST_INCLUDE_DIR})
        cuda_include_directories(${THRUST_INCLUDE_DIR})
        message(STATUS "Storm (CudaPlugin) - Found Thrust Version ${THRUST_VERSION} in location ${THRUST_INCLUDE_DIR}.")
    else()
        message(FATAL_ERROR "Storm (CudaPlugin) - Could not find Thrust. Check your CUDA installation.")
    endif()

    include_directories(${CUDA_INCLUDE_DIRS})
    include_directories(${ADDITIONAL_INCLUDE_DIRS})

    cuda_add_library(${STORM_CUDA_LIB_NAME}
            ${STORM_CUDA_KERNEL_FILES} ${STORM_CUDA_HEADER_FILES}
            )

    message (STATUS "Storm - Linking with CUDA.")
    list(APPEND STORM_LINK_LIBRARIES ${STORM_CUDA_LIB_NAME})
    include_directories("${PROJECT_SOURCE_DIR}/cuda/kernels/")
endif()

add_custom_target(copy_resources_headers DEPENDS ${SPARSEPP_BINDIR_HEADERS})
