//
// `FeaturePlugin` - 'c++ tool for transformating and hashing features'
// Copyright (C) 2019 - present timepi <timepi123@gmail.com>
// FeaturePlugin is provided under: GNU Affero General Public License (AGPL3.0)
// https://www.gnu.org/licenses/agpl-3.0.html unless stated otherwise.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.

#ifndef SAMPLE_LUBAN_LUA_PLUGIN_H
#define SAMPLE_LUBAN_LUA_PLUGIN_H
#include "iplugin.h"
#include <sol/sol.hpp>

namespace sample_luban{
    
class LuaPluginBridge: public IFeaturePlugin{
public:
    LuaPluginBridge(std::string name, std::string file_path);
    ~LuaPluginBridge();
    virtual std::shared_ptr<luban::Features> process_sample(std::shared_ptr<luban::Features> user_feature, PoolGetter* pool) override;
    virtual std::shared_ptr<luban::Features> process_user(std::shared_ptr<luban::Features>  user_feature, 
        PoolGetter* pool) override;
    virtual std::shared_ptr<luban::Features> process_item(std::shared_ptr<luban::Features>  item_feature) override;
private:
    void load(std::string name,std::string file_path);
    void try_load();
    sol::state m_lua;
    std::string m_name;
    std::string m_file_path;
};

} // namespace sample_luban

#endif // SAMPLE_LUBAN_LUA_PLUGIN_H