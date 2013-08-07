//
//  mcbDrawTexturePoly.cpp
//  mcb
//
//  Created by Radif Sharafullin on 7/30/13.
//
//

#include "mcbDrawTexturePoly.h"
using namespace cocos2d;
namespace mcb{namespace PlatformSupport{namespace DrawTexturePoly{
    void drawTexturePoly(cocos2d::CCTexture2D * tex, const cocos2d::CCPoint * poli, unsigned int numberOfPoints, bool preferHorizontal){
    
        const float m_uPixelsWide(tex->getPixelsWide());
        const float m_uPixelsHigh(tex->getPixelsHigh());
        const float m_fMaxT(1.f);
        const float m_fMaxS(1.f);
        
        GLfloat    coordinates[] = {
            0.0f,    m_fMaxT,
            m_fMaxS,m_fMaxT,
            0.0f,    0.0f,
            m_fMaxS,0.0f };
        
        CCPoint halfOffset{-m_uPixelsWide*.5f, -m_uPixelsHigh*.5f};
        
        GLuint m_uName(tex->getName());
        
        CCGLProgram *m_pShaderProgram(tex->getShaderProgram());
        ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords );
        m_pShaderProgram->use();
        m_pShaderProgram->setUniformsForBuiltins();
        ccGLBindTexture2D( m_uName );
                
        for (int pointI(1); pointI<numberOfPoints; ++pointI) {
            CCPoint currentPoint(ccpAdd(poli[pointI], halfOffset));
            CCPoint prevPoint(ccpAdd(poli[pointI-1], halfOffset));

            //CCPoint prevPoint(poli[pointI-1]);
            
            const float dist(preferHorizontal? fabsf(currentPoint.x-prevPoint.x) : fabsf(currentPoint.y-prevPoint.y));
            const float step(preferHorizontal? m_uPixelsWide : m_uPixelsHigh);
            
            for (int i(0); i<dist; i+=step) {
                float progress(float(i)/dist);
                CCPoint point{
                    prevPoint.x+(currentPoint.x-prevPoint.x)*progress
                    ,
                    prevPoint.y+(currentPoint.y-prevPoint.y)*progress
                };
                
                
                GLfloat    width = (GLfloat)m_uPixelsWide * m_fMaxS,
                height = (GLfloat)m_uPixelsHigh * m_fMaxT;
                
                GLfloat        vertices[] = {
                    point.x,            point.y,
                    width + point.x,    point.y,
                    point.x,            height  + point.y,
                    width + point.x,    height  + point.y };
                
                
                glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
                glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, coordinates);
                
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            }
            
            
            
        }
    
    }
}}}
