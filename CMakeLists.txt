project(gateplugin)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} . )
LINK_LIBRARIES(${KDE4_LIBS})

set(kategateplugin_PART_SRCS gatekateplugin.cpp kateconsole.cpp )
kde4_add_plugin(kategateplugin ${kategateplugin_PART_SRCS})
target_link_libraries(kategateplugin  ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} kateinterfaces )

########### install files ###############
install(TARGETS kategateplugin  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES ui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kate/plugins/kategate )
install( FILES kategateplugin.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
