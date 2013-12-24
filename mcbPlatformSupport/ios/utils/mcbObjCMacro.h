//
//  mcbObjCMacro.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 12/24/13.
//
//

#import <Foundation/Foundation.h>

#if __cplusplus
#error THIS CODE CAN ONLY BE USEFUL FOR obj-c, not obj-c++! USE auto in c++!
#endif

#define mcbMakeWeakSelf __weak typeof(self)weakSelf=self
#define mcbMakeWeak(INSTANCE) __weak typeof(INSTANCE)weak##INSTANCE = INSTANCE
#define mcbAssignWeak(existingInstance, newInstance) __weak typeof(existingInstance)newInstance = existingInstance

#define mcbAuto(name , expression) typeof(expression) name=expression

#define mcbMakeLambda(lamdaName , lamdaExpression) mcbAuto(lamdaName , lamdaExpression)