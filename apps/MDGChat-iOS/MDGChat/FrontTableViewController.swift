//
//  FrontTableViewController.swift
//  MDGChat
//
//  Created by Martin Rodalgaard on 16/08/16.
//  Copyright © 2016 Trifork A/S. All rights reserved.
//

import UIKit

class FrontTableViewController: UITableViewController {
    let supportUrl = "http://securedevicegrid.com/"

    override func viewDidLoad() {
        super.viewDidLoad()
    }

    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        if indexPath.row == 4 {
            if let url = URL(string: supportUrl) {
                UIApplication.shared.openURL(url)
            }
            self.tableView.deselectRow(at: indexPath, animated: false)
        }
    }
}
