/**
 * plugin.cpp — Qt plugin entry point for LezRegistryModule.
 *
 * This file is intentionally minimal. The Q_PLUGIN_METADATA macro in
 * lez_registry_module.h already embeds the metadata.json and declares
 * the qt_plugin_instance() / qt_plugin_query_metadata_v2() symbols that
 * the Qt plugin system requires.
 *
 * Including the header here (via the MOC-generated moc_lez_registry_module.cpp)
 * ensures the plugin object factory is compiled into the shared library.
 */

#include "lez_registry_module.h"
