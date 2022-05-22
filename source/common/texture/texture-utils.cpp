#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

our::Texture2D *our::texture_utils::empty(GLenum format, glm::ivec2 size)
{
    our::Texture2D *texture = new our::Texture2D();
    // TODO: (Req 10) Finish this function to create an empty texture with the given size and format
    // this function empty used with frame buffer

    /*target:Specifies the target to which the texture object is bound for glTexStorage2D. Must be one of GL_TEXTURE_2D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP, GL_PROXY_TEXTURE_2D, GL_PROXY_TEXTURE_1D_ARRAY, GL_PROXY_TEXTURE_RECTANGLE, or GL_PROXY_TEXTURE_CUBE_MAP.
    texture:Specifies the texture object name for glTextureStorage2D. The effective target of texture must be one of the valid non-proxy target values above.
    levels:Specify the number of texture levels.
    internalformat:Specifies the sized internal format to be used to store texture image data.  
    width:Specifies the width of the texture, in texels.
    height:Specifies the height of the texture, in texels.*/
    texture->bind();
    glTexStorage2D(GL_TEXTURE_2D, 1, format, size[0], size[1]); 
    return texture;
}

our::Texture2D *our::texture_utils::loadImage(const std::string &filename, bool generate_mipmap)
{
    glm::ivec2 size;
    int channels;
    // Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    // We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    // Load image data and retrieve width, height and number of channels in the image
    // The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    // Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char *pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if (pixels == nullptr)
    {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D *texture = new our::Texture2D();
    // Bind the texture such that we upload the image data to its storage
    texture->bind();
    // TODO: (Req 4) Finish this function to fill the texture with the data found in "pixels" and generate the mipmaps if requested

    // target, level, internal-format, width, height, border=0 const all the time , format, type, data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pixels);

    if (generate_mipmap) // if the mipmap is true then enable it 
        glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels); // Free image data after uploading to GPU
    return texture;
}