set(SRC_LIST 
	main.c 
	output.c 
	usage.c 
	handEDID.c 
	handDisplayAll.c
	handDisplayProductID.c 
	handDisplayNormal.c
	bless_private.h
	blessedid.h
	enums.h
	modes.h
	protos.h
	structs.h
	)
set(CONFIGURATION_BUILD_DIR "build")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(readedid ${SRC_LIST})