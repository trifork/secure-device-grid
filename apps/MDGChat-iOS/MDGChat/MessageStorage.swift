//
//  MessageStorage.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 02/10/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit
import MDG

enum MessageSender {
    case Me, Them, Info
}

struct Message {
    let data: Data
    let sender: MessageSender
    let peerId: String
    var isRead: Bool

    var text: String? {
        let messageText: String
        if let text = NSString(data: self.data, encoding: String.Encoding.utf8.rawValue) as? String {
            messageText = text
        } else {
            messageText = self.data.hexString
        }
        return messageText
    }
}

protocol MessageStorageDelegate {
    func added(message: Message)
}

class MessageStorage: NSObject {
    var delegate: MessageStorageDelegate?
    var messages = [Message]()

    func add(data: Data, forConnection connection: MDGPeerConnection, sender: MessageSender) {
        if let peerId = connection.peerId {
            let message = Message(data: data, sender: sender, peerId: peerId, isRead: false)
            self.messages.append(message)
            self.delegate?.added(message: message)
        }
    }

    func markAllAsRead(forPeerId: String?) {
        for index in 0..<self.messages.count {
            if forPeerId == nil {
                self.messages[index].isRead = true
            } else if self.messages[index].peerId == forPeerId {
                self.messages[index].isRead = true
            }
        }
    }
}

extension Data {
    var hexString: String {
        return map { String(format: "%02hhx", $0) }.joined()
    }
}
