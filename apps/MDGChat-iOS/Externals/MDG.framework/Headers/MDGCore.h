//
//  MDGCore.h
//  MDG-iOS
//
//  Created by Bjarke Hesthaven Søndergaard on 17/08/15.
//  Copyright (c) 2015 Trifork A/S. All rights reserved.
//

@import Foundation;

@class MDGStatus;
@class MDGPairingState;
@class MDGPeerConnection;
@class MDGConfiguration;

typedef NS_ENUM(NSInteger, MDGRoutingStatus) {
    MDGRoutingStatusPeerNotAvailable        = -2,
    MDGRoutingStatusFailed                  = -1,
    MDGRoutingStatusDisconnected            = 0,
    MDGRoutingStatusConnected               = 1,
    MDGRoutingStatusUnknown                 = -9999
};

typedef NS_ENUM(NSInteger, MDGPairingStatus) {
    MDGPairingStatusStarting                = 0,
    MDGPairingStatusOneTimePasscodeReady    = 1,
    MDGPairingStatusCompleted               = 2,
    MDGPairingStatusFailedGeneric           = 3,
    MDGPairingStatusFailedOneTimePasscode   = 4,
    MDGPairingStatusUnknown                 = -9999
};

typedef NS_ENUM(NSInteger, MDGConnectionStatus) {
    MDGConnectionStatusNotDesired           = 0,
    MDGConnectionStatusConnecting           = 1,
    MDGConnectionStatusConnected            = 2,
    MDGConnectionStatusFailed               = 3,
    MDGConnectionStatusUnknown              = -9999
};

@protocol MDGCoreDelegate;

NS_ASSUME_NONNULL_BEGIN
@interface MDGCore : NSObject

@property (weak, nonatomic, nullable) id<MDGCoreDelegate> delegate;
@property (copy, nonatomic, readonly) NSArray<MDGPeerConnection *> *connections;
@property (copy, nonatomic, readonly) NSArray<NSString *> *pairings;
@property (nonatomic) BOOL persistence;
@property (nonatomic) BOOL debugging;
@property (nonatomic, readonly) BOOL started;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)sharedCore;

#pragma mark - General
- (void)startWithConfiguration:(nullable MDGConfiguration *)configuration; // Must be run before anything else
- (nullable MDGStatus *)statusWithError:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)aggressivePingForSeconds:(NSUInteger)durationInSeconds error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (nullable NSString *)whoAmIWithError:(NSError * __autoreleasing _Nullable * _Nullable)error;

#pragma mark - Connection to MDG
- (BOOL)connectWithProperties:(NSDictionary<NSString *, id> *)properties error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)disconnectWithError:(NSError * __autoreleasing _Nullable * _Nullable)error;

#pragma mark - Pairings
- (BOOL)enablePairingModeForSeconds:(NSUInteger)durationInSeconds error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (void)disablePairingMode;
- (BOOL)addPairingWithPeerId:(NSString *)peerId error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)revokePairingWithPeerId:(NSString *)peerId error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)revokeAllPairingsWithError:(NSError * __autoreleasing _Nullable * _Nullable)error;

- (BOOL)pairRemoteWithOneTimePasscode:(NSString *)oneTimePasscode error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)pairLocalWithOneTimePasscode:(NSString *)oneTimePasscode peerIp:(NSString *)peerIp port:(NSUInteger)port error:(NSError * __autoreleasing _Nullable * _Nullable)error;

#pragma mark - Connection to peer
- (nullable MDGPeerConnection *)placeCallRemoteWithPeerId:(NSString *)peerId protocolName:(NSString *)protocolName timeout:(NSUInteger)timeoutInSeconds error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (nullable MDGPeerConnection *)placeCallLocalWithPeerId:(nullable NSString *)peerId protocolName:(NSString *)protocolName peerIp:(NSString *)peerIp port:(NSUInteger)port timeout:(NSUInteger)timeoutInSeconds error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)connection:(MDGPeerConnection *)connection sendData:(NSData *)data error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)closeConnection:(MDGPeerConnection *)connection error:(NSError * __autoreleasing _Nullable * _Nullable)error;

#pragma mark - Debugging
- (BOOL)enableRemoteLoggingForSeconds:(NSUInteger)durationInSeconds error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)setDebugLogTarget:(NSInteger)target error:(NSError * __autoreleasing _Nullable * _Nullable)error;

@end

@protocol MDGCoreDelegate <NSObject>

@optional
- (void)connectionStatusChanged:(MDGConnectionStatus)status;
- (void)pairingStateChanged:(MDGPairingState *)state;
- (void)routingStatusChanged:(MDGPeerConnection *)connection toStatus:(MDGRoutingStatus)status;
- (BOOL)acceptIncomingCallFromPeerWithProtocolName:(NSString *)protocolName; // Default behavior is true
- (void)coreDidUpdatePairings:(NSArray<NSString *> *)pairings;
- (void)coreDidUpdateConnections:(NSArray<MDGPeerConnection *> *)connections;
- (void)connection:(MDGPeerConnection *)connection didReceiveData:(NSData *)data;

@end
NS_ASSUME_NONNULL_END
