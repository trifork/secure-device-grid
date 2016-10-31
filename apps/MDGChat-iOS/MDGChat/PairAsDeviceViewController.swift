//
//  PairAsDeviceViewController.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit
import LogView
import MDG

class PairAsDeviceViewController: UIViewController {
    @IBOutlet weak var otpLabel: UILabel!
    @IBOutlet weak var peerNameTextField: UITextField!

    let client = MDGClient.sharedClient
    let peers = MDGPeers.sharedPeers

    override func viewDidLoad() {
        super.viewDidLoad()

        client.pairingDelegate = self
    }

    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)

        client.disablePairing()
    }

    func openForPairing() {
        do {
            try client.enablePairing()
        } catch {
            LogView.sharedLogView.add(line: "Start pairing failed")
            otpLabel.text = "..."
        }
        self.peerNameTextField.resignFirstResponder()
    }
}

extension PairAsDeviceViewController : UITextFieldDelegate {
    @IBAction func doneButtonClicked(_ sender: Any) {
        openForPairing()
    }

    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        if textField == self.peerNameTextField {
            openForPairing()
            textField.resignFirstResponder()
        }
        return true
    }
}

extension PairAsDeviceViewController: PairingDelegate {
    func pairingChanged(state: MDGPairingState) {
        if state.status == .completed {
            peers.setPeer(name: peerNameTextField.text, forPeerId: state.peerId)
        }

        DispatchQueue.main.async { [weak self] in
            LogView.sharedLogView.add(line: state.status.stringValue)
            if state.status == .oneTimePasscodeReady {
                self?.otpLabel.text = self?.client.format(otp: state.oneTimePasscode)
            }
        }
    }
}
