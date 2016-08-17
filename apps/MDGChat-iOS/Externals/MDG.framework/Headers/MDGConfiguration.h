//
//  MDGConfiguration.h
//  MDG
//
//  Created by Bjarke Hesthaven Søndergaard on 03/03/16.
//  Copyright © 2016 Trifork A/S. All rights reserved.
//

@import Foundation;

@class MDGServerEndpoint;

NS_ASSUME_NONNULL_BEGIN
@interface MDGConfiguration : NSObject

@property (copy, nonatomic, readonly) NSArray<MDGServerEndpoint *> *servers;
@property (copy, nonatomic, readonly) NSArray<NSData *> *trustedServerKeys;
@property (copy, nonatomic, nullable, readonly) NSString *mdnsName;

- (instancetype)initWithServers:(NSArray<MDGServerEndpoint *> *)servers trustedServerKeys:(NSArray<NSData *> *)trustedServerKeys mdnsName:(nullable NSString *)mdnsName;

@end
NS_ASSUME_NONNULL_END
