//
//  mcbPress.hpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/14/16.
//
//

#ifndef mcbPress_hpp
#define mcbPress_hpp

#include "cocos2d.h"

namespace mcb{namespace PlatformSupport{
    class Press;
    typedef std::shared_ptr<Press> p_Press;
    struct Press{
        typedef enum{
            TypeUpArrow,
            TypeDownArrow,
            TypeLeftArrow,
            TypeRightArrow,
            
            TypeSelect,
            TypeMenu,
            TypePlayPause,
        }Type;
        typedef enum{
            PhaseBegan,         // whenever a button press begins.
            PhaseChanged,       // whenever a button moves.
            PhaseStationary,    // whenever a buttons was pressed and is still being held down.
            PhaseEnded,         // whenever a button is releasd.
            PhaseCancelled,     // whenever a button press doesn't end but we need to stop tracking.
        }Phase;
        
        const Type type;
        const float force;
        const long long timestamp;
        const Phase phase;
        
        static p_Press create(const Type type, const long long timestamp, const Phase phase, const float force=1.0){return std::make_shared<Press>(type, timestamp, phase, force);}
        virtual ~Press()=default;
        
        Press(const Type m_type, const long long m_timestamp, const Phase m_phase, const float m_force=1.0): type(m_type), timestamp(m_timestamp), phase(m_phase), force(m_force){}
    };
}}
#endif /* mcbPress_hpp */
