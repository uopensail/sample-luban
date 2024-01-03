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

#ifndef SAMPLE_LUBAN_PLUGIN_H
#define SAMPLE_LUBAN_PLUGIN_H

#include <string>
#include <unordered_map>
#include <shared_mutex>
#include "pool.h"

namespace sample_luban{
class IFeaturePlugin {
public:
    virtual std::shared_ptr<luban::Features> process_sample(std::shared_ptr<luban::Features>  user_feature, 
        PoolGetter* pool) =0;
    virtual std::shared_ptr<luban::Features> process_user(std::shared_ptr<luban::Features>  user_feature, 
        PoolGetter* pool) =0;
    virtual std::shared_ptr<luban::Features> process_item(std::shared_ptr<luban::Features>  item_feature) =0;
    virtual std::string process_label(std::shared_ptr<luban::Features>  sample_features, const std::string& label) =0;

};

typedef  void* (*FeatureProcessFunc)(void *user_feature_ptr, void *pool);

} // namespace sample_luban

#endif // SAMPLE_LUBAN_PLUGIN_H
