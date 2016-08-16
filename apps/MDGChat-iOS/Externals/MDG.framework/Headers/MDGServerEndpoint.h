//
//  MDGServerEndpoint.h
//  MDG
//
//  Created by Bjarke Hesthaven Søndergaard on 03/03/16.
//  Copyright © 2016 Trifork A/S. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
@interface MDGServerEndpoint : NSObject

@property (copy, nonatomic) NSString *hostname;
@property (assign, nonatomic) NSInteger port;

- (instancetype)initWithHostname:(NSString *)hostname port:(NSInteger)port;

@end
NS_ASSUME_NONNULL_END
