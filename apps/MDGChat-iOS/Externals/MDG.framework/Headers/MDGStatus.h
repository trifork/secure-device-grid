//
//  MDGStatus.h
//  MDG-iOS
//
//  Created by Bjarke Hesthaven Søndergaard on 27/08/15.
//  Copyright (c) 2015 Trifork A/S. All rights reserved.
//

@import Foundation;

@interface MDGStatus : NSObject

@property (nonatomic, readonly) BOOL connectSwitch;
@property (nonatomic, readonly) BOOL connectedToMdg;
@property (nonatomic, readonly) BOOL pairingMode;
@property (nonatomic, readonly) BOOL aggressivePing;
@property (nonatomic, readonly) BOOL remoteLoggingEnabled;

- (instancetype)initWithConnectSwitch:(BOOL)connectSwitch connectedToMdg:(BOOL)connectedToMdg pairingMode:(BOOL)pairingMode aggressivePing:(BOOL)aggressivePing remoteLoggingEnabled:(BOOL)remoteLoggingEnabled;

@end
