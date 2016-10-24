//
//  AppDelegate.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 14/09/15.
//  Copyright (c) 2015 Trifork A/S. All rights reserved.
//

import UIKit
import IQKeyboardManagerSwift

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {
    var window: UIWindow?

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey : Any]? = nil) -> Bool {
        IQKeyboardManager.sharedManager().enable = true

        MDGClient.sharedClient.connect()
        return true
    }

    func applicationWillResignActive(_ application: UIApplication) {}

    func applicationDidEnterBackground(_ application: UIApplication) {
        MDGClient.sharedClient.disconnect()
    }

    func applicationWillEnterForeground(_ application: UIApplication) {}

    func applicationDidBecomeActive(_ application: UIApplication) {
        MDGClient.sharedClient.connect()
    }

    func applicationWillTerminate(_ application: UIApplication) {}
}
