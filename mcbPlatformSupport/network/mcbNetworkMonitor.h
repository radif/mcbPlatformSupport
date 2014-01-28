//
//  mcbNetworkMonitor.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/27/14.
//
//

#ifndef __SoundSurfer__mcbNetworkMonitor__
#define __SoundSurfer__mcbNetworkMonitor__

#include "mcbFactory.h"
#include "mcbNotificationHandler.h"

//Derive from NetworkNotifier or
//subscribe to kConnectionTypeChangedNotificationName to get notified.
//use mcbNotificationHandler to facilitate notification handling

namespace mcb{namespace PlatformSupport{namespace network{
    extern const std::string kConnectionTypeChangedNotificationName;
    
    class NetworkMonitor : public SingletonFactory<NetworkMonitor>{
        bool _isMonitoring=false;
        void * _nativeHandleHolder=nullptr;
        void * _nativeListenerHolder=nullptr;
    public:
        typedef enum{ConnectionTypeUnknown, ConnectionTypeNone, ConnectionTypeCellular, ConnectionTypeWifi} ConnectionType;
        void init() override;
        
        void startMonitoring();//this is called automatically by init, thus not necessary to call explicitely on initialization
        void stopMonitoring();//why wold anyone do that, but it is there. Call startMonitoring to restore listener

        bool isMonitoring() const{return _isMonitoring;}
        bool isConnected() const{return connectionType()==ConnectionTypeCellular || connectionType()==ConnectionTypeWifi;}
        //unreliable if the connection monitor is blocked
        ConnectionType connectionType() const;

        virtual ~NetworkMonitor();
    };
    
    class NetworkNotifier: virtual Notification{
    protected:
        NetworkMonitor::ConnectionType connectionType() const;
        bool isConnected() const;
        virtual void connectionStatusChanged(NetworkMonitor::ConnectionType status)=0;
    public:
        NetworkNotifier();
    };
    
}}}
#endif /* defined(__SoundSurfer__mcbNetworkMonitor__) */
