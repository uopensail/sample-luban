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
#include <tuple>



namespace sample_luban{
    
class SamplePreProcessor {
public:
    SamplePreProcessor(std::string process_plugin_file_path);
    ~SamplePreProcessor() = default;
    std::tuple<std::string, luban::SharedFeaturesPtr> process_sample(PoolGetter* pool, luban::SharedFeaturesPtr user_feature, std::string_view item_id, const std::string& label);
    std::tuple<std::string, luban::SharedFeaturesPtr> process_sample(PoolGetter* pool, luban::SharedFeaturesPtr user_feature, luban::SharedFeaturesPtr item_feature, const std::string& label);
    luban::SharedFeaturesPtr process_user_feature(PoolGetter* pool, luban::SharedFeaturesPtr user_feature);
    luban::SharedFeaturesPtr process_item_featrue(luban::SharedFeaturesPtr item_feature);
private:
    std::string m_plugin_name;
    std::shared_ptr<IFeaturePlugin> m_plugin;

};

class SampleLubanToolKit {
public:
  SampleLubanToolKit() = delete;
  explicit SampleLubanToolKit(const std::string& process_plugin_file_path,
    const std::string& luban_config_file);
  ~SampleLubanToolKit() {}
  std::shared_ptr<luban::Rows> process_user(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature);
  std::tuple<std::string, std::shared_ptr<luban::Rows>> process_sample(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature, const std::string& item_id, const std::string& label);
  std::tuple<std::string, std::shared_ptr<luban::Rows>> process_sample(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature, luban::SharedFeaturesPtr item_feature, const std::string& label);
  std::tuple<std::string,luban::SharedFeaturesPtr> sample_feature(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature, const std::string& item_id, const std::string& label);

private:
  
    std::shared_ptr<SamplePreProcessor> m_sample_tool_kit;
    std::shared_ptr<luban::Toolkit> m_luban_kit;
};

class SampleToolKit {
public:
  SampleToolKit() = delete;
  explicit SampleToolKit(const std::vector<std::string> & pool_files,
    const std::string& process_plugin_file_path,
    const std::string& luban_config_file);
  ~SampleToolKit() = default;
  void process_sample_files(const std::vector<std::string> & files, int thread_num,std::string output_dir);
private:
    std::string m_process_plugin_file_path;
    std::string m_luban_config_file;
    std::shared_ptr<PoolGetter> m_pool;
};

} // namespace sample_luban

#endif // SAMPLE_LUBAN_API_H