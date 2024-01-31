

#include "feature.h"
#include "pool.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <atomic>



namespace sample_luban {
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
std::unordered_map<std::string, luban::SharedFeaturesPtr> load(std::string file_path) {
  std::unordered_map<std::string, luban::SharedFeaturesPtr> items;
  std::ifstream reader(file_path, std::ios::in);
  if (!reader) {
    std::cerr << "read pool data file: " << file_path << " error" << std::endl;
    exit(-1);
  }
  std::string line;
 
  while (std::getline(reader, line)) {
    auto ss = split(line,'\t');
    if (ss.size() != 2) {
      continue;
    }
    auto item_id = ss[0];
    luban::SharedFeaturesPtr features = std::make_shared<luban::Features>(ss[1]);
    items[item_id] = features;
    
  }
  reader.close();
  return items;
}
Pool::Pool(std::string file_path) {
 m_items = load(file_path);
}

luban::SharedFeaturesPtr Pool::operator[](const std::string &key) {
  auto it = m_items.find(key);
  if (it != m_items.end()) {
    return it->second;
  }
  return nullptr;
}

luban::SharedFeaturesPtr Pool::get(const std::string &key) {
  auto it = m_items.find(key);
  if (it != m_items.end()) {
    return it->second;
  }
  return nullptr;
}

PoolGetter::PoolGetter(const std::vector<std::string>& files) {
  m_pools_size = files.size();
  m_pools = new std::shared_ptr<Pool>[m_pools_size];
  for (int i=0;i<files.size();i++) {
      m_pools[i]= std::make_shared<Pool>(files[i]);
  }
}
PoolGetter::PoolGetter(const std::vector<std::shared_ptr<Pool> >& pools)  {
  m_pools_size = pools.size();
  m_pools = new std::shared_ptr<Pool>[m_pools_size];
  for (int i=0;i<pools.size();i++) {
      m_pools[i]= pools[i];
  }
}
PoolGetter::~PoolGetter() {
  delete []m_pools;
}

bool PoolGetter::update_pool(int index, std::string file) {
   if (index >=0 && index < m_pools_size) {
      auto pool =  std::make_shared<Pool>(file);
      if (pool != nullptr) {
          std::atomic_store(&m_pools[index], pool);

          return true;
      }
   }
   return false;
}

luban::SharedFeaturesPtr PoolGetter::get(const std::string &key){
   for (int i=0;i<m_pools_size;i++) {
    std::shared_ptr<Pool> pool = std::atomic_load(&m_pools[i]);
    auto ret = pool->get(key);
    if (ret != nullptr) {
        return ret;
     }
   }
   return nullptr;
}

} // namespace sample_luban
