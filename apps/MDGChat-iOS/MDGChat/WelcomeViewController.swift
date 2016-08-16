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

        if let savedEmail = NSUserDefaults.standardUserDefaults().stringForKey(userEmailKey) {
            email = savedEmail
            self.emailTextField.text = savedEmail
        }
    }

    override func viewWillDisappear(animated: Bool) {
        let newEmail = self.emailTextField.text
        if newEmail != email {
            NSUserDefaults.standardUserDefaults().setObject(newEmail, forKey: userEmailKey)
            NSUserDefaults.standardUserDefaults().synchronize()
            MDGClient.sharedClient.reconnect()
        }
    }

    func textFieldShouldReturn(textField: UITextField) -> Bool {
        self.navigationController?.popViewControllerAnimated(true)
        return false
    }
}
