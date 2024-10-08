//
// Created by Pavlo Yevsehnieiev
//

#include "project.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>


namespace blipcade::loader {
    Project::Project(std::string directory) {
        m_directory = std::filesystem::path(directory);

        loadProject();
    }

    void Project::loadProject() {
        // Find a project.blipcade file in the directory
        std::filesystem::path projectFile = m_directory / "project.blipcade";
        if (!std::filesystem::exists(projectFile)) {
            throw std::runtime_error("Project file not found");
        }

        nlohmann::json projectJson;

        // Load the project file
        std::ifstream projectStream(projectFile);
        projectStream >> projectJson;

        // Log the project file
        std::cout << projectJson.dump(4) << std::endl;
        entrypoint = m_directory / projectJson["entrypoint"];
        mainScene = m_directory / projectJson["mainScene"];

        // Log paths
        std::cout << "Entrypoint: " << entrypoint.lexically_normal() << std::endl;
        std::cout << "Main Scene: " << mainScene.lexically_normal() << std::endl;
    }


} // loader
// blipcade