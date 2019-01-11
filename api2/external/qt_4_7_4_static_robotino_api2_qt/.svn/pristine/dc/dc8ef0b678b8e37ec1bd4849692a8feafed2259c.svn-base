IF( NOT QT4_STATIC_MOC_EXECUTABLE )
	SET( QT4_STATIC_MOC_EXECUTABLE "${QT_STATIC_MOC_EXECUTABLE}" )
ENDIF( NOT QT4_STATIC_MOC_EXECUTABLE )

# Modified copy from FindQt4.cmake since the QT4_WRAP_CPP macro does not
# generate unique moc names for files with identical names residing in
# different subdirectories
MACRO (MY_WRAP_CPP outfiles prefix)
	# get include dirs
	QT4_STATIC_GET_MOC_FLAGS(moc_includes)

	FOREACH (it ${ARGN})
		GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
		GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)

		SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/moc_${prefix}_${outfile}.cxx)
		ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
		COMMAND ${QT4_STATIC_MOC_EXECUTABLE}
		ARGS -nw ${moc_includes} -o ${outfile} ${it}
		DEPENDS ${it})
		SET(${outfiles} ${${outfiles}} ${outfile})
	ENDFOREACH(it)
ENDMACRO (MY_WRAP_CPP)
