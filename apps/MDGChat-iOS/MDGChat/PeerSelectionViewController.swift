//
//  PeerSelectionTableViewController.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit
import MDG

class PeerSelectionViewController: UIViewController {
    @IBOutlet weak var tableView: UITableView!

    let client = MDGClient.sharedClient
    let peers = MDGPeers.sharedPeers
    let logView = LogView()

    var waitingConnection: MDGPeerConnection?

    override func viewDidLoad() {
        super.viewDidLoad()

        self.tableView.dataSource = self
        self.tableView.delegate = self
        self.view.addSubview(self.logView)
    }

    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)

        client.connectionDelegate = self
        client.receiveDataDelegate = self
        self.logView.update()
        self.tableView.reloadData()
    }
}

extension PeerSelectionViewController: UITableViewDataSource {
    func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }

    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return client.pairings.count
    }

    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "PeerCell", for: indexPath)
        cell.textLabel?.text = peers.peerName(forPeerId: client.pairings[indexPath.row])

        let peerId = client.pairings[indexPath.row]
        let unreadMessages = self.client.messageStorage.messages.filter { $0.peerId == peerId && $0.isRead == false }
        cell.accessoryView = unreadMessages.isEmpty ? nil : createBadgeAccessoryView(count: unreadMessages.count)

        return cell
    }

    func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt indexPath: IndexPath) {
        if editingStyle == .delete {
            let peerId = client.pairings[indexPath.row]
            do {
                try client.revokePair(peerId: peerId)
            } catch {
                let peerName = peers.peerName(forPeerId: peerId)
                logView.add(line: "Failed to remove \(peerName)")
            }
            self.tableView.reloadData()
        }
    }

    func createBadgeAccessoryView(count: Int) -> UILabel {
        let badge = UILabel(frame: CGRect(x: 0, y: 0, width: 20, height: 20))
        badge.text = String(count)
        badge.backgroundColor = triforkOrange
        badge.textColor = UIColor.white
        badge.adjustsFontSizeToFitWidth = true
        badge.textAlignment = .center
        badge.layer.cornerRadius = 4
        badge.clipsToBounds = true
        return badge
    }
}

extension PeerSelectionViewController: UITableViewDelegate {
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        self.waitingConnection = nil
        let peerId = client.pairings[indexPath.row]
        let peerName = peers.peerName(forPeerId: peerId)
        logView.add(line: "Connecting to \(peerName)")

        do {
            let connection = try client.connectTo(peerId: client.pairings[indexPath.row])
            let connectedText = connection.connected ? "connected" : "not connected"

            logView.add(line: "Connection \(connection.connectionId): \(connectedText)")
            self.tableView.deselectRow(at: indexPath, animated: false)

            if connection.connected {
                self.show(connection: connection)
            } else {
                self.waitingConnection = connection
            }
        } catch let error as NSError {
            logView.add(line: "Connect failed: \(error)")
        }
    }
}

extension PeerSelectionViewController {
    func show(connection: MDGPeerConnection) {
        self.performSegue(withIdentifier: "connection.start", sender: connection)
    }

    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if let chatVC = segue.destination as? ChatWithPeerViewController,
            let connection = sender as? MDGPeerConnection {
            chatVC.connection = connection
            chatVC.messageStorage = self.client.messageStorage
        }
    }
}

extension PeerSelectionViewController: ConnectionDelegate {
    func routingChanged(status: MDGRoutingStatus, connection: MDGPeerConnection) {
        let connId = connection.connectionId

        DispatchQueue.main.async { [weak self, weak connection] in
            switch status {
            case .connected:
                self?.logView.add(line: "Connected to peer on conn_id \(connId)")
                if let waitingConnection = self?.waitingConnection,
                    let connection = connection, waitingConnection.peerId == connection.peerId {
                    self?.show(connection: waitingConnection)
                }
            case .peerNotAvailable:
                self?.logView.add(line: "Peer not available on conn_id \(connId)")
            case .failed:
                self?.logView.add(line: "Connect failed on conn_id \(connId)")
            case .disconnected:
                self?.logView.add(line: "Connect closed on conn_id \(connId)")
            case .unknown:
                self?.logView.add(line: "Unknown error occurred on conn_id \(connId)")
            }
        }
    }
}

extension PeerSelectionViewController: ReceiveDataDelegate {
    func didReceive(data: Data, connection: MDGPeerConnection) {
        DispatchQueue.main.async { [weak self, weak connection] in
            if let connectionId = connection?.connectionId {
                self?.logView.add(line: "Received data from conn_id \(connectionId)")
                self?.tableView.reloadData()
            }
        }
    }
}
