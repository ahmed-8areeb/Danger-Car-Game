#pragma once

#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our
{
    // There are some options in the render pipeline that we cannot control via shaders
    // such as blending, depth testing and so on
    // Since each material could require different options (e.g. transparent materials usually use blending),
    // we will encapsulate all these options into a single structure that will also be responsible for configuring OpenGL's pipeline
    struct PipelineState
    {
        // This set of pipeline options specifies whether face culling will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum culledFace = GL_BACK;
            GLenum frontFace = GL_CCW;
        } faceCulling;

        // This set of pipeline options specifies whether depth testing will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum function = GL_LEQUAL;
        } depthTesting;

        // This set of pipeline options specifies whether blending will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum equation = GL_FUNC_ADD;
            GLenum sourceFactor = GL_SRC_ALPHA;
            GLenum destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
            glm::vec4 constantColor = {0, 0, 0, 0};
        } blending;

        // These options specify the color and depth mask which can be used to
        // prevent the rendering/clearing from modifying certain channels of certain targets in the framebuffer
        glm::bvec4 colorMask = {true, true, true, true}; // To know how to use it, check glColorMask
        bool depthMask = true;                           // To know how to use it, check glDepthMask

        // This function should set the OpenGL options to the values specified by this structure
        // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
        void setup() const
        {
            // Specifies wheteher the depth buffer is enabled for writing
            glDepthMask(depthMask);
            /**
             * @brief Construct a new gl Color Mask object
             * @param GLBollean red
             * @param GLBoolean green
             * @param GlBoolean blue
             * @param GlBoolean alpha
             */
            glColorMask(colorMask[0], colorMask[1], colorMask[2], colorMask[3]);

            // TODO: (Req 3) Write this function
            // faceCulling option
            if (faceCulling.enabled)
            {
                glEnable(GL_CULL_FACE);
                // culling the culled_face
                glCullFace(faceCulling.culledFace);
                // settnig the front face
                glFrontFace(faceCulling.frontFace);
            }
            else
                glDisable(GL_CULL_FACE); // This line disables face culling.

            // depthTesting option
            if (depthTesting.enabled)
            {
                glEnable(GL_DEPTH_TEST);

                // Specifies the depth comparison function. Symbolic constants GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS are accepted. The initial value is GL_LESS.
                glDepthFunc(depthTesting.function);
            }
            else
                glDisable(GL_DEPTH_TEST); // This line disables depth testing.
            
            // blending option
            if (blending.enabled)
            {
                glEnable(GL_BLEND);
                // specifies how source and destination colors are combined. It must be GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX.
                glBlendEquation(blending.equation);
                /*
                sfactor:Specifies how the red, green, blue, and alpha source blending factors are computed. The initial value is GL_ONE.
                dfactor:Specifies how the red, green, blue, and alpha destination blending factors are computed. The following symbolic constants are accepted:
                 GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA. GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA, and GL_ONE_MINUS_CONSTANT_ALPHA. The initial value is GL_ZERO.
                */
                glBlendFunc(blending.sourceFactor, blending.destinationFactor);
                glBlendColor(blending.constantColor.r, blending.constantColor.g, blending.constantColor.b, blending.constantColor.a);
            }
            else
                glDisable(GL_BLEND);

            // The blending formula has the following form: (source_factor * source) operation (destination_factor *
            // destination). The operation is specified by the blend equation function. The possible values are:
            // - GL_FUNC_ADD: the operation is "+".
            // - GL_FUNC_SUBTRACT: the operation is "-".
            // - GL_FUNC_REVERSE_SUBTRACT: the operation is "-" but the operands are reversed.
            // - GL_MIN: the operation picks the minimum value among the 2 operands.
            // - GL_MAX: the operation picks the maximum value among the 2 operands.

            // sfactor
            // - Specifies how the red, green, blue, and alpha source blending factors are computed.
            //   The initial value is GL_ONE.
            // dfactor
            // - Specifies how the red, green, blue, and alpha destination blending factors are computed.
            //   The initial value is GL_ZERO.

            // In case you're using any of the factors that use the constant color, you need to define it via the
            // glBlendColor function.
        }

        // Given a json object, this function deserializes a PipelineState structure
        void deserialize(const nlohmann::json &data);
    };

}