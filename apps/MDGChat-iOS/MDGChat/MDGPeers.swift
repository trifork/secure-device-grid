//
//  MDGPeers.swift
//  MDGChat
//
//  Created by Martin Rodalgaard on 29/08/16.
//  Copyright © 2016 Trifork A/S. All rights reserved.
//

import Foundation

// MDGPeers gets and stores the paired peers
public class MDGPeers: NSObject {
    public static let sharedPeers = MDGPeers()

    private let peerNamesKey = "PeerNamesKey"
    private var peerNames: [String: String]!

    override init() {
        super.init()
        self.peerNames = (NSUserDefaults.standardUserDefaults().objectForKey(self.peerNamesKey) as? [String: String]) ?? [String: String]()
    }

    public func setPeerName(name: String?, forPeerId peerId: String) {
        guard let name = name where name != "" else {
            return
        }
        self.peerNames[peerId] = name
        NSUserDefaults.standardUserDefaults().setObject(self.peerNames, forKey: self.peerNamesKey)
        NSUserDefaults.standardUserDefaults().synchronize()
    }

    public func peerName(peerId: String) -> String {
        if let peerName = self.peerNames[peerId] where peerName != "" {
            return peerName
        }
        return peerId.substringToIndex(peerId.startIndex.advancedBy(12)) + "..."
    }
}
