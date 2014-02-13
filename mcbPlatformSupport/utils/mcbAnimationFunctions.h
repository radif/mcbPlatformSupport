//
//  mcbAnimationFunctions.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/12/13.
//
//

#ifndef __SoundSurfer__mcbAnimationFunctions__
#define __SoundSurfer__mcbAnimationFunctions__

#include "cocos2d.h"
namespace mcb{ namespace PlatformSupport{
    class mcbEaseBackInOut : public cocos2d::CCEaseBackInOut{
        const float _overshot;
    public:
        virtual void update(float time);
        mcbEaseBackInOut(float overshot);
        virtual ~mcbEaseBackInOut();
    public:
        /** creates the action */
        static mcbEaseBackInOut* create(CCActionInterval* pAction, float overshot=1.f);
    };
    
    
    class Bezier;
    typedef std::shared_ptr<Bezier> pBezier;
    struct BezierConfig {
        //! Bezier control point 1
        cocos2d::CCPoint controlPt1;
        //! Bezier control point 2
        cocos2d::CCPoint controlPt2;
        //! end position of the bezier
        cocos2d::CCPoint endPos;
    };
    
    typedef std::vector<BezierConfig> bezier_configs_t;
    class Bezier{
        cocos2d::CCPoint _startPosition;
        bezier_configs_t _configs;
    public:
        static pBezier create();
        static pBezier create(const cocos2d::CCPoint & startPos, const bezier_configs_t & configs);
        void setStartPosition(const cocos2d::CCPoint & startPos){_startPosition=startPos;}
        bezier_configs_t & configs(){return _configs;}
        void clearConfigs(){_configs.clear();}
        void setConfigs(const bezier_configs_t & configs);
        void addConfig(const BezierConfig & config);
        void addConfig(const cocos2d::CCPoint & pt1, const cocos2d::CCPoint & pt2, const cocos2d::CCPoint & pt3);
        
        cocos2d::CCPoint pointAt(float progress);
    };
    
    
}}

#endif /* defined(__SoundSurfer__mcbAnimationFunctions__) */
