FILE(GLOB SRCS *.cpp)
FILE(GLOB HDRS *.h)

MY_WRAP_CPP(
	MOC_SRCS
	"my"
	${HDRS}
)

ADD_EXECUTABLE(
	Test
	WIN32
	${HDRS}
	${SRCS}
	${MOC_SRCS}
)

TARGET_LINK_LIBRARIES(
	Test
	rec_robotino_api2
	rec_robotino_rpc
	rec_rpc
	rec_cv_lt
	rec_robotino3_fleetcom
	Qt5::Core
	Qt5::Network
	Qt5::Xml
)