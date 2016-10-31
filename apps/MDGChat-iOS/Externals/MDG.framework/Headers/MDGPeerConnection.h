//
//  MDGPeerConnection.h
//  MDG-iOS
//
//  Created by Bjarke Hesthaven Søndergaard on 27/08/15.
//  Copyright (c) 2015 Trifork A/S. All rights reserved.
//

@import Foundation;

@protocol MDGPeerConnectionDelegate;

NS_ASSUME_NONNULL_BEGIN
@interface MDGPeerConnection : NSObject

@property (weak, nonatomic) id<MDGPeerConnectionDelegate> delegate;
@property (nonatomic, readonly) NSUInteger connectionId;
@property (nonatomic, readonly, nullable) NSString *peerId;
@property (nonatomic, readonly, nullable) NSString *protocolName;
@property (nonatomic, readonly, nullable) NSString *issuedById;
@property (nonatomic, readonly) BOOL connected;
@property (nonatomic, readonly) BOOL connecting;

- (instancetype)init NS_UNAVAILABLE;

#pragma mark - Send/Receive
- (BOOL)sendData:(NSData *)data error:(NSError * __autoreleasing _Nullable * _Nullable)error;
- (BOOL)closeConnectionWithError:(NSError * __autoreleasing _Nullable * _Nullable)error;

@end

@protocol MDGPeerConnectionDelegate <NSObject>

@optional
- (void)connection:(MDGPeerConnection *)connection receivedData:(NSData *)data;
- (void)connection:(MDGPeerConnection *)connection changedConnected:(BOOL)connected;

@end
NS_ASSUME_NONNULL_END
