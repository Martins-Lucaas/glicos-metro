import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';

class DataAcquisitionState with ChangeNotifier {
  FlutterBlue flutterBlue = FlutterBlue.instance;
  BluetoothDevice? _device;
  BluetoothCharacteristic? _characteristic;
  List<double> dataPoints = [];
  double currentValue = 0.0;
  int acquisitionRate = 500;
  bool isAcquiring = false;
  bool isConnected = false;

  Future<void> connectToDevice() async {
    if (!isConnected) {
      try {
        List<BluetoothDevice> devices = await flutterBlue.connectedDevices;
        if (devices.isNotEmpty) {
          _device = devices.firstWhere(
            (device) => device.name == 'ESP32_glucometer',
            orElse: () => devices.first,
          );
        } else {
          // Scan for devices
          var scanSubscription = flutterBlue.scan().listen((scanResult) async {
            if (scanResult.device.name == 'ESP32_glucometer') {
              _device = scanResult.device;
              await _device?.connect();
              flutterBlue.stopScan();
            }
          });
          await Future.delayed(const Duration(seconds: 5));
          scanSubscription.cancel();
        }
        if (_device != null) {
          var services = await _device!.discoverServices();
          for (var service in services) {
            var characteristics = service.characteristics;
            for (var c in characteristics) {
              if (c.properties.notify) {
                _characteristic = c;
                await _characteristic!.setNotifyValue(true);
                _characteristic!.value.listen((value) {
                  currentValue = _parseData(value);
                  if (dataPoints.length >= 100) {
                    dataPoints.removeAt(0);
                  }
                  dataPoints.add(currentValue);
                  notifyListeners();
                });
                isConnected = true;
                notifyListeners();
                break;
              }
            }
          }
        }
      } catch (e) {
        debugPrint("Error connecting to device: $e");
      }
    }
  }

  void startAcquisition() {
    if (!isAcquiring && _characteristic != null) {
      isAcquiring = true;
      _sendCommand("START");
      notifyListeners();
      debugPrint("Starting data acquisition...");
    }
  }

  void stopAcquisition() {
    if (isAcquiring) {
      isAcquiring = false;
      _sendCommand("STOP");
      debugPrint("Stopping data acquisition...");
      notifyListeners();
    }
  }

  void updateAcquisitionRate(int rate) {
    acquisitionRate = rate;
    _sendCommand("RATE $rate");
    debugPrint("Updating acquisition rate to $rate ms");
    notifyListeners();
  }

  double _parseData(List<int> data) {
    // Implement your data parsing logic here
    return data.isNotEmpty ? data[0].toDouble() : 0.0;
  }

  void _sendCommand(String command) {
    if (_characteristic != null) {
      _characteristic!.write(utf8.encode('$command\n'));
    }
  }
}
