set( OpenInMpv_SRCS
	openinmpv.cpp
	icon.cpp
	)


set( OpenInMpv_RSCS
	openinmpv.qrc
)
qt5_add_resources(RSCS ${OpenInMpv_RSCS})

ecm_create_qm_loader( OpenInMpv_SRCS falkon_OpenInMpv_qt )

add_library(OpenInMpv MODULE ${OpenInMpv_SRCS} ${RSCS})
install(TARGETS OpenInMpv DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(OpenInMpv FalkonPrivate)
