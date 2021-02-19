![BluepillIcon](doc/img/bluepill_text.png)

![](https://github.com/linkedin/bluepill/workflows/master/badge.svg)

# Background

## iOS Simulator Architecture

The iOS Simulator for macOS is part of Xcode. It is located at `/Applications/Xcode.app/Contents/Developer/Applications/Simulator.app`, which if you inspect using `otool -L`, you'll see is linked with `CoreSimulator.framework`.

There are three ways to use Simulators in macOS:

1. Open the `Simulator.app` GUI application (either via Xcode or standalone).
2. Through a command line interface: `xcrun simctl`.
3. By linking directly to the `CoreSimulator.framework` and using the API.

Bluepill uses the third approach. A handy way of looking at the methods in `CoreSimulator.framework` is by running `nm` on the binary:

```
❯ nm /Library/Developer/PrivateFrameworks/CoreSimulator.framework/Versions/A/CoreSimulator | head
000000000000f94d t +[NSArray(SimPasteboardItem) blacklistedTypes]
000000000007de0b t +[SimAudioHostRouteDescriptorState defaultAudioDescriptorStateWithPowerState:guestDataPath:]
000000000004b872 t +[SimDevice _axResponseFromXPCMessage:]
000000000004b6b4 t +[SimDevice _xpcMessageWithAXRequest:]
0000000000029524 t +[SimDevice createDeviceWithName:deviceSet:deviceType:runtime:initialDataPath:]
000000000002a6e9 t +[SimDevice isValidState:]
000000000002a513 t +[SimDevice simDevice:UDID:deviceTypeIdentifier:runtimeIdentifier:runtimePolicy:runtimeSpecifier:state:deviceSet:]
0000000000029b2d t +[SimDevice simDeviceAtPath:deviceSet:]
000000000002d15f t +[SimDevice supportsFeature:deviceType:runtime:]
000000000006ab8a t +[SimDeviceBootInfo supportsSecureCoding]
```

The iOS Simulator as a user process in macOS (that is, it uses the same macOS kernel). This means you can inspect the processes running inside the simulator by running common utilities like `ps` (e.g. look for `launchd_sim` to find LaunchPad running inside the Simulator).

## Testing iOS Applications

There are two types of tests you can write for iOS applications:

- UI Tests
	- Tests your app's UI
	- Uses accessibility API of iOS
	- Stuff like asserts about things being on screen and being visible
- Unit Tests
	- Non UI assertions

Both of these tests use Apple's `XCTest.framework`, but they are run in different ways. When you run Unit Tests, Xcode uses code injection via the dynamic loader (See `man dyld`) to set `DYLD_INSERT_LIBRARIES` and load an injection dylib (typically created by Xcode when building an app in debug mode and placed inside the app's bundle at `libXCTestBundleInject.dylib`, and if your application has Swift code you'll also see `libXCTestSwiftSupport.dylib`).

Your tests are compiled as Plugins to your App; that is, in the `Plugins` directory inside your app's bundle, you'll find all the `*.xctest` bundles for all of your tests. The XCTest injection dylibs responsibility is to prevent your application from running its own `main()` and instead load all the `*.xctest` bundles from the `Plugins` directory and execute the tests.

When you run UI Tests, a slightly different mechanism is used. Xcode creates a _runner_ application typically named the same as your app but with `UITests-Runner` appended. For example, for `BPSampleApp.app` Xcode will create `BPSampleAppUITests-Runner.app`. This runner app's responsibility is to launch your app and observe it using the Accessibility Framework for UI Testing.

Xcode encodes all the information on how to run tests in an `.xctestrun` file, a PLIST format file (typically XML) with information about how to run the tests. If you build Bluepill's sample app, you can see the information in the generated `BPSampleApp_iphonesimulator14.4-x86_64.xctestrun` by running `plutil -p` on it:

```
❯ plutil -p ./build/Build/Products/BPSampleApp_iphonesimulator14.4-x86_64.xctestrun | head
{
  "__xctestrun_metadata__" => {
    "FormatVersion" => 1
  }
  "BPAppNegativeTests" => {
    "BlueprintName" => "BPAppNegativeTests"
    "BundleIdentifiersForCrashReportEmphasis" => [
      0 => "LI.BPSampleAppHangingTests"
      1 => "LI.BPAppNegativeTests"
      2 => "LI.BPSampleAppFatalErrorTests"
```

## Building Bluepill

### Using Xcode

- Open `Bluepill.xcworkspace` and you should see three projects:
  - `BPSampleApp`, which is an iOS application that demonstrates many of the common problems found while testing iOS applications: flaky tests, tests that crash, tests that timeout, etc. It is used for testing Bluepill itself.
  - `bluepill` corresponds to the `bluepill` binary (a.k.a. `BLUEPILL_BINARY`) that is responsible for test packing, final reporting and keeping track of the many `bp` processes that are tied each to a single simulator run.
  - `bp` which corresponds to the `bp` binary which is the main driver for testing in a single Simulator.

### Using the Terminal

- Run [`scripts/bluepill.sh build`](scripts/bluepill.sh) to build Bluepill (is what we use in CI). It'll leave the built artifacts in the `build` directory.

You can take a look at [`.github/workflows/master.yml`](.github/workflows/master.yml) to see how we call it in CI.


## How Bluepill runs the tests

Bluepill mimics the way Xcode launches tests by calling `CoreSimulator.framework` and `XCTest.framework` APIs and setting up environment variables to the dynamic loader in the same way as Xcode.

## Bluepill High Level Architecture
![](doc/img/architecture.png)

- `bluepill` (aka `BLUEPILL_BINARY`) is a command line application
	- [`bluepill/main.m`](bluepill/main.m) is the entry point
	- Starts with the most important task of a Matrix-style opening sequence (try `bluepill --matrix` and see)
	- [`BPConfiguration.m`](bp/src/BPConfiguration.m) has all the args and config file processor which can parse command line arguments or a JSON config file. The same logic is shared by `BLUEPILL_BINARY` and `BP_BINARY`.
	- `bluepill` will create many `bp` (aka `BP_BINARY`) processes to run simulators
	- Monitor the `bp` processes for failure
	- Aggregates individual test results into a unified test results
- `bp` (aka `BP_BINARY`) is designed to run one `.xctest` bundle using one simulator. It will automatically delete and create new simulators if the tests crash or fail to provide a clean slate for the tests to run. However, it will only have at any time a single simulator running. The parallelism of Bluepill is provided by the multiple `bp` processes that `bluepill` spawns.
	- [`bp/src/main.m`](bp/src/main.m) is the entry point.
- Bluepill is fully self contained and needs no external dependencies

## Running Bluepill
- `bluepill --config bp/tests/Resources ... config.json`
![](doc/img/config.png)
- [`BPRunner.m`](bluepill/src/BPRunner.m)
	- Keeps track of how many `BP_BINARY` processes at a given time?
	- Packing algorithm gets all XCTestBundles in Plugins directory to determine the number of parallel simulators
	- X Tests : Y Simulators (What are some packing strategies?)
	- Perhaps Bluepill can use previous run's stats to determine the packing algorithm and weight by Bundle size if no prior knowledge exists. The key is to parallelize equally.
- `BLUEPILL_BINARY` aggregates the test results
![](doc/img/test_result.png)
- `BLUEPILL_BINARY` kepps CPU usage/memory details of all `BP_BINARY` processes
	- Similar to a Chrome Trace/Log
![](doc/img/lifespan.png)
- When a single `BP_BINARY` process crashes, a new one is spawned to resume testing. Since results are aggregated at the end, killing and respawning `BP_BINARY` processes are somewhat cheap

## Summary
- `BLUEPILL_BINARY` is responsible for:
	- Packing algorithm
	- Planning the test run
	- Monitoring CPU/Memory usage
	- Aggregating test results

- `BP_BINARY` is spawned by `BLUEPILL_BINARY`
	- It runs a single simulator and runs one or more test bundles




