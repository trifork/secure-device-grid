//
//  MDGClient.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import MDG

public protocol PairingDelegate: NSObjectProtocol {
    func pairingChanged(state: MDGPairingState)
}

public protocol ConnectionDelegate: NSObjectProtocol {
    func routingChanged(status: MDGRoutingStatus, connection: MDGPeerConnection)
}

public protocol ReceiveDataDelegate: NSObjectProtocol {
    func didReceive(data: Data, connection: MDGPeerConnection)
}

// MDGClient handles interaction with the MDG framework
public class MDGClient: NSObject {
    public static let sharedClient = MDGClient()
    let messageStorage = MessageStorage()

    private let otpLength: Int = 7
    private let core = MDGCore.shared()
    private var properties: [String: String] {
        var props = [String: String]()
        if let userEmail = UserDefaults.standard.string(forKey: userEmailKey) {
            props["client_email"] = userEmail
        }
        return props
    }

    private var configuration: MDGConfiguration {
        // Server endpoint and key for test server. Do not use in production!
        let servers = [
            MDGServerEndpoint(hostname: "77.66.11.95", port: 443)
        ]
        let keys = [
            Data(bytes: [81, 13, 101, 52, 29, 109, 136, 196, 86, 91, 34, 91, 3, 19, 150, 3, 215, 43, 210, 9, 242, 146, 119, 188, 153, 245, 78, 232, 94, 113, 37, 47])
        ]
        return MDGConfiguration(servers: servers, trustedServerKeys: keys, mdnsName: nil)
    }

    public var pairings: [String] {
        return core.pairings
    }

    public weak var pairingDelegate: PairingDelegate?
    public weak var connectionDelegate: ConnectionDelegate?
    public weak var receiveDataDelegate: ReceiveDataDelegate?

    private override init() {
        super.init()

        _ = try? core.start(with: self.configuration)
        core.delegate = self
    }

    public func connect() {
        _ = try? core.connect(withProperties: self.properties)
    }

    public func disconnect() {
        _ = try? core.disconnect()
        for connection in core.connections {
            _ = try? connection.close()
        }
    }

    public func reconnect() {
        self.disconnect()
        self.connect()
    }

    public func pair(otp: String) throws {
        try core.pairRemote(withOneTimePasscode: otp)
    }

    public func revokePair(peerId: String) throws {
        try core.revokePairing(withPeerId: peerId)
    }

    public func enablePairing() throws {
        self.disablePairing()
        try core.enablePairingMode(forSeconds: 120)
    }

    public func disablePairing() {
        core.disablePairingMode()
    }

    public func connectTo(peerId: String) throws -> MDGPeerConnection {
        if let connectionIndex = self.core.connections.index(where: { $0.peerId == peerId }) {
            return self.core.connections[connectionIndex]
        } else {
            return try self.core.placeCallRemote(withPeerId: peerId, protocolName: "chat-client", timeout: 10)
        }
    }

    public func format(otp: String) -> String {
        var formattedOtp = ""
        let allowedCharacters = NSCharacterSet(charactersIn: "0123456789").inverted
        var strippedOtp = otp.components(separatedBy: allowedCharacters).joined(separator: "")

        if strippedOtp.characters.count > otpLength {
            let index = strippedOtp.index(strippedOtp.startIndex, offsetBy: otpLength)
            strippedOtp = strippedOtp.substring(to: index)
        }

        for (i, number) in strippedOtp.characters.enumerated() {
            formattedOtp.append(number)
            if (i % 3 == 2) && (i < strippedOtp.characters.count - 1) {
                formattedOtp += "-"
            }
        }
        return formattedOtp
    }
}

extension MDGClient: MDGCoreDelegate {
    public func pairingStateChanged(_ state: MDGPairingState) {
        if let delegate = pairingDelegate {
            delegate.pairingChanged(state: state)
        }
    }

    public func routingStatusChanged(_ connection: MDGPeerConnection, to status: MDGRoutingStatus) {
        if let delegate = connectionDelegate {
            delegate.routingChanged(status: status, connection: connection)
        }
    }

    public func connection(_ connection: MDGPeerConnection, didReceive data: Data) {
        self.messageStorage.add(data: data, forConnection: connection, sender: .Them)
        if let delegate = receiveDataDelegate {
            delegate.didReceive(data: data, connection: connection)
        }
    }
}

extension MDGPairingStatus {
    var stringValue: String {
        let value: String
        switch self {
        case .completed:
            value = "Pairing completed"
        case .failedGeneric:
            value = "Pairing failed generic"
        case .failedOneTimePasscode:
            value = "One time passcode failed"
        case .oneTimePasscodeReady:
            value = "One time passcode is ready"
        case .starting:
            value = "Pairing starting"
        case .unknown:
            value = "Pairing status unknown"
        }
        return value
    }
}
