//
// Created by Pavlo Yevsehnieiev
//

#ifndef PROJECT_H
#define PROJECT_H
#include <string>
#include <filesystem>

namespace blipcade::loader {

class Project {
public:
    explicit Project(std::string directory);
    ~Project() = default;

    std::string getEntryPoint() const { return entrypoint.lexically_normal(); }
private:
    std::filesystem::path m_directory;
    void loadProject();
    std::filesystem::path entrypoint;
    std::filesystem::path mainScene;
};

} // loader
// blipcade

#endif //PROJECT_H
