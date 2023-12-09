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

#ifndef SAMPLE_LUBAN_API_H
#define SAMPLE_LUBAN_API_H
#include "iplugin.h"
#include "pool.h"
#include "toolkit.h"


namespace sample_luban{
    
class SampleToolkit {
public:
    SampleToolkit(std::string process_plugin_file_path);
    ~SampleToolkit() = default;
    luban::SharedFeaturesPtr process_sample(PoolGetter* pool, luban::SharedFeaturesPtr user_feature, std::string_view item_id);
    luban::SharedFeaturesPtr process_user_feature(PoolGetter* pool, luban::SharedFeaturesPtr user_feature);
    luban::SharedFeaturesPtr process_item_featrue(luban::SharedFeaturesPtr item_feature);
private:
    std::string m_plugin_name;
    std::shared_ptr<IFeaturePlugin> m_plugin;

};

class SampleLubanToolKit {
public:
  SampleLubanToolKit() = delete;
  explicit SampleLubanToolKit(
    const std::string& pool_file,
    const std::string &luban_config_file, 
    const std::string& process_plugin_file_path);
  ~SampleLubanToolKit() {}
  std::shared_ptr<luban::Rows> process_sample(const std::string& user_feature_json, const std::string& item_id);
private:
    std::shared_ptr<PoolGetter> m_pool_getter;
    std::shared_ptr<SampleToolkit> m_sample_tool_kit;
    std::shared_ptr<luban::Toolkit> m_luban_kit;
};

} // namespace sample_luban

#endif // SAMPLE_LUBAN_API_H