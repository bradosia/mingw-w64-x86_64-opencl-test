set(EXENAME example00)

set(SOURCES
    main.cpp
)

set(HEADERS

)

include_directories(

)

link_directories(

)

add_executable(${EXENAME} ${SOURCES})

set(LIBRARIES

)

if(WIN32)
    set(LIBRARIES ${LIBRARIES}
        # Windows Libraries

    )
elseif(UNIX)
    set(LIBRARIES ${LIBRARIES}
        # Unix Libraries

    )
endif()

target_link_libraries(${EXENAME} ${LIBRARIES})
