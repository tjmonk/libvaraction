cmake_minimum_required(VERSION 3.10)

project(varaction
	VERSION 0.1
    DESCRIPTION "Variable Action Script Suport Library"
)

include(GNUInstallDirs)

add_library( ${PROJECT_NAME} SHARED
	src/varaction.c
    src/varcompare.c
    src/varstrings.c
    src/varboolean.c
    src/varmath.c
    src/varbitwise.c
    src/vartypecast.c
    src/varassign.c
    src/vartimer.c
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION 1
)

set(VARACTION_HEADERS inc/varaction/varaction.h)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${VARACTION_HEADERS}")

target_include_directories( ${PROJECT_NAME} PRIVATE . )

target_include_directories( ${PROJECT_NAME} PUBLIC inc )

target_link_libraries( ${PROJECT_NAME}
    varserver
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/varaction)


