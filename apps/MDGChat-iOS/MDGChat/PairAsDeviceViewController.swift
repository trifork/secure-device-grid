//
//  PairAsDeviceViewController.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit
import MDG

class PairAsDeviceViewController: UIViewController {
    @IBOutlet weak var otpLabel: UILabel!
    @IBOutlet weak var peerNameTextField: UITextField!

    let client = MDGClient.sharedClient
    let logView = LogView()

    override func viewDidLoad() {
        super.viewDidLoad()

        client.pairingDelegate = self
        self.view.addSubview(logView)
    }

    override func viewDidDisappear(animated: Bool) {
        super.viewDidDisappear(animated)

        client.disablePairing()
    }

    func openForPairing() {
        do {
            try client.enablePairing()
        } catch {
            logView.addLine("Start pairing failed")
            otpLabel.text = "..."
        }
        self.peerNameTextField.resignFirstResponder()
    }
}

extension PairAsDeviceViewController : UITextFieldDelegate {
    @IBAction func doneButtonClicked(sender: AnyObject) {
        openForPairing()
    }

    func textFieldShouldReturn(textField: UITextField) -> Bool {
        if textField == self.peerNameTextField {
            openForPairing()
            textField.resignFirstResponder()
        }
        return true
    }
}

extension PairAsDeviceViewController: PairingDelegate {
    func pairingStateChanged(state: MDGPairingState) {
        if state.status == .Completed {
            client.setPeerName(peerNameTextField.text, forPeerId: state.peerId)
        }

        dispatch_async(dispatch_get_main_queue()) {
            self.logView.addLine(state.status.stringValue)
            if state.status == .OneTimePasscodeReady {
                self.otpLabel.text = self.client.formatOtp(state.oneTimePasscode)
            }
        }
    }
}
