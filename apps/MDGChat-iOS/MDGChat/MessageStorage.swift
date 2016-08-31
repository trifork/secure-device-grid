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
    let data: NSData
    let sender: MessageSender
    let peerId: String
    var isRead: Bool

    var text: String? {
        let messageText: String
        if let text = NSString(data: self.data, encoding: NSUTF8StringEncoding) as? String {
            messageText = text
        } else {
            messageText = self.data.hexString
        }
        return messageText
    }
}

protocol MessageStorageDelegate {
    func addedMessage(message: Message)
}

class MessageStorage: NSObject {
    var delegate: MessageStorageDelegate?
    var messages = [Message]()

    func addData(data: NSData, forConnection connection: MDGPeerConnection, sender: MessageSender) {
        if let peerId = connection.peerId {
            let message = Message(data: data, sender: sender, peerId: peerId, isRead: false)
            self.messages.append(message)
            self.delegate?.addedMessage(message)
        }
    }

    func markAllAsRead(peerId: String?) {
        for index in 0..<self.messages.count {
            if self.messages[index].peerId == peerId {
                self.messages[index].isRead = true
            }
        }
    }
}

extension NSData {
    var hexString: String {
        let bytes = UnsafePointer<UInt8>(self.bytes)

        var hexString = ""
        for i in 0..<self.length {
            let byte = bytes.advancedBy(i)
            hexString += String(format: "%02x", arguments: [UInt(byte.memory)])
        }

        return hexString
    }
}
