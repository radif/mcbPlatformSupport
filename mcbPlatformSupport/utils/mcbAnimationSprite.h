//
//  mcbAnimationSprite.h
//  PlatformSupport
//
//  Created by Radif Sharafullin on 7/24/13.
//
//

#ifndef __mcb__utils__AnimationSprite__
#define __mcb__utils__AnimationSprite__

#include "cocos2d.h"
namespace mcb{ namespace PlatformSupport{
    class AnimationSprite : public cocos2d::CCSprite{
        typedef cocos2d::CCSprite super;
        bool _isAnimating=false;
    public:
        AnimationSprite(){}
        virtual ~AnimationSprite(){}
        
        static AnimationSprite* create(const char *pszFileName);

        class AnimationData{
            typedef std::vector<cocos2d::CCTexture2D *> p_oTextures;
            float _frameDelay=1.f/24.f;
            int _currentFrameNumber=0;
            p_oTextures _texures;
            void reset(){_currentFrameNumber=0;}
            std::function<void()> _cycleCompletion;

            void bumpAnimationFrame(){
                _currentFrameNumber++;
                if(_currentFrameNumber>=_texures.size()){
                    _currentFrameNumber=0;
                    if (_cycleCompletion)
                        _cycleCompletion();
                }
            }
            
            cocos2d::CCTexture2D * textureForCurrentFrame(){return _texures.at(_currentFrameNumber);}
            
            float _dt=0.f;
            void addDeltaTime(const float & dt){
                if (_frameDelay>0.f) {
                    _dt+=dt;
                    while (_dt>_frameDelay) {
                        _dt-=_frameDelay;
                        bumpAnimationFrame();
                    }
                }
            }
            
            friend AnimationSprite;

        public:
            AnimationData(){};
            virtual ~AnimationData();

            void addTexture(cocos2d::CCTexture2D * tex);
            void setFrameDelay(float frameDelay){_frameDelay=frameDelay;}
            
            static std::shared_ptr<AnimationData> create(){return std::make_shared<AnimationData>();}
            
        };
        typedef std::shared_ptr<AnimationData> pAnimationData;
        
        void setAllAnimationFrameDelay(float frameDelay);
        void setAnimationDataForKey(pAnimationData animData, const std::string & key);
        void setAnimationFrameDelayForAnimationKey(float frameDelay, const std::string & key);
        void playAnimationForKey(const std::string & key, const std::function<void()> & cycleCompletion=nullptr, bool reset=true);
        void startCurrentAnimation(){_isAnimating=true;}
        void stopAnimation(){_isAnimating=false;}
        
    protected:
        virtual void onEnter() override;
        virtual void onExit() override;
        virtual void update(float dt) override;
    private:
        pAnimationData _currentAnimation=nullptr;
        std::map<std::string, pAnimationData> _animations;
        
        
    };
}}
#endif /* defined(__mcb__utils__AnimationSprite__) */
