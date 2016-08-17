//
//  MDGLogFormatter.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 15/01/16.
//  Copyright © 2016 Trifork A/S. All rights reserved.
//

import Foundation
import MDG
import CocoaLumberjack

final class MDGLogFormatter: NSObject, DDLogFormatter {
    private static var dateFormatter: NSDateFormatter = {
        let formatter = NSDateFormatter()
        formatter.dateFormat = "yyyy-MM-dd HH:mm:ss.SSS"
        return formatter
    }()

    let showsDate: Bool

    init(showsDate: Bool = true) {
        self.showsDate = showsDate
    }

    func formatLogMessage(logMessage: DDLogMessage!) -> String! {
        let logLevel: String
        let logFlag = logMessage.flag
        if logFlag.contains(.Error) {
            logLevel = "E"
        } else if logFlag.contains(.Warning) {
            logLevel = "W"
        } else if logFlag.contains(.Info) {
            logLevel = "I"
        } else if logFlag.contains(.Debug) {
            logLevel = "D"
        } else if logFlag.contains(.Verbose) {
            logLevel = "V"
        } else {
            logLevel = "?"
        }

        let context: String
        switch logMessage.context {
        case MDGLumberjackContext:
            context = "MDG"
        default:
            context = "MDGChat"
        }

        let date: String
        if showsDate {
            date = MDGLogFormatter.dateFormatter.stringFromDate(logMessage.timestamp) + " "
        } else {
            date = ""
        }

        return "\(date)[\(context)] |\(logLevel)| |\(logMessage.fileName):#\(logMessage.line)| \(logMessage.message)"
    }
}
