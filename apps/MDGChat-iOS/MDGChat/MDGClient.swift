//
//  MDGClient.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit
import MDG

let peerNamesKey = "PeerNamesKey"

public protocol PairingDelegate: NSObjectProtocol {
    func pairingStateChanged(state: MDGPairingState)
}

public protocol ConnectionDelegate: NSObjectProtocol {
    func routingStatusChanged(connection: MDGPeerConnection, status: MDGRoutingStatus)
}

public protocol ReceiveDataDelegate: NSObjectProtocol {
    func didReceiveData(connection: MDGPeerConnection, data: NSData)
}

public class MDGClient: NSObject {
    public static let sharedClient = MDGClient()
    let messageStorage = MessageStorage()

    private let otpLength: Int = 7
    private let core = MDGCore.sharedCore()
    private var properties: [String: String] {
        var props = [String: String]()
        if let userEmail = NSUserDefaults.standardUserDefaults().stringForKey(userEmailKey) {
            props["client_email"] = userEmail
        }
        return props
    }

    public var pairings: [String] {
        return core.pairings
    }

    private var peerNames: [String: String] = (NSUserDefaults.standardUserDefaults().objectForKey(peerNamesKey) as? [String: String]) ?? [String: String]()

    public weak var pairingDelegate: PairingDelegate?
    public weak var connectionDelegate: ConnectionDelegate?
    public weak var receiveDataDelegate: ReceiveDataDelegate?

    private override init() {
        super.init()
        core.startWithConfiguration(nil)
        core.delegate = self
    }

    public func connect() {
        _ = try? core.connectWithProperties(self.properties)
    }

    public func disconnect() {
        _ = try? core.disconnect()
        for connection in core.connections {
            _ = try? connection.closeConnection()
        }
    }

    public func reconnect() {
        self.disconnect()
        self.connect()
    }

    public func pair(otp: String) throws {
        try core.pairRemoteWithOneTimePasscode(otp)
    }

    public func revokePair(peerId: String) throws {
        try core.revokePairingWithPeerId(peerId)
    }

    public func enablePairing() throws {
        self.disablePairing()
        try core.enablePairingModeForSeconds(120)
    }

    public func disablePairing() {
        core.disablePairingMode()
    }

    public func connectToPeer(peerId: String) throws -> MDGPeerConnection {
        if let connectionIndex = self.core.connections.indexOf({ $0.peerId == peerId }) {
            return self.core.connections[connectionIndex]
        } else {
            return try self.core.placeCallRemoteWithPeerId(peerId, protocolName: "chat-client", timeout: 10)
        }
    }

    public func setPeerName(name: String?, forPeerId peerId: String) {
        guard let name = name where name != "" else {
            return
        }
        peerNames[peerId] = name
        NSUserDefaults.standardUserDefaults().setObject(peerNames, forKey: peerNamesKey)
        NSUserDefaults.standardUserDefaults().synchronize()
    }

    public func peerName(peerId: String) -> String {
        if let peerName = peerNames[peerId] where peerName != "" {
            return peerName
        }
        return peerId.substringToIndex(peerId.startIndex.advancedBy(12)) + "..."
    }

    public func formatOtp(otp: String) -> String {
        var formattedOtp = ""
        let allowedCharacters = NSCharacterSet(charactersInString: "0123456789").invertedSet
        var strippedOtp: String = otp.componentsSeparatedByCharactersInSet(allowedCharacters).joinWithSeparator("")

        if strippedOtp.characters.count > otpLength {
            strippedOtp = strippedOtp.substringToIndex(strippedOtp.startIndex.advancedBy(otpLength))
        }

        for (i, number) in strippedOtp.characters.enumerate() {
            formattedOtp.append(number)
            if (i % 3 == 2) && (i < strippedOtp.characters.count - 1) {
                formattedOtp += "-"
            }
        }
        return formattedOtp
    }
}

extension MDGClient: MDGCoreDelegate {
    public func pairingStateChanged(state: MDGPairingState) {
        if let delegate = pairingDelegate {
            delegate.pairingStateChanged(state)
        }
    }

    public func routingStatusChanged(connection: MDGPeerConnection, toStatus status: MDGRoutingStatus) {
        if let delegate = connectionDelegate {
            delegate.routingStatusChanged(connection, status: status)
        }
    }

    public func connection(connection: MDGPeerConnection, didReceiveData data: NSData) {
        self.messageStorage.addData(data, forConnection: connection, sender: .Them)
        if let delegate = receiveDataDelegate {
            delegate.didReceiveData(connection, data: data)
        }
    }
}

extension MDGPairingStatus {
    var stringValue: String {
        let value: String
        switch self {
        case .Completed:
            value = "Pairing completed"
        case .FailedGeneric:
            value = "Pairing failed generic"
        case .FailedOneTimePasscode:
            value = "One time passcode failed"
        case .OneTimePasscodeReady:
            value = "One time passcode is ready"
        case .Starting:
            value = "Pairing starting"
        case .Unknown:
            value = "Pairing status unknown"
        }
        return value
    }
}
