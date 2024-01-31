

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

#ifndef SAMPLE_LUBAN_POOL_H
#define SAMPLE_LUBAN_POOL_H

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include "feature.h"

namespace sample_luban {
 
std::vector<std::string> split(const std::string& str, char delimiter) ;
class Pool {
public:
  Pool() = default;
  Pool(std::string file_path);
  ~Pool() = default;
  Pool(const Pool&) = delete;
  Pool& operator=(const Pool&) = delete;
  int item_size() {return m_items.size();}
  luban::SharedFeaturesPtr operator[](const std::string &key);
  luban::SharedFeaturesPtr get(const std::string &key);
private:
  std::unordered_map<std::string, luban::SharedFeaturesPtr> m_items;
};

class PoolGetter {
public:
  PoolGetter(const std::vector<std::string>& files) ;
  PoolGetter(const std::vector<std::shared_ptr<Pool> >& pools) ;
  ~PoolGetter(); 
  bool update_pool(int index, std::string file);
  luban::SharedFeaturesPtr get(const std::string &key);
private:
 std::shared_ptr<Pool>* m_pools;
 int m_pools_size;
};

} // namespace sample_luban

#endif // SAMPLE_LUBAN_POOL_H
