//
// Created by Pavlo Yevsehnieiev
//

#include "postprocessing.h"

#include <iostream>
#include <ostream>

#include "postprocessing_shader.h"

namespace blipcade::renderer {

    Postprocessing::Postprocessing() {
        const std::string code = R"(
        void fragment() {
            COLOR = texture(TEXTURE, fragTexCoord);
        }
        )";

        changeShader(code);
    }

    Postprocessing::~Postprocessing() {
        UnloadShader(postprocessingShader);
    }

    void Postprocessing::changeShader(const std::string &code) {
        UnloadShader(postprocessingShader);

        std::string fragmentSource = postprocessing_shader_fragmentSource;

        fragmentSource.replace(fragmentSource.find("%FRAGMENT%"), 10, code);

        postprocessingShader = LoadShaderFromMemory(nullptr, fragmentSource.c_str());

        baseTextureLoc = GetShaderLocation(postprocessingShader, "TEXTURE");
        globalTimeLoc = GetShaderLocation(postprocessingShader, "TIME");
        screenWidthLoc = GetShaderLocation(postprocessingShader, "SCREEN_WIDTH");
        screenHeightLoc = GetShaderLocation(postprocessingShader, "SCREEN_HEIGHT");
    }

    void Postprocessing::postprocess(const RenderTexture2D &baseTexture, const RenderTexture2D &renderTexture, float globalTime, float screenWidth, float screenHeight) {
        BeginTextureMode(renderTexture);
        BeginShaderMode(postprocessingShader);
        SetShaderValueTexture(postprocessingShader, baseTextureLoc, baseTexture.texture);
        SetShaderValue(postprocessingShader, globalTimeLoc, &globalTime, SHADER_UNIFORM_FLOAT);
        SetShaderValue(postprocessingShader, screenWidthLoc, &screenWidth, SHADER_UNIFORM_FLOAT);
        SetShaderValue(postprocessingShader, screenHeightLoc, &screenHeight, SHADER_UNIFORM_FLOAT);
        DrawTextureRec(baseTexture.texture, {0, 0, static_cast<float>(baseTexture.texture.width), static_cast<float>(baseTexture.texture.height)}, {0, 0}, WHITE);
        EndShaderMode();
        EndTextureMode();
    }

} // renderer
// blipcade