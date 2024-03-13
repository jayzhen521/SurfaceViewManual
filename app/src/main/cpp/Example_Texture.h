/*
 * Example.cpp (Example_Texturing)
 *
 * Copyright (c) 2015 Lukas Hermanns. All rights reserved.
 * Licensed under the terms of the BSD 3-Clause license (see LICENSE.txt).
 */

#include <ExampleBase.h>
#include <FileUtils.h>
#include <LLGL/Utils/TypeNames.h>
#include <DDSImageReader.h>
#include <stb/stb_image.h>
#include <memory>
#include <EGL/egl.h>

class Example_Texturing : public ExampleBase
{

    ShaderPipeline              shaderPipeline;
    LLGL::PipelineLayout*       pipelineLayout      = nullptr;
    LLGL::PipelineState*        pipeline            = nullptr;
    LLGL::Buffer*               vertexBuffer        = nullptr;
    LLGL::Buffer*               indexBuffer         = nullptr;
    LLGL::Buffer*               sceneBuffer         = nullptr;
    LLGL::Texture*              colorMaps[2]        = {};
    LLGL::Sampler*              samplers[3]         = {};

    unsigned                    resourceIndex       = 0;

    const char*                 resourceLabels[4]   =
    {
        "compressed (BC1UNorm)",
        "uncompressed (RGBA8UNorm)",
        "uncompressed (RGBA8UNorm), lod bias",
        "uncompressed (RGBA8UNorm), lod bias, nearest filter",
    };

    struct Scene
    {
        Gs::Matrix4f wvpMatrix;
        Gs::Matrix4f wMatrix;
    }
    scene;

public:

    Example_Texturing() :
        ExampleBase { "LLGL Example: Texturing"}
    {
    }

private:

    void OnDrawFrame() override
    {
        commands->Begin();
        commands->Clear(LLGL::ClearFlags::ColorDepth, backgroundColor);
        commands->End();

        commandQueue->Submit(*commands);
    }

};

typedef std::shared_ptr<Example_Texturing> Sp_Example_Texturing;



