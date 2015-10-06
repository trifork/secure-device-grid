# Trifork Secure Device Grid

[A secure device-to-device communications solution for IoT!](http://securedevicegrid.com)

## Demo clients for GOTO Copenhagen 2015

These clients are for handout at the Trifork booth at the conference.

*   [Linux 64 bit (Build on Ubuntu 15.04)](./binaries/sandbox/ubuntu/sandbox-demo)
*   [OSX for Mac](./binaries/sandbox/macosx/Sandbox-demo)
*   [Windows](./binaries/sandbox/windows/win32-sandbox.zip)
*   [Android 4+](./binaries/sandbox/android/sandbox-demo.apk)
*   [iOS 8+ (source code)](./binaries/release_ios.zip)

The "desktop" clients are ready-to-run executables. They store their
local state in the filesystem in the directory where they are
launched. You can run multiple copies on one machine, provided you
keep thoses directories distinct.

The above Android/iOS applications need installing on your phone. If
you are a developer for the smartphone platform, you know how to do
this.

On Windows, you need to have [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145) installled. If not, it will complain about missing vcruntime140.dll

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
