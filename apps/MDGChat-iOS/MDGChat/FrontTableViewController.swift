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

    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        if indexPath.row == 4 {
            if let url = NSURL(string: supportUrl) {
                UIApplication.sharedApplication().openURL(url)
            }
            self.tableView.deselectRowAtIndexPath(indexPath, animated: false)
        }
    }
}
