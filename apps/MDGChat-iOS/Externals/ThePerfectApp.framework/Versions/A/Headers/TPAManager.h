//
//  TPAManager.h
//  ThePerfectAppLib
//
//  Created by Peder Toftegaard Olsen on 05/06/12.
//  Copyright (c) 2012 Trifork GmbH. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifndef HAS_THE_PERFECT_APP_LIBRARY
#define HAS_THE_PERFECT_APP_LIBRARY 1
#endif


typedef NS_OPTIONS(NSUInteger, TPALoggingDestination)
{
    TPALoggingDestinationNone = 0x00,
    TPALoggingDestinationConsole = 1 << 0,
    TPALoggingDestinationRemote = 1 << 1,
};

typedef NS_ENUM(NSUInteger, TPACrashReporting)
{
    /** Crash reporting is disabled. */
    TPACrashReportingDisabled,
    /** User is asked before sending a crash report. */
    TPACrashReportingAlwaysAsk,
    /** Crash reports will be sent without asking the user. */
    TPACrashReportingAlwaysSend,
};

typedef NS_ENUM(NSUInteger, TPAFeedbackInvocation)
{
    TPAFeedbackInvocationDisabled = 0,
    TPAFeedbackInvocationEnabled = 1 << 0,
    TPAFeedbackInvocationEventShake = TPAFeedbackInvocationEnabled | 1 << 1,
};

/**
 * The TPAManager is responsible for handling update notifications and crash reporting to a TPA server.
 * To enable either a baseURL and projectUuid must be specified.
 */
@interface TPAManager : NSObject

/** Get the one and only TPAManager instance. */
+(TPAManager *)sharedManager;


#pragma mark - TPA server configuration

/**
 * Starts the TPA Manager. No properties of the manager can be modified after calling this method.
 *
 * If a debugger is attached to the app, or the app is executing in the iOS simulator, the
 * TPAManager will not start.
 *
 * @param baseUrl The base URL of the TPA server. Usually of the form https://server/.
 * @param projectUuid Project UUID as it is known to the TPA server.
 */
- (void)startManagerWithBaseUrl:(NSString *)baseUrl projectUuid:(NSString *)projectUuid;


#pragma mark - Update notifications

/** 
 * Enable update notifications by polling the TPAW server when the app becomes active.
 *
 * Cannot be enabled for apps distributed via the App Store.
 * Cannot be enabled for extensions.
 *
 * Disabled by default.
 */
@property (nonatomic, readwrite, getter = isUpdateNotificationsEnabled) BOOL updateNotificationsEnabled;


#pragma mark - Crash reporting

/**
 * Enable crash reporting by uploading crash reports to the TPAW server on next restart of the app after a crash.
 *
 * Only TPACrashReportingDisabled and TPACrashReportingAlwaysSend are valid for extensions.
 *
 * TPACrashReportingDisabled by default.
 */
@property (nonatomic, assign) TPACrashReporting crashReporting;

/**
 * Trap fatal signals via a Mach exception server.
 *
 * Disabled by default.
 */
@property (nonatomic, assign, getter=isMachExceptionHandlerEnabled) BOOL enableMachExceptionHandler;

#pragma mark - Session Recording

/**
 * If enabled, session start/end events will be sent to the TPA server containing basic information about the device.
 *
 * Disabled by default.
 */
@property (nonatomic, getter = isSessionRecordingEnabled) BOOL sessionRecordingEnabled;


#pragma mark - Remote Logging

/**
 * Logging destinations.
 *
 * Value cannot be changed for apps distrbuted via the App Store.
 * Only TPALoggingDestinationNone and TPALoggingDestinationConsole are valid for extensions.
 *
 * Defaults to TPALoggingDestinationConsole, except for apps distributed via the App Store where it is TPALoggingDestinationNone.
 */
@property (nonatomic) TPALoggingDestination loggingDestinations;


#pragma mark - Feedback

/**
 *
 * Disabled by default.
 */
@property (nonatomic, assign) TPAFeedbackInvocation feedbackInvocation;

/**
 * Programmatically invoke the feedback UI.
 */
- (void)invokeFeedback;


/**
 * Log message to loggingDestinations.
 */
- (void)logMessage:(NSString *)message;

/**
 * Log message to loggingDestinations.
 */
- (void)log:(NSString *)format, ...;

/**
 * Log message to loggingDestinations.
 */
- (void)log:(NSString *)format args:(va_list)args;


#pragma mark - Esoteric stuff

/**
 * Enable debug logging. Will output extra information about the TPA library to the destinations specified in loggingDestinations.
 * Cannot be enabled for apps distributed via the App Store.
 *
 * Disabled by default.
 */
@property (nonatomic, readwrite, getter = isDebugLoggingEnabled) BOOL debugLoggingEnabled;

/**
 * Validate TPA server certificate.
 * Disable validation if using a self-signed certificate.
 *
 * Enabled by default.
 */
@property (nonatomic, readwrite) BOOL validateServerCertificate;
@end


/**
 * Logs message to the destinations specified by [TPAManager loggingDestinations].
 */
extern void TPALog(NSString *format, ...);

/**
 * Logs message to the destinations specified by [TPAManager loggingDestinations].
 */
extern void TPALogv(NSString *format, va_list args);
