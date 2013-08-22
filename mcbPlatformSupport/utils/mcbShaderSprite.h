//
//  ShaderSprite.h
//  mcb::utils
//
//  Created by Radif Sharafullin on 7/27/13.
//
//

#ifndef __mcb__utils__ShaderSprite__
#define __mcb__utils__ShaderSprite__

#include "cocos2d.h"
namespace mcb{ namespace utils{
    template <class Derived>
    class ShaderSprite : public cocos2d::CCSprite{
        typedef cocos2d::CCSprite super;
        void initShader(const std::string & fragmentShaderPath, void* userData=nullptr){
            const GLchar *  fragmentSource =(GLchar *)cocos2d::CCString::createWithContentsOfFile(cocos2d::CCFileUtils::sharedFileUtils()-> fullPathForFilename(fragmentShaderPath.c_str()).c_str())->getCString();
            if (fragmentShaderPath.size() && strlen(fragmentSource)) {
                cocos2d::CCGLProgram * program(cocos2d::CCShaderCache::sharedShaderCache()->programForKey(fragmentShaderPath.c_str()));
                if (/*program*/false)
                    setShaderProgram(program);
                else{
                    program=new cocos2d::CCGLProgram;
                    if (program){
                        program->initWithVertexShaderByteArray(cocos2d::ccPositionTextureA8Color_vert, fragmentSource);
                        setShaderProgram(program);
                        program->release();
                    }
                }
                
                if (program){
                    program->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
                    program->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);
                    program->addAttribute(kCCAttributeNameColor, cocos2d::kCCVertexAttrib_Color);
                    program->link();
                    program->updateUniforms();
                    
                    setupShaderProgram(program, userData);
                    
                    cocos2d::CCShaderCache::sharedShaderCache()->addProgram(program, fragmentShaderPath.c_str());
                    scheduleUpdate();
                    program->use();
                    setupShaderProgramBindingsBeforeFirstUpdate(program, userData);
                }else
                    shaderProgramInitFailed(fragmentShaderPath, userData);
                
            }
            
        }
    public:
        static Derived* create(const std::string & texturePath, const std::string & fragmentShaderPath, void* userData=nullptr){
            Derived * pobSprite = new Derived();
            if (pobSprite && pobSprite->initWithFile(texturePath.c_str())){
                pobSprite->initShader(fragmentShaderPath, userData);
                pobSprite->autorelease();
                return pobSprite;
            }
            CC_SAFE_DELETE(pobSprite);
            return nullptr;
        }
        

    protected:
        //Override this in order to setup your shader behavior
        virtual void setupShaderProgram(cocos2d::CCGLProgram * program, void* userData=nullptr)=0;
        virtual void setupShaderProgramBindingsBeforeFirstUpdate(cocos2d::CCGLProgram * program, void* userData=nullptr){}
        virtual void shaderProgramInitFailed(const std::string & fragmentShaderPath, void* userData=nullptr)=0;
        //updates are scheduled automatically if the shader program is instantiated
        virtual void update(float dt) override=0;
        
        ShaderSprite(){}
        virtual ~ShaderSprite(){}
    };
}}

#endif /* defined(__mcb__utils__ShaderSprite__) */
