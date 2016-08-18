//
//  ChatWithPeerViewController.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit
import SlackTextViewController
import MDG

class ChatWithPeerViewController: SLKTextViewController {
    @IBOutlet weak var stateButton: UIBarButtonItem!

    var connection: MDGPeerConnection!
    var messageStorage: MessageStorage!
    let client = MDGClient.sharedClient
    let logView = LogView()

    var messages = [Message]()
    var waitingMessageData: NSData?

    let maxCharCount: UInt = 256
    let messageCellIdentifier: String = "MessageCell"

    override var tableView: UITableView {
        get {
            return super.tableView!
        }
    }

    override class func tableViewStyleForCoder(decoder: NSCoder) -> UITableViewStyle {
        return .Plain
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        self.stateButton.enabled = false
        self.bounces = true
        self.shakeToClearEnabled = true
        self.keyboardPanningEnabled = true
        self.shouldScrollToBottomAfterKeyboardShows = false
        self.inverted = true

        self.textView.placeholder = "Type something..."
        self.textView.placeholderColor = .lightGrayColor()

        self.rightButton.setTitle("Send", forState: .Normal)

        self.textInputbar.autoHideRightButton = true
        self.textInputbar.maxCharCount = maxCharCount
        self.textInputbar.counterPosition = .Top
        self.textInputbar.counterStyle = .Split
        self.rightButton.tintColor = triforkOrange

        self.tableView.backgroundColor = .whiteColor()
        self.tableView.registerClass(MessageCell.classForCoder(), forCellReuseIdentifier: messageCellIdentifier)
        self.tableView.rowHeight = UITableViewAutomaticDimension
        self.tableView.estimatedRowHeight = 44.0
        self.tableView.separatorStyle = .None
        self.registerClassForTypingIndicatorView(UILabel.self)
    }

    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)

        messageStorage.delegate = self
        client.connectionDelegate = self

        if let peerId = self.connection.peerId {
            self.messages = self.messageStorage.messages.filter { $0.peerId == peerId }.reverse()
        } else {
            self.messages = []
        }
        self.tableView.reloadData()
    }

    override func viewDidDisappear(animated: Bool) {
        super.viewDidDisappear(animated)
        self.waitingMessageData = nil
    }

    func addMessage(message: Message) {
        let indexPath = NSIndexPath(forRow: 0, inSection: 0)
        let rowAnimation: UITableViewRowAnimation = self.inverted ? .Top : .Bottom
        let scrollPosition: UITableViewScrollPosition = self.inverted ? .Bottom : .Top
        self.tableView.beginUpdates()
        self.messages.insert(message, atIndex: 0)
        self.tableView.insertRowsAtIndexPaths([indexPath], withRowAnimation: rowAnimation)
        self.tableView.endUpdates()

        self.tableView.scrollToRowAtIndexPath(indexPath, atScrollPosition: scrollPosition, animated: true)
    }

    func sendData(data: NSData) throws {
        do {
            try self.connection.sendData(data)
        } catch {
            // Try to reconnect and send data again
            self.waitingMessageData = data
            do {
                self.connection = try client.connectToPeer(connection.peerId ?? "")
            } catch {
                return
            }
        }
        self.messageStorage.addData(data, forConnection: self.connection, sender: .Me)
    }
}

extension ChatWithPeerViewController {
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return self.messages.count
    }

    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let message = self.messages[indexPath.row]

        let cell = tableView.dequeueReusableCellWithIdentifier(messageCellIdentifier, forIndexPath: indexPath) as? MessageCell
        cell?.message = message
        cell?.transform = self.tableView.transform

        return cell ?? MessageCell()
    }
}

extension ChatWithPeerViewController {
    override func didPressRightButton(sender: AnyObject!) {
        self.textView.refreshFirstResponder()
        let data = self.textView.text.dataUsingEncoding(NSUTF8StringEncoding) ?? NSData()
        _ = try? sendData(data)

        super.didPressRightButton(sender)
    }

    @IBAction func stateButtonPressed(sender: AnyObject) {
        do {
            self.connection = try client.connectToPeer(self.connection.peerId ?? "")
        } catch {}
    }
}

extension ChatWithPeerViewController: MessageStorageDelegate {
    func addedMessage(message: Message) {
        if let peerId = self.connection.peerId where message.peerId == peerId {
            dispatch_async(dispatch_get_main_queue()) { [weak self] in
                self?.addMessage(message)
            }
        }
    }
}

extension ChatWithPeerViewController: ConnectionDelegate {
    func routingStatusChanged(connection: MDGPeerConnection, status: MDGRoutingStatus) {
        dispatch_async(dispatch_get_main_queue()) { [weak self] in
            if self?.connection.peerId == connection.peerId {
                switch status {
                case .Connected:
                    if let messageData = self?.waitingMessageData {
                        _ = try? self?.sendData(messageData)
                        self?.waitingMessageData = nil
                    }
                    self?.stateButton.enabled = false
                case .Disconnected:
                    self?.stateButton.enabled = true
                case .PeerNotAvailable:
                    dispatch_async(dispatch_get_main_queue()) { [weak self] in
                        self?.logView.addLine("Peer not available on conn_id \(connection.connectionId)")
                        self?.navigationController?.popViewControllerAnimated(true)
                    }
                default: break
                }
            }
        }
    }
}
