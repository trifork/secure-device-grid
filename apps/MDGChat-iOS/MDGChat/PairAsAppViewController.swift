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
    let logView = LogView()

    override func viewDidLoad() {
        super.viewDidLoad()

        client.pairingDelegate = self
        self.otpTextField.addTarget(self, action: #selector(PairAsAppViewController.otpTextFieldChanged(_:)), forControlEvents: UIControlEvents.EditingChanged)

        self.view.addSubview(logView)
    }

    func pair() {
        do {
            try client.pair(self.otpTextField.text ?? "")
        } catch {
            logView.addLine("Pairing failed")
        }
        self.otpTextField.text = ""
    }

    func otpTextFieldChanged(textField: UITextField) {
        if let text = textField.text {
            textField.text = client.formatOtp(text)
        }
    }
}

extension PairAsAppViewController : UITextFieldDelegate {
    @IBAction func doneButtonClicked(sender: AnyObject) {
        pair()
    }

    func textFieldShouldReturn(textField: UITextField) -> Bool {
        if textField == self.peerNameTextField {
            pair()
            textField.resignFirstResponder()
        }
        return true
    }
}

extension PairAsAppViewController: PairingDelegate {
    func pairingStateChanged(state: MDGPairingState) {
        if state.status == .Completed {
            client.setPeerName(peerNameTextField.text, forPeerId: state.peerId)
        }

        dispatch_async(dispatch_get_main_queue()) {
            self.logView.addLine(state.status.stringValue)
        }
    }
}
