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
        self.peerNames = (UserDefaults.standard.object(forKey: self.peerNamesKey) as? [String: String]) ?? [String: String]()
    }

    public func setPeer(name: String?, forPeerId peerId: String) {
        guard let name = name, name != "" else {
            return
        }
        self.peerNames[peerId] = name
        UserDefaults.standard.set(self.peerNames, forKey: self.peerNamesKey)
        UserDefaults.standard.synchronize()
    }

    public func peerName(forPeerId: String) -> String {
        if let peerName = self.peerNames[forPeerId], peerName != "" {
            return peerName
        }
        let index = forPeerId.index(forPeerId.startIndex, offsetBy: 12)
        return forPeerId.substring(to: index) + "..."
    }
}
