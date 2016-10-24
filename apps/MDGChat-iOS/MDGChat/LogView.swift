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

    func getLog() -> String {
        return log
    }

    func clearLog() {
        log = ""
    }

    func add(line: String) -> String {
        let newLine = "\(getTimestamp()): \(line)\n"
        log = "\(newLine)\(log)"
        return log
    }

    func getTimestamp() -> String {
        let formatter = DateFormatter()
        formatter.dateFormat = "HH:mm:ss.SSS"
        return formatter.string(from: Date())
    }
}

// Self-adjusting log view with toolbar style using shared log store
public class LogView: UIView {
    private let toolBar = UIToolbar()
    private let textView = UITextView()
    private let logStore = LogStore.sharedLogStore

    var barStyle = UIBarStyle.default {
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
        updateFrame(frame: frame)
        createView()

        NotificationCenter.default.addObserver(forName: NSNotification.Name.UIDeviceOrientationDidChange, object: nil, queue: nil, using: { _ in
            self.updateFrame()
        })
    }

    required public init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }

    func add(line: String) {
        textView.text = logStore.add(line: line)
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
            let windowWidth = UIApplication.shared.keyWindow?.frame.size.width ?? 0
            let windowHeight = UIApplication.shared.keyWindow?.frame.size.height ?? 0
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
        textView.backgroundColor = UIColor.clear
        textView.isEditable = false
        textView.isSelectable = false
        textView.showsHorizontalScrollIndicator = false
        textView.textContainerInset = UIEdgeInsets(top: 6, left: 0, bottom: 2, right: 0)
        textView.text = logStore.getLog()

        toolBar.addSubview(textView)
        toolBar.barStyle = barStyle

        self.addSubview(toolBar)
    }
}
