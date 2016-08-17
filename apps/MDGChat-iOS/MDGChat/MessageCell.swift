//
//  MessageCell.swift
//  MDGChat
//
//  Created by Bjarke Hesthaven Søndergaard on 21/09/15.
//  Copyright © 2015 Trifork A/S. All rights reserved.
//

import UIKit

let triforkOrange = UIColor(red: 253/255, green: 120/255, blue: 36/250, alpha: 1) // #FD7824

class MessageCell: UITableViewCell {
    var messageLabel: UILabel = UILabel()
    let textColorThem = UIColor.darkTextColor()
    let textColorMe = triforkOrange
    let closestEdge: CGFloat = 5

    var leadingMessageLabelConstraintThem: NSLayoutConstraint!
    var leadingMessageLabelConstraintMe: NSLayoutConstraint!
    var trailingMessageLabelConstraintThem: NSLayoutConstraint!
    var trailingMessageLabelConstraintMe: NSLayoutConstraint!

    override init(style: UITableViewCellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        self.configureView()
    }

    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }

    func configureView() {
        self.selectionStyle = .None
        self.backgroundColor = UIColor.whiteColor()
        self.contentView.addSubview(self.messageLabel)
        self.messageLabel.translatesAutoresizingMaskIntoConstraints = false

        leadingMessageLabelConstraintThem = NSLayoutConstraint(item: messageLabel, attribute: .Leading, relatedBy: .Equal, toItem: contentView, attribute: .LeadingMargin, multiplier: 1, constant: closestEdge)
        leadingMessageLabelConstraintMe = NSLayoutConstraint(item: messageLabel, attribute: .Leading, relatedBy: .GreaterThanOrEqual, toItem: contentView, attribute: .LeadingMargin, multiplier: 1, constant: 0)
        trailingMessageLabelConstraintThem = NSLayoutConstraint(item: contentView, attribute: .TrailingMargin, relatedBy: .GreaterThanOrEqual, toItem: messageLabel, attribute: .Trailing, multiplier: 1, constant: 0)
        trailingMessageLabelConstraintMe = NSLayoutConstraint(item: contentView, attribute: .TrailingMargin, relatedBy: .Equal, toItem: messageLabel, attribute: .Trailing, multiplier: 1, constant: closestEdge)
    }

    var message: Message? {
        didSet {
            messageLabel.text = message?.text
            setSender(message?.sender)
        }
    }

    override func prepareForReuse() {
        super.prepareForReuse()

        messageLabel.text = ""
        message = nil
    }

    func setSender(sender: MessageSender?) {
        let isMe = sender == .Me
        leadingMessageLabelConstraintMe.active = isMe
        trailingMessageLabelConstraintMe.active = isMe
        leadingMessageLabelConstraintThem.active = !isMe
        trailingMessageLabelConstraintThem.active = !isMe
        messageLabel.textColor = isMe ? textColorMe : textColorThem
        messageLabel.sizeToFit()
    }
}
