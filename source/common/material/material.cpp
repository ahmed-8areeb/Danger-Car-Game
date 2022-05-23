#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our
{

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const
    {
        // TODO: (Req 6) Write this function
        //  call the pipline state setup to set face culling and depth testing and blending
        pipelineState.setup();
        // to program use
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        if (data.contains("pipelineState"))
        {
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint
    void TintedMaterial::setup() const
    {
        // TODO: (Req 6) Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex"
    void TexturedMaterial::setup() const
    {
        // TODO: (Req 6) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        texture->bind();
        sampler->bind(0);
        shader->set("tex", 0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data)
    {
        TintedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void LightedMaterial::setup() const
    {
        // TODO: (Req 6) Write this function
        Material::setup();

        glActiveTexture(GL_TEXTURE0);
        if (albedoTexture)
            albedoTexture->bind();
        albedoSampler->bind(0);
        shader->set("material.albedo", 0);

        glActiveTexture(GL_TEXTURE1);
        if (specularTexture)
            specularTexture->bind();
        specularSampler->bind(1);
        shader->set("material.specular", 1);

        glActiveTexture(GL_TEXTURE2);
        if (roughnessTexture)
            roughnessTexture->bind();
        roughnessSampler->bind(2);
        shader->set("material.roughness", 2);

        glActiveTexture(GL_TEXTURE3);
        if (ambientOcclusionTexture)
        {
            ambientOcclusionTexture->bind();
            shader->set("material.ambientOcclusionEnable", true);
        }else
            shader->set("material.ambientOcclusionEnable", false);
        ambientOcclusionSampler->bind(3);
        shader->set("material.ambient_occlusion", 3);

        glActiveTexture(GL_TEXTURE4);
        if (emissiveTexture)
            emissiveTexture->bind();
        emissiveSampler->bind(4);
        shader->set("material.emissive", 4);

        glActiveTexture(GL_TEXTURE5);
        if (alphaTexture)
        {
            alphaTexture->bind();
            shader->set("material.alphaTextureEnable", true);
        }
        else
            shader->set("material.alphaTextureEnable", false);
        alphaSampler->bind(5);
        shader->set("material.alpha", 5);

        glActiveTexture(GL_TEXTURE0);
    }

    // This function read the material data from a json object
    void LightedMaterial::deserialize(const nlohmann::json &data)
    {
        LightedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        if(data.contains("albedo_texture"))
            albedoTexture = AssetLoader<Texture2D>::get(data.value("albedo_texture", ""));
        albedoSampler = AssetLoader<Sampler>::get(data.value("albedo_sampler", "default"));

        if (data.contains("specular_texture"))
            specularTexture = AssetLoader<Texture2D>::get(data.value("specular_texture", ""));
        specularSampler = AssetLoader<Sampler>::get(data.value("specular_sampler", "default"));

        if (data.contains("roughness_texture"))
            roughnessTexture = AssetLoader<Texture2D>::get(data.value("roughness_texture", ""));
        roughnessSampler = AssetLoader<Sampler>::get(data.value("roughness_sampler", "default"));

        if (data.contains("ambientOcclusion_texture"))
            ambientOcclusionTexture = AssetLoader<Texture2D>::get(data.value("ambientOcclusion_texture", ""));
        ambientOcclusionSampler = AssetLoader<Sampler>::get(data.value("ambientOcclusion_sampler", "default"));

        if (data.contains("emissive_texture"))
            emissiveTexture = AssetLoader<Texture2D>::get(data.value("emissive_texture", ""));
        emissiveSampler = AssetLoader<Sampler>::get(data.value("emissive_sampler", "default"));

        if (data.contains("alpha_texture"))
            alphaTexture = AssetLoader<Texture2D>::get(data.value("alpha_texture", ""));
        alphaSampler = AssetLoader<Sampler>::get(data.value("alpha_sampler", "default"));
        
    }
}