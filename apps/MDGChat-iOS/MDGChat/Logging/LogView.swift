//
//  LogView.swift
//  MDGChat
//
//  Created by Martin Rodalgaard on 11/08/16.
//  Copyright © 2016 Trifork A/S. All rights reserved.
//

import UIKit

// Really simple shared instance log storage used by LogView
private class LogStore: NSObject {
    static let sharedLogStore = LogStore()
    private var log: String = "Log Output"

    override init() {
        super.init()
    }

    func getLog() -> String {
        return log
    }

    func clearLog() {
        log = ""
    }

    func addLine(text: String) -> String {
        let line = "\(getTimestamp()): \(text)\n"
        log = "\(line)\(log)"
        return log
    }

    func getTimestamp() -> String {
        let formatter = NSDateFormatter()
        formatter.dateFormat = "HH:mm:ss.SSS"
        return formatter.stringFromDate(NSDate())
    }
}

// Self-adjusting log view with toolbar style using shared log store
public class LogView: UIView {
    private let toolBar = UIToolbar()
    private let textView = UITextView()
    private let logStore = LogStore.sharedLogStore

    var barStyle = UIBarStyle.Default {
        didSet {
            self.toolBar.barStyle = self.barStyle
        }
    }
    var minHeight: CGFloat = 30 {
        didSet {
            updateFrame()
        }
    }
    var maxHeight: CGFloat = 90 {
        didSet {
            updateFrame()
        }
    }

    override init(frame: CGRect) {
        super.init(frame: frame)
        updateFrame(frame)
        createView()

        NSNotificationCenter.defaultCenter().addObserverForName(UIDeviceOrientationDidChangeNotification, object: nil, queue: nil, usingBlock: { _ in
            self.updateFrame()
        })
    }

    required public init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }

    func addLine(text: String) {
        textView.text = logStore.addLine(text)
        scrollToTop()
    }

    func update() {
        let log = logStore.getLog()
        if log != textView.text {
            textView.text = log
        }
        scrollToTop()
    }

    private func scrollToTop() {
        textView.scrollRangeToVisible(NSRange(location: 0, length: 0))
    }

    private func updateFrame(frame: CGRect = CGRect(x: 0, y: 0, width: 0, height: 0)) {
        if frame.size.height == 0 && frame.size.width == 0 {
            let windowWidth = UIApplication.sharedApplication().keyWindow?.frame.size.width ?? 0
            let windowHeight = UIApplication.sharedApplication().keyWindow?.frame.size.height ?? 0
            let mainBound = CGRect(x: 0, y: windowHeight - maxHeight, width: windowWidth, height: maxHeight)
            self.frame = mainBound
        } else {
            maxHeight = frame.size.height
            self.frame = frame
        }

        toolBar.frame = self.frame
        toolBar.frame.origin.y = 0
        textView.frame = self.frame
        textView.frame.origin.y = 0
    }

    private func createView() {
        textView.backgroundColor = UIColor.clearColor()
        textView.editable = false
        textView.selectable = false
        textView.showsHorizontalScrollIndicator = false
        textView.textContainerInset = UIEdgeInsets(top: 6, left: 0, bottom: 2, right: 0)
        textView.text = logStore.log

        toolBar.addSubview(textView)
        toolBar.barStyle = barStyle

        self.addSubview(toolBar)
    }
}
