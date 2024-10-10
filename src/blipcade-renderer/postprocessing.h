//
// Created by Pavlo Yevsehnieiev
//

#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H
#include <raylib.h>
#include <vector>


namespace blipcade::renderer {

class Postprocessing {
public:
    Postprocessing();
    ~Postprocessing();

    void changeShader(const std::string &code);

    void postprocess(const RenderTexture2D &baseTexture, const RenderTexture2D &renderTexture, float globalTime, float screenWidth, float
                     screenHeight);

private:
    std::vector<std::string> m_shaders;
    Shader postprocessingShader;
    int baseTextureLoc;
    int globalTimeLoc;

    int screenWidthLoc = 0;
    int screenHeightLoc = 0;
};

} // renderer
// blipcade

#endif //POSTPROCESSING_H
