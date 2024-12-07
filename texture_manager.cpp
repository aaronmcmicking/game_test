#pragma once

#include <raylib.h>
#include <algorithm>
#include <memory>

class TextureManager{
    public:
        TextureManager(){
        }

        // rule of 3: destructor, copy assignment, copy constructor
        ~TextureManager(){
            for(std::shared_ptr<Texture2D> text: textures){
                UnloadTexture(*text);
            }
        }

        void add(std::shared_ptr<Texture2D> texture){
            textures.push_back(texture);
        }

        void unload(){
            for(std::shared_ptr<Texture2D> text: textures){
                UnloadTexture(*text);
            }
        }

        std::vector<std::shared_ptr<Texture2D>> textures;
};

TextureManager texture_manager = TextureManager();
