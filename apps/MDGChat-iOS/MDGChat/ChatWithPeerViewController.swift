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
    var waitingMessageData: Data?

    let maxCharCount: UInt = 256
    let messageCellIdentifier: String = "MessageCell"
    let infoMessage = "The SDG Tool will log all data on the SDG backend system."

    override class func tableViewStyle(for decoder: NSCoder) -> UITableViewStyle {
        return .plain
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        setupView()
    }

    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)

        messageStorage.delegate = self
        client.connectionDelegate = self

        if let peerId = self.connection.peerId {
            self.messages = self.messageStorage.messages.filter { $0.peerId == peerId }.reversed()
        } else {
            self.messages = []
        }
        setupInfoMessage()
        self.tableView?.reloadData()
    }

    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)

        self.waitingMessageData = nil
        self.messageStorage.markAllAsRead(forPeerId: self.connection.peerId)
    }

    func setupView() {
        self.stateButton.isEnabled = false
        self.bounces = true
        self.shakeToClearEnabled = true
        self.isKeyboardPanningEnabled = true
        self.shouldScrollToBottomAfterKeyboardShows = false
        self.isInverted = true

        self.textView.placeholder = "Type something..."
        self.textView.placeholderColor = UIColor.lightGray

        self.rightButton.setTitle("Send", for: .normal)

        self.textInputbar.autoHideRightButton = true
        self.textInputbar.maxCharCount = maxCharCount
        self.textInputbar.counterPosition = .top
        self.textInputbar.counterStyle = .split
        self.rightButton.tintColor = triforkOrange

        self.tableView?.backgroundColor = UIColor.white
        self.tableView?.register(MessageCell.classForCoder(), forCellReuseIdentifier: messageCellIdentifier)
        self.tableView?.rowHeight = UITableViewAutomaticDimension
        self.tableView?.estimatedRowHeight = 44.0
        self.tableView?.separatorStyle = .none
        self.registerClass(forTypingIndicatorView: UILabel.self)
    }

    func setupInfoMessage() {
        if self.messages.isEmpty {
            let infoData = self.infoMessage.data(using: String.Encoding.utf8) ?? Data()
            self.messageStorage.add(data: infoData, forConnection: self.connection, sender: .Info)
        }
    }

    func add(message: Message) {
        let indexPath = IndexPath(row: 0, section: 0)
        let rowAnimation: UITableViewRowAnimation = self.isInverted ? .top : .bottom
        let scrollPosition: UITableViewScrollPosition = self.isInverted ? .bottom : .top
        self.tableView?.beginUpdates()
        self.messages.insert(message, at: 0)
        self.tableView?.insertRows(at: [indexPath], with: rowAnimation)
        self.tableView?.endUpdates()

        self.tableView?.scrollToRow(at: indexPath, at: scrollPosition, animated: true)
    }

    func send(data: Data) throws {
        do {
            try self.connection.send(data)
        } catch {
            // Try to reconnect and send data again
            self.waitingMessageData = data
            do {
                self.connection = try client.connectTo(peerId: connection.peerId ?? "")
            } catch {
                return
            }
        }
        self.messageStorage.add(data: data, forConnection: self.connection, sender: .Me)
    }
}

extension ChatWithPeerViewController {
    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return self.messages.count
    }

    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let message = self.messages[indexPath.row]

        let cell = tableView.dequeueReusableCell(withIdentifier: messageCellIdentifier, for: indexPath) as? MessageCell ?? MessageCell()
        cell.message = message
        cell.transform = self.tableView?.transform ?? CGAffineTransform()
        return cell
    }
}

extension ChatWithPeerViewController {
    override func didPressRightButton(_ sender: Any?) {
        self.textView.refreshFirstResponder()
        let data = self.textView.text.data(using: String.Encoding.utf8) ?? Data()
        _ = try? send(data: data)

        super.didPressRightButton(sender)
    }

    @IBAction func stateButtonPressed(_ sender: Any) {
        do {
            self.connection = try client.connectTo(peerId: self.connection.peerId ?? "")
        } catch {}
    }
}

extension ChatWithPeerViewController: MessageStorageDelegate {
    func added(message: Message) {
        if let peerId = self.connection.peerId, message.peerId == peerId {
            DispatchQueue.main.async { [weak self] in
                self?.add(message: message)
            }
        }
    }
}

extension ChatWithPeerViewController: ConnectionDelegate {
    func routingChanged(status: MDGRoutingStatus, connection: MDGPeerConnection) {
        DispatchQueue.main.async { [weak self] in
            if self?.connection.peerId == connection.peerId {
                switch status {
                case .connected:
                    if let messageData = self?.waitingMessageData {
                        _ = try? self?.send(data: messageData)
                        self?.waitingMessageData = nil
                    }
                    self?.stateButton.isEnabled = false
                case .disconnected:
                    self?.stateButton.isEnabled = true
                case .peerNotAvailable:
                    self?.logView.add(line: "Peer not available on conn_id \(connection.connectionId)")
                    _ = self?.navigationController?.popViewController(animated: true)
                default: break
                }
            }
        }
    }
}
