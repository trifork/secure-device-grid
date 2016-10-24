//
//  WelcomeViewController.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit

public let userEmailKey = "UserEmailKey"

class WelcomeViewController: UIViewController, UITextFieldDelegate {
    @IBOutlet weak var emailTextField: UITextField!
    var email: String = ""

    override func viewDidLoad() {
        super.viewDidLoad()

        if let savedEmail = UserDefaults.standard.string(forKey: userEmailKey) {
            email = savedEmail
            self.emailTextField.text = savedEmail
        }
    }

    override func viewWillDisappear(_ animated: Bool) {
        let newEmail = self.emailTextField.text
        if newEmail != email {
            UserDefaults.standard.set(newEmail, forKey: userEmailKey)
            UserDefaults.standard.synchronize()
            MDGClient.sharedClient.reconnect()
        }
    }

    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        _ = self.navigationController?.popViewController(animated: true)
        return false
    }
}
