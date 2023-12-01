#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

our::Texture2D* our::texture_utils::empty(GLenum format, glm::ivec2 size){
    our::Texture2D* texture = new our::Texture2D();
    //DONE: (Req 11) Finish this function to create an empty texture with the given size and format
    texture->bind();
    glTexStorage2D(GL_TEXTURE_2D, 1, format, size.x, size.y); //glTexStorage2D(GL_TEXTURE_2D, #mipmapLevels, #format, width, height)
    texture->unbind();
    return texture;
}

our::Texture2D* our::texture_utils::loadImage(const std::string& filename, bool generate_mipmap) {
    glm::ivec2 size;
    int channels;
    //Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    //We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    //Load image data and retrieve width, height and number of channels in the image
    //The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    //Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char* pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if(pixels == nullptr){
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D* texture = new our::Texture2D();
    //Bind the texture such that we upload the image data to its storage
    GLenum format;
    //TODO: (Req 5) Finish this function to fill the texture with the data found in "pixels"
    switch(channels){
        case 1: 
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
    }
    texture->bind();
    //glTexImage2D(GL_TEXTURE_2D, #mipmapLevel, #internalFormat, width, height, 0, #format, #type, data)
    // takes the internal format and output it into RGBA type 
    glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    //Generate mipmaps if requested
    if (generate_mipmap) glGenerateMipmap(GL_TEXTURE_2D);
    //Unbind the texture
    texture->unbind();
    stbi_image_free(pixels); //Free image data after uploading to GPU
    return texture;
}