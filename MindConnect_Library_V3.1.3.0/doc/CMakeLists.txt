# add a target to generate API documentation with Doxygen
FIND_PACKAGE(Doxygen)
MESSAGE(STATUS "Create and install the HTML based API documentation (requires Doxygen). Doxygen found = " ${DOXYGEN_FOUND})

SET(DOXYGEN_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/build/doc")
SET(DOXYGEN_DOCUMENTATION_ZIP_FILE "${DOXYGEN_WORKING_DIRECTORY}/MCL_Doxygen.zip")
SET(DOXYGEN_DOCUMENTATION_DIRECTORY "${CMAKE_BINARY_DIR}/build/doc/html")

IF(DOXYGEN_FOUND)
    MESSAGE(STATUS "Using Doxygen (${DOXYGEN_EXECUTABLE}) to build project documents.")
    SET(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/mcl.doxyfile)
    SET(DOXYFILE ${DOXYGEN_WORKING_DIRECTORY}/mcl.doxyfile)
    CONFIGURE_FILE(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

	SET(README_MD_IN ${CMAKE_SOURCE_DIR}/README.md)
    SET(README_MD ${DOXYGEN_WORKING_DIRECTORY}/README.md)
	CONFIGURE_FILE(${README_MD_IN} ${README_MD} @ONLY)

    ADD_CUSTOM_TARGET(mcl_doc
                      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build/doc
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)

    #Zip doxygen documents
    ADD_CUSTOM_TARGET(create_zip
                  COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${DOXYGEN_DOCUMENTATION_ZIP_FILE} --format=7zip "html"
                  WORKING_DIRECTORY ${DOXYGEN_WORKING_DIRECTORY})
                   
    ADD_DEPENDENCIES(create_zip mcl_doc)
    
    #Install zip of doxygen documents
    INSTALL(FILES ${DOXYGEN_DOCUMENTATION_ZIP_FILE} DESTINATION doc COMPONENT ${BINARY_COMPONENT_NAME})
ELSE()
    MESSAGE(FATAL_ERROR "Doxygen is required to build the documentation.")
ENDIF()
