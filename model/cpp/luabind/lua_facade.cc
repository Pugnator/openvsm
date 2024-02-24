#include "lua_facade.hpp"

namespace LuaScripting
{
  LuaTableFacade::LuaTableFacade(const std::string &module)
  {
    //LOG_DEBUG("Loading module {}\n", module);    
    L_ = LuaContextManager::getInstance().getLuaState();
    if(!lua_getglobal(L_, "require"))
    {
      throw LuaException("Error requiring module: require not found");
    }
    lua_pushstring(L_, module.c_str());
    if (lua_pcall(L_, 1, 1, 0) != 0)
    {
      std::string error = lua_tostring(L_, -1);
      lua_pop(L_, 1);
      throw LuaException("Error requiring module: " + error);
    }
    //LOG_DEBUG("Module {} required successfully\n", module);
    tableRef_ = luaL_ref(L_, LUA_REGISTRYINDEX);
    if (!checkLuaTable(L_, tableRef_))
    {
      //LOG_DEBUG("Module {} is not a table\n", module);
      throw LuaException("Error loading module: not a table");
    }
    //LOG_DEBUG("Module {} loaded successfully\n", module);
  }

  LuaTableFacade::~LuaTableFacade()
  {
    luaL_unref(L_, LUA_REGISTRYINDEX, tableRef_);
  }

  void LuaTableFacade::newSelfInstance(int self)
  {    
    if (!checkLuaTable(L_, self))
    {
      std::string err = "Failed to create new instance: not a table.";
      //LOG_DEBUG("[LUA]: {}\n", err);
      throw LuaException(err);
    }
    luaL_unref(L_, LUA_REGISTRYINDEX, tableRef_);
    tableRef_ = self;
  }

  bool LuaTableFacade::checkLuaTable(lua_State *L, int tableRef)
  {
    lua_rawgeti(L, LUA_REGISTRYINDEX, tableRef);
    bool isTable = lua_istable(L, -1);
    lua_pop(L, 1); // Remove the table (or non-table) from the stack
    return isTable;
  }
}