# Trifork Secure Device Grid

[A secure device-to-device communications solution for IOT!](http://securedevicegrid.com)

## Demo clients for GOTO Copenhagen 2015

These clients are for handout at the Trifork booth at the conference.

*   [Linux 64 bit (Build on Ubuntu 15.04)](./binaries/sandbox/ubuntu/sandbox-demo)
*   [OSX for Mac](./binaries/sandbox/macosx/sandbox-demo)
*   [Windows](./binaries/sandbox/windows/sandbox.zip)
*   [Android 4+](./binaries/sandbox/android/sandbox-demo.apk)
*   [iOS 8+ (source code)](./binaries/sandbox/ios/sandbox-demo.zip)

The "desktop" clients are ready-to-run executables. They store their
local state in the filesystem in the directory where they are
launched. You can run multiple copies on one machine, provided you
keep thoses directories distinct.

The above Android/iOS applications need installing on your phone. If
you are a developer for the smartphone platform, you know how to do
this.

## Embedded device examples and client library binaries.

These examples are for embedded boards. We don't provide complete
binaries for these development boards, as you have to compile for the
actual board you have. Instead we provide example programs
similar to the above binaries, but in source code as demo apps. The
example app includes a binary version of the communications library
for the platform.

*   [Broadcom WICED on NetX Duo (Using WICED-SDK v3.1.2)](./binaries/release_wiced.zip)
*   [TI CC3200 Simplelink SDK (Using CC3200SDK v1.1.0)](./binaries/release_cc3200.zip)

## Licensing

You may use these examples and demos for evaluation purposes only.
Using Trifork Secure Device Grid in your product requires a
license. [Please contact us.](http://trifork.com)
