//
//  TPALogger.swift
//  MDGChat
//
//  Created by Anders Eriksen on 15/01/2016.
//  Copyright © 2016 Trifork A/S. All rights reserved.
//

import Foundation
import ThePerfectApp
import CocoaLumberjack

final class TPALogger: DDAbstractLogger {
    static let sharedInstance = TPALogger()

    private var _logFormatter: DDLogFormatter?
    override var logFormatter: DDLogFormatter? {
        get {
            return _logFormatter
        }
        set {
            _logFormatter = newValue
        }
    }

    private override init() {}

    override func logMessage(logMessage: DDLogMessage) {
        let logMsg: String
        if let formatter = self._logFormatter {
            logMsg = formatter.formatLogMessage(logMessage)
        } else {
            logMsg = logMessage.message
        }

        TPALogv(logMsg, getVaList([]))
    }
}
