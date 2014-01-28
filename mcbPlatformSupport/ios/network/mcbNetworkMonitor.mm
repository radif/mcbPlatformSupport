//
//  mcbNetworkMonitor.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/27/14.
//
//

#include "mcbNetworkMonitor.h"
#import "Reachability.h"
#include "CCNotificationCenter.h"

@interface mcbReachabilityListener : NSObject{
    @public
    std::function<void(Reachability * r)> reachabilityChangedHandler;
}
@end
@implementation mcbReachabilityListener
-(instancetype)init{
    self=[super init];
    if (self) {
        reachabilityChangedHandler=nullptr;
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(reachabilityChanged:) name:kReachabilityChangedNotification object:nil];
    }
    return self;
}
- (void)reachabilityChanged:(NSNotification *)note{
	Reachability* curReach = [note object];
	NSParameterAssert([curReach isKindOfClass:[Reachability class]]);
	if (reachabilityChangedHandler)
        reachabilityChangedHandler(curReach);
}
- (void)dealloc{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:kReachabilityChangedNotification object:nil];
    [super dealloc];
}


@end

namespace mcb{namespace PlatformSupport{namespace network{
#define _nativeHandle (Reachability *)_nativeHandleHolder
#define _nativeListener (mcbReachabilityListener *)_nativeListenerHolder

    void NetworkMonitor::init(){
        _nativeHandleHolder=[[Reachability reachabilityForInternetConnection] retain];
        _nativeListenerHolder=[mcbReachabilityListener new];
        (_nativeListener)->reachabilityChangedHandler=[=](Reachability * r){
            cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(kConnectionTypeChangedNotificationName.c_str(), nullptr);
        };
        startMonitoring();
    }
    NetworkMonitor::ConnectionType NetworkMonitor::connectionType() const{
        NetworkStatus s([_nativeHandle currentReachabilityStatus]);
        ConnectionType connectionType(ConnectionTypeUnknown);
        switch (s) {
            case NotReachable:
                connectionType=ConnectionTypeNone;
                break;
            case ReachableViaWiFi:
                connectionType=ConnectionTypeWifi;
                break;
            case ReachableViaWWAN:
                connectionType=ConnectionTypeCellular;
                break;
                
            default:
                break;
        }
        
        return connectionType;
    }
    void NetworkMonitor::startMonitoring(){
        [_nativeHandle startNotifier];
        _isMonitoring=true;
    }
    void NetworkMonitor::stopMonitoring(){
        [_nativeHandle stopNotifier];
        _isMonitoring=false;
    }

    NetworkMonitor::~NetworkMonitor(){
        stopMonitoring();
        if (_nativeListener)
            [_nativeListener release];
        if (_nativeHandle)
            [_nativeHandle release];
    }
    
    
}}}
