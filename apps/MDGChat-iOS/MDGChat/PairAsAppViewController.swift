//
//  PairAsAppViewController.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit
import MDG

class PairAsAppViewController: UIViewController {
    @IBOutlet weak var otpTextField: UITextField!
    @IBOutlet weak var peerNameTextField: UITextField!

    let client = MDGClient.sharedClient
    let peers = MDGPeers.sharedPeers
    let logView = LogView()

    override func viewDidLoad() {
        super.viewDidLoad()

        client.pairingDelegate = self
        self.otpTextField.addTarget(self, action: #selector(PairAsAppViewController.otpTextFieldChanged), for: UIControlEvents.editingChanged)

        self.view.addSubview(logView)
    }

    func pair() {
        do {
            try client.pair(otp: self.otpTextField.text ?? "")
        } catch {
            logView.add(line: "Pairing failed")
        }
        self.otpTextField.text = ""
        self.otpTextField.resignFirstResponder()
        self.peerNameTextField.resignFirstResponder()
    }

    func otpTextFieldChanged(textField: UITextField) {
        if let text = textField.text {
            textField.text = client.format(otp: text)
        }
    }
}

extension PairAsAppViewController: UITextFieldDelegate {
    @IBAction func doneButtonClicked(_ sender: Any) {
        pair()
    }

    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        if textField == self.peerNameTextField {
            pair()
            textField.resignFirstResponder()
        }
        return true
    }
}

extension PairAsAppViewController: PairingDelegate {
    func pairingChanged(state: MDGPairingState) {
        if state.status == .completed {
            peers.setPeer(name: peerNameTextField.text, forPeerId: state.peerId)
        }

        DispatchQueue.main.async {
            self.logView.add(line: state.status.stringValue)
        }
    }
}
