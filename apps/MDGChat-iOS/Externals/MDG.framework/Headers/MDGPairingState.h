//
//  MDGPairingState.h
//  MDG-iOS
//
//  Created by Bjarke Hesthaven Søndergaard on 27/08/15.
//  Copyright (c) 2015 Trifork A/S. All rights reserved.
//

@import Foundation;
#import "MDGCore.h"

@interface MDGPairingState : NSObject

@property (nonatomic, readonly) MDGPairingStatus status;
@property (nonatomic, readonly, getter=isRemote) BOOL remote;
@property (nonatomic, readonly) NSUInteger startTimestamp;
@property (nonatomic, readonly) NSUInteger endTimestamp;
@property (strong, nonatomic, readonly) NSString *oneTimePasscode;
@property (strong, nonatomic, readonly) NSString *peerId;

- (instancetype)initWithPairingStatus:(MDGPairingStatus)status isRemote:(BOOL)isRemote startTimestamp:(NSUInteger)startTimestamp endTimestamp:(NSUInteger)endTimestamp oneTimePasscode:(NSString *)oneTimePasscode peerId:(NSString *)peerId;

@end
