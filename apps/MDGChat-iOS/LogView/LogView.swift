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

    private func getTimestamp() -> String {
        let formatter = DateFormatter()
        formatter.dateFormat = "HH:mm:ss.SSS"
        return formatter.string(from: Date())
    }
}

// Self-adjusting log view with toolbar style using shared log store
public class LogView: UIViewController {
    public static let sharedLogView = LogView()

    let toolBar = UIToolbar()
    let minimizeButton = UIButton()
    let maximizeButton = UIButton()
    let textView = UITextView()
    private let logStore = LogStore.sharedLogStore

    var heightConstraint: NSLayoutConstraint?
    var minimized = false {
        didSet {
            if minimized {
                maximized = false
                heightConstraint?.constant = minHeight
                minimizeButton.setTitle("△", for: .normal)
            } else {
                heightConstraint?.constant = maxHeight
                minimizeButton.setTitle("▽", for: .normal)
                maximizeButton.isEnabled = true
            }
        }
    }
    var maximized = false {
        didSet {
            if maximized {
                minimized = false
                if let height = view.superview?.frame.height {
                    heightConstraint?.constant = height - 20
                }
                maximizeButton.setTitle("◲", for: .normal)
            } else {
                heightConstraint?.constant = maxHeight
                maximizeButton.setTitle("⧈", for: .normal)
            }
        }
    }

    var maxHeight: CGFloat = 87
    var minHeight: CGFloat = 18
    var tintColor: UIColor = UIColor.lightGray

    public func show() {
        self.view.isHidden = false
    }

    public func hide() {
        self.view.isHidden = true
    }

    init() {
        super.init(nibName: nil, bundle: nil)

        renderView()
        renderTextView()
        renderMaximizeButton()
        renderMinimizeButton()
    }

    required public init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    public func add(line: String) {
        textView.text = logStore.add(line: line)
        scrollToTop()
    }

    public func update() {
        let log = logStore.getLog()
        if log != textView.text {
            textView.text = log
        }
        scrollToTop()
    }

    func renderView() {
        if let window = UIApplication.shared.keyWindow {
            view.isHidden = true
            window.addSubview(view)
        }

        self.view.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint(item: view, attribute: .centerX, relatedBy: .equal, toItem: view.superview, attribute: .centerX, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: view, attribute: .width, relatedBy: .equal, toItem: view.superview, attribute: .width, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: view, attribute: .bottom, relatedBy: .equal, toItem: view.superview, attribute: .bottom, multiplier: 1, constant: 0).isActive = true
        heightConstraint = NSLayoutConstraint(item: view, attribute: .height, relatedBy: .equal, toItem: nil, attribute: .notAnAttribute, multiplier: 1, constant: maxHeight)
        heightConstraint?.isActive = true
    }

    func scrollToTop() {
        textView.scrollRangeToVisible(NSRange(location: 0, length: 0))
    }

    func renderTextView() {
        textView.backgroundColor = UIColor.clear
        textView.isEditable = false
        textView.isSelectable = false
        textView.showsHorizontalScrollIndicator = false
        textView.textContainerInset = UIEdgeInsets(top: 2, left: 0, bottom: 2, right: 0)
        textView.text = logStore.getLog()
        toolBar.addSubview(textView)

        textView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint(item: textView, attribute: .centerX, relatedBy: .equal, toItem: textView.superview, attribute: .centerX, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: textView, attribute: .centerY, relatedBy: .equal, toItem: textView.superview, attribute: .centerY, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: textView, attribute: .width, relatedBy: .equal, toItem: textView.superview, attribute: .width, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: textView, attribute: .height, relatedBy: .equal, toItem: textView.superview, attribute: .height, multiplier: 1, constant: 0).isActive = true

        toolBar.barStyle = .default
        self.view.addSubview(toolBar)

        toolBar.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint(item: toolBar, attribute: .centerX, relatedBy: .equal, toItem: toolBar.superview, attribute: .centerX, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: toolBar, attribute: .centerY, relatedBy: .equal, toItem: toolBar.superview, attribute: .centerY, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: toolBar, attribute: .width, relatedBy: .equal, toItem: toolBar.superview, attribute: .width, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: toolBar, attribute: .height, relatedBy: .equal, toItem: toolBar.superview, attribute: .height, multiplier: 1, constant: 0).isActive = true
    }

    func renderMinimizeButton() {
        minimized = false

        minimizeButton.setTitleColor(tintColor, for: .normal)
        minimizeButton.setTitleColor(tintColor.withAlphaComponent(0.3), for: .highlighted)
        minimizeButton.titleEdgeInsets = UIEdgeInsets(top: -6, left: 0, bottom: 6, right: 0)
        minimizeButton.addTarget(self, action: #selector(minimizeButtonPressed), for: .touchUpInside)
        self.view.addSubview(minimizeButton)

        minimizeButton.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint(item: minimizeButton, attribute: .trailing, relatedBy: .equal, toItem: maximizeButton, attribute: .leadingMargin, multiplier: 1, constant: 0).isActive = true
        NSLayoutConstraint(item: minimizeButton, attribute: .top, relatedBy: .equal, toItem: maximizeButton, attribute: .top, multiplier: 1, constant: 0).isActive = true
    }

    func renderMaximizeButton() {
        maximized = false

        maximizeButton.setTitleColor(tintColor, for: .normal)
        maximizeButton.setTitleColor(tintColor.withAlphaComponent(0.3), for: .highlighted)
        maximizeButton.titleEdgeInsets = UIEdgeInsets(top: -6, left: 0, bottom: 6, right: 0)
        maximizeButton.addTarget(self, action: #selector(maximizeButtonPressed), for: .touchUpInside)
        self.view.addSubview(maximizeButton)

        maximizeButton.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint(item: maximizeButton, attribute: .trailing, relatedBy: .equal, toItem: view, attribute: .trailingMargin, multiplier: 1, constant: 15).isActive = true
        NSLayoutConstraint(item: maximizeButton, attribute: .top, relatedBy: .equal, toItem: view, attribute: .top, multiplier: 1, constant: 0).isActive = true
    }

    func minimizeButtonPressed(sender: UIButton) {
        minimized = !minimized
    }

    func maximizeButtonPressed(sender: UIButton) {
        maximized = !maximized
    }
}
