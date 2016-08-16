//
//  AppDelegate.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 14/09/15.
//  Copyright (c) 2015 Trifork A/S. All rights reserved.
//

import UIKit
import ThePerfectApp
import IQKeyboardManagerSwift
import CocoaLumberjack

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {
    var window: UIWindow?

    func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
        IQKeyboardManager.sharedManager().enable = true

        MDGClient.sharedClient.connect()
        return true
    }

    func applicationWillResignActive(application: UIApplication) {}

    func applicationDidEnterBackground(application: UIApplication) {
        MDGClient.sharedClient.disconnect()
    }

    func applicationWillEnterForeground(application: UIApplication) {}

    func applicationDidBecomeActive(application: UIApplication) {
        MDGClient.sharedClient.connect()
    }

    func applicationWillTerminate(application: UIApplication) {}
}
