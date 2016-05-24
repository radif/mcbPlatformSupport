//
//  mcbCallLambda.h
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//
#ifndef __mcb__mcbCallLambda__
#define __mcb__mcbCallLambda__

#include "cocos2d.h"

namespace mcb{namespace PlatformSupport{
    class CallLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void()> _lambda=nullptr;
        virtual void update(float time) {execute();}
    public:
        static CallLambda * create(const std::function<void()> & lambda, int tag=0);
        static cocos2d::CCSequence * createWithDelay(const float delay, const std::function<void()> & lambda, int tag=0);
        virtual bool initWithLambda(const std::function<void()> & lambda);
        virtual void execute();
    protected:
        CallLambda() = default;
        virtual ~CallLambda(){}
    };
    
    template <typename T>
    class ScheduleLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void(const float deltaTime, T *userData , bool & stop)> _lambda=nullptr;
        virtual void update(float deltaTime) {execute(deltaTime);}
        bool _isDone=false;
        virtual bool isDone(){return _isDone;}
        virtual void step(float dt){update(dt);}
        T _userData;
    public:
        static ScheduleLambda * create(T && userData, std::function<void(const float deltaTime, T *userData , bool & stop)> &&lambda){
            ScheduleLambda *retVal = new ScheduleLambda();
            if (retVal){
                retVal->_userData=std::forward<T>(userData);
                retVal->_lambda=std::forward<decltype(lambda)>(lambda);
                retVal->autorelease();
                return retVal;
            }
            CC_SAFE_DELETE(retVal);
            return nullptr;
        }
        
        virtual void execute(float deltaTime){
            if (_lambda)
                _lambda(deltaTime, &_userData,_isDone);
        }
    protected:
        ScheduleLambda() = default;
        virtual ~ScheduleLambda()=default;
    };
    template <typename T>
    ScheduleLambda<T> * create_scheduleLambda(T userData, std::function<void(float deltaTime, decltype(userData) *userData , bool & stop)> && lambda){
        return ScheduleLambda<T>::create(std::forward<T>(userData), std::forward<decltype(lambda)>(lambda));
    }
    
    
    class ScheduleUpdateLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void(const float deltaTime, bool & stop)> _lambda=nullptr;
        virtual void update(float deltaTime) {execute(deltaTime);}
        bool _isDone=false;
        virtual bool isDone(){return _isDone;}
        virtual void step(float dt){update(dt);}
    public:
        static ScheduleUpdateLambda * create(std::function<void(const float deltaTime, bool & stop)> && lambda, int tag=0);
        virtual void execute(float deltaTime);
    protected:
        ScheduleUpdateLambda()=default;
        virtual ~ScheduleUpdateLambda()=default;
    };
    
    class ScheduleTimerLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void(const float deltaTime, const float progress, bool & stop)> _lambda=nullptr;
        std::function<void()> _finally=nullptr;
        virtual void update(float deltaTime) {execute(deltaTime);}
        bool _isDone=false;
        const float _duration;
        float _actionTime=0.f;
        virtual bool isDone(){return _isDone;}
        virtual void step(float dt){update(dt);}
    public:
        static ScheduleTimerLambda * create(const float duration, std::function<void(const float deltaTime, const float progress, bool & stop)> && lambda, std::function<void()> && finally=nullptr, int tag=0);
        virtual void execute(float deltaTime);
    protected:
        ScheduleTimerLambda(const float & duration): _duration(duration){}
        virtual ~ScheduleTimerLambda()=default;
    };
    
    class TruTimerLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void(const float deltaTime, const float progress, bool & stop)> _lambda=nullptr;
        std::function<void()> _finally=nullptr;
        virtual void update(float deltaTime) {execute(deltaTime);}
        bool _isDone=false;
        const float _duration;
        long long _startTime=0;
        virtual bool isDone(){return _isDone;}
        virtual void step(float dt){update(dt);}
    public:
        static TruTimerLambda * create(const float duration, std::function<void(const float deltaTime, const float progress, bool & stop)> && lambda, std::function<void()> && finally=nullptr, int tag=0);
        virtual void execute(float deltaTime);
    protected:
        TruTimerLambda(const float & duration): _duration(duration){}
        virtual ~TruTimerLambda()=default;
    };
    
    namespace ProgressCurveFunctions{
        static float inline linearToEaseOut(const float progress){return 1.f-cosf(progress*M_PI_2);}
        static float inline linearToEaseIn(const float progress){return sinf(progress*M_PI_2);}
        static float inline linearToEaseInOut(const float progress){return linearToEaseOut(linearToEaseIn(progress));}
        float linearToBackInOut(float progress, float multiplier=1.f, float rate=1.5f);
        float linearToBackIn(float progress, float rate=1.5f);
        float linearToBackOut(float progress, float rate=1.5f);
        
        class Bezier;
        typedef std::shared_ptr<Bezier> pBezier;
        
        class Bezier{
            cocos2d::CCPoint _startPosition;
        public:
            struct Config {
                //! Bezier control point 1
                cocos2d::CCPoint controlPt1;
                //! Bezier control point 2
                cocos2d::CCPoint controlPt2;
                //! end position of the bezier
                cocos2d::CCPoint endPos;
            };
            typedef std::vector<Bezier::Config> bezier_configs_t;
        private:
            bezier_configs_t _configs;
        public:
            static pBezier create();
            static pBezier create(const cocos2d::CCPoint & startPos, const bezier_configs_t & configs);
            void setStartPosition(const cocos2d::CCPoint & startPos){_startPosition=startPos;}
            bezier_configs_t & configs(){return _configs;}
            void clearConfigs(){_configs.clear();}
            void setConfigs(const bezier_configs_t & configs);
            void addConfig(const Bezier::Config & config);
            void addConfig(const cocos2d::CCPoint & pt1, const cocos2d::CCPoint & pt2, const cocos2d::CCPoint & pt3);
            
            cocos2d::CCPoint pointAt(float progress);
        };

        
    }
}}


#endif /* defined(__mcb__mcbCallLambda__) */
