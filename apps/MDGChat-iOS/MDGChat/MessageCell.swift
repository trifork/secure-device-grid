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
    let textColorThem = UIColor.darkText
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
        self.selectionStyle = .none
        self.backgroundColor = UIColor.white
        self.contentView.addSubview(self.messageLabel)
        self.messageLabel.translatesAutoresizingMaskIntoConstraints = false

        leadingMessageLabelConstraintThem = NSLayoutConstraint(item: messageLabel, attribute: .leading, relatedBy: .equal, toItem: contentView, attribute: .leadingMargin, multiplier: 1, constant: closestEdge)
        leadingMessageLabelConstraintMe = NSLayoutConstraint(item: messageLabel, attribute: .leading, relatedBy: .greaterThanOrEqual, toItem: contentView, attribute: .leadingMargin, multiplier: 1, constant: 0)
        trailingMessageLabelConstraintThem = NSLayoutConstraint(item: contentView, attribute: .trailingMargin, relatedBy: .greaterThanOrEqual, toItem: messageLabel, attribute: .trailing, multiplier: 1, constant: 0)
        trailingMessageLabelConstraintMe = NSLayoutConstraint(item: contentView, attribute: .trailingMargin, relatedBy: .equal, toItem: messageLabel, attribute: .trailing, multiplier: 1, constant: closestEdge)
    }

    var message: Message? {
        didSet {
            messageLabel.text = message?.text
            set(sender: message?.sender)
        }
    }

    override func prepareForReuse() {
        super.prepareForReuse()

        messageLabel.text = ""
        message = nil
    }

    func set(sender: MessageSender?) {
        let isMe = sender == .Me
        leadingMessageLabelConstraintMe.isActive = isMe
        trailingMessageLabelConstraintMe.isActive = isMe
        leadingMessageLabelConstraintThem.isActive = !isMe
        trailingMessageLabelConstraintThem.isActive = !isMe

        messageLabel.font = messageLabel.font.withSize(sender == .Info ? 12 : 16)
        messageLabel.numberOfLines = 2
        messageLabel.textColor = isMe ? textColorMe : textColorThem
        messageLabel.sizeToFit()
    }
}
