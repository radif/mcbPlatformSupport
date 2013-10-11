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
        cocos2d::CCGLProgram * _program=nullptr;//retained
        void initShader(const std::string & fragmentShaderPath, void* userData=nullptr){
            const GLchar *  fragmentSource =(GLchar *)cocos2d::CCString::createWithContentsOfFile(cocos2d::CCFileUtils::sharedFileUtils()-> fullPathForFilename(fragmentShaderPath.c_str()).c_str())->getCString();
            if (fragmentShaderPath.size() && strlen(fragmentSource)) {
                cocos2d::CCGLProgram * program(cocos2d::CCShaderCache::sharedShaderCache()->programForKey(fragmentShaderPath.c_str()));
                if (program)
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
                    
                    _program=program;
                    _program->retain();
                    
                    setupShaderProgram(program, userData);
                    
                    cocos2d::CCShaderCache::sharedShaderCache()->addProgram(program, fragmentShaderPath.c_str());
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
        cocos2d::CCGLProgram * shaderProgram(){return _program;}
    protected:
        
        //Override this in order to setup your shader behavior
        virtual void setupShaderProgram(cocos2d::CCGLProgram * program, void* userData=nullptr)=0;
        virtual void setupShaderProgramBindingsBeforeFirstUpdate(cocos2d::CCGLProgram * program, void* userData=nullptr){}
        virtual void shaderProgramInitFailed(const std::string & fragmentShaderPath, void* userData=nullptr)=0;
        
        //use visit() to set uniforms of the shader
        
        ShaderSprite()=default;
        virtual ~ShaderSprite(){CC_SAFE_RELEASE(_program);}
    };
}}

#endif /* defined(__mcb__utils__ShaderSprite__) */
