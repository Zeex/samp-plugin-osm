#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <vector>
#include <subhook.h>
#include "sdk/amx/amx.h"
#include "sdk/plugincommon.h"

#if _MSC_VER < 1700
  #define vsnprintf vsprintf_s
#endif

typedef void (*logprintf_t)(const char *format, ...);

static const std::size_t LogprintfBufferSize = 1024;

extern void *pAMXFunctions;
static logprintf_t logprintf;

static SubHook logprintf_hook;
static std::vector<AMX*> scripts;

static void ProcessServerMessage(AMX *amx, const char *msg) {
  int index;
  if (amx_FindPublic(amx, "OnServerMessage", &index) == AMX_ERR_NONE) {
    cell address;
    amx_PushString(amx, &address, 0, msg, 0, 0);
    amx_Exec(amx, 0, index);
    amx_Release(amx, address);
  }
}

static void do_logprintf(const char *format, ...) {
  SubHook::ScopedRemove remove(&logprintf_hook);

  char buffer[LogprintfBufferSize];
  std::va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  for (std::vector<AMX*>::const_iterator it = scripts.begin();
       it != scripts.end(); it++) {
    ProcessServerMessage(*it, buffer);
  }

  logprintf("%s", buffer);
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
  return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
  pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
  logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
  logprintf_hook.Install(ppData[PLUGIN_DATA_LOGPRINTF], (void*)do_logprintf);
  return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
  logprintf_hook.Remove();
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
  scripts.push_back(amx);
  return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
  scripts.erase(std::remove(scripts.begin(), scripts.end(), amx));
  return AMX_ERR_NONE;
}
