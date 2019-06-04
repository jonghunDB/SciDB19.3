//
// Created by snmyj on 4/23/17.
//

#ifndef SCIDB_TILEDBARRAYDESC_H
#define SCIDB_TILEDBARRAYDESC_H

#include <string>
#include <smgr/io/TileDBStorage.h>
#include <iostream>

namespace scidb
{
namespace hdf5gateway
{

class TileDBAttributeDesc {
public:
    const std::string& getName() const { return _name; }
    TileDBAttributeDesc(std::string name, std::string type)
        : _name(name), _type(type) {
    }
private:
    std::string _name;
    TileDBType _type;
};

class TileDBArrayDesc {
public:
    TileDBArrayDesc(std::string path) : _path(path) { }
    std::string const& getPath() const { return _path; }
    template<typename ...Args>
    int addAttribute(Args&& ...args) {
        int id = (int) _attributes.size();
        _attributes.emplace_back(std::forward<Args>(args)...);
        return id;
    }

    const TileDBAttributeDesc& getAttribute(int attrId) const {
        return _attributes[attrId];
    }

private:
    std::string _path;
    int _coordinate_type;
    std::vector<TileDBAttributeDesc> _attributes;
};

}
}





#endif //SCIDB_TILEDBARRAYDESC_H
