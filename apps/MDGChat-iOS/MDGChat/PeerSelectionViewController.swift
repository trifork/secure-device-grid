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

    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)

        client.connectionDelegate = self
        client.receiveDataDelegate = self
        self.logView.update()
        self.tableView.reloadData()
    }
}

extension PeerSelectionViewController: UITableViewDataSource {
    func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }

    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return client.pairings.count
    }

    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("PeerCell", forIndexPath: indexPath)
        cell.textLabel?.text = peers.peerName(client.pairings[indexPath.row])

        let peerId = client.pairings[indexPath.row]
        let unreadMessages = self.client.messageStorage.messages.filter { $0.peerId == peerId && $0.isRead == false }
        cell.accessoryView = unreadMessages.isEmpty ? nil : createBadgeAccessoryView(unreadMessages.count)

        return cell
    }

    func createBadgeAccessoryView(count: Int) -> UILabel {
        let badge = UILabel(frame: CGRect(x: 0, y: 0, width: 20, height: 20))
        badge.text = String(count)
        badge.backgroundColor = triforkOrange
        badge.textColor = UIColor.whiteColor()
        badge.adjustsFontSizeToFitWidth = true
        badge.textAlignment = .Center
        badge.layer.cornerRadius = 4
        badge.clipsToBounds = true
        return badge
    }
}

extension PeerSelectionViewController: UITableViewDelegate {
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        self.waitingConnection = nil
        let peerId = client.pairings[indexPath.row]
        let peerName = peers.peerName(peerId)
        logView.addLine("Connecting to \(peerName)")

        do {
            let connection = try client.connectToPeer(client.pairings[indexPath.row])
            let connectedText = connection.connected ? "connected" : "not connected"

            logView.addLine("Connection \(connection.connectionId): \(connectedText)")
            self.tableView.deselectRowAtIndexPath(indexPath, animated: false)

            if connection.connected {
                self.showConnection(connection)
            } else {
                self.waitingConnection = connection
            }
        } catch let error as NSError {
            logView.addLine("Connect failed: \(error)")
        }
    }

    func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
        if editingStyle == .Delete {
            let peerId = client.pairings[indexPath.row]
            do {
                try client.revokePair(peerId)
            } catch {
                let peerName = peers.peerName(peerId)
                logView.addLine("Failed to remove \(peerName)")
            }
            self.tableView.reloadData()
        }
    }
}

extension PeerSelectionViewController {
    func showConnection(connection: MDGPeerConnection) {
        self.performSegueWithIdentifier("connection.start", sender: connection)
    }

    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if let chatVC = segue.destinationViewController as? ChatWithPeerViewController, connection = sender as? MDGPeerConnection {
            chatVC.connection = connection
            chatVC.messageStorage = self.client.messageStorage
        }
    }
}

extension PeerSelectionViewController: ConnectionDelegate {
    func routingStatusChanged(connection: MDGPeerConnection, status: MDGRoutingStatus) {
        let connId = connection.connectionId
        dispatch_async(dispatch_get_main_queue()) { [weak self, weak connection] in
            switch status {
            case .Connected:
                self?.logView.addLine("Connected to peer on conn_id \(connId)")
                if let waitingConnection = self?.waitingConnection, connection = connection where waitingConnection.peerId == connection.peerId {
                    self?.showConnection(waitingConnection)
                }
            case .PeerNotAvailable:
                self?.logView.addLine("Peer not available on conn_id \(connId)")
            case .Failed:
                self?.logView.addLine("Connect failed on conn_id \(connId)")
            case .Disconnected:
                self?.logView.addLine("Connect closed on conn_id \(connId)")
            case .Unknown:
                self?.logView.addLine("Unknown error occurred on conn_id \(connId)")
            }
        }
    }
}

extension PeerSelectionViewController: ReceiveDataDelegate {
    func didReceiveData(connection: MDGPeerConnection, data: NSData) {
        dispatch_async(dispatch_get_main_queue()) { [weak self, weak connection] in
            if let connectionId = connection?.connectionId {
                self?.logView.addLine("Received data from conn_id \(connectionId)")
                self?.tableView.reloadData()
            }
        }
    }
}
