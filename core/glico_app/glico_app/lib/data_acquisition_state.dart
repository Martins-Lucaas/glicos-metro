import 'dart:async';
import 'dart:math';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

enum MathFunction { sum, subtract, multiply, divide, fft }

class DataAcquisitionState with ChangeNotifier {
  final String esp32Ip;
  List<double> dataPoints = [];
  double currentValue = 0.0;
  int acquisitionRate = 50;  // Taxa de aquisição ajustada para 50ms
  bool isAcquiring = false;
  Timer? _timer;

  List<double> fftData = [];
  double frequency = 0.0;
  
  int timeDivisions = 100; // Escala de tempo padrão em ms/div

  DataAcquisitionState(this.esp32Ip);

  get frequencies => null;

  void startAcquisition() async {
    if (!isAcquiring) {
      isAcquiring = true;
      notifyListeners();
      debugPrint("Starting data acquisition...");
      try {
        final response = await http.get(Uri.parse('http://$esp32Ip/startAcquisition'));
        if (response.statusCode == 200) {
          debugPrint("Acquisition started successfully.");
          _scheduleNextFetch();
        } else {
          debugPrint("Failed to start acquisition: ${response.statusCode}");
        }
      } catch (e) {
        debugPrint("Error starting acquisition: $e");
      }
    }
  }

  void stopAcquisition() async {
    if (isAcquiring) {
      isAcquiring = false;
      _timer?.cancel();
      debugPrint("Stopping data acquisition...");
      try {
        final response = await http.get(Uri.parse('http://$esp32Ip/stopAcquisition'));
        if (response.statusCode == 200) {
          debugPrint("Acquisition stopped successfully.");
        } else {
          debugPrint("Failed to stop acquisition: ${response.statusCode}");
        }
      } catch (e) {
        debugPrint("Error stopping acquisition: $e");
      }
      notifyListeners();
    }
  }

  void toggleAcquisition() {
    if (isAcquiring) {
      stopAcquisition();
    } else {
      startAcquisition();
    }
  }

  void _scheduleNextFetch() {
    if (isAcquiring) {
      _timer = Timer(Duration(milliseconds: acquisitionRate), () async {
        await _fetchData();
        _scheduleNextFetch();
      });
    }
  }

  Future<void> _fetchData() async {
    try {
      final response = await http.get(Uri.parse('http://$esp32Ip/vADCvalue'));
      if (response.statusCode == 200) {
        currentValue = double.parse(response.body);
        if (dataPoints.length >= timeDivisions) { 
          dataPoints.removeAt(0);  // Limite de pontos baseado na escala de tempo
        }
        dataPoints.add(currentValue);
        notifyListeners();
      } else {
        debugPrint("Falha ao adquirir os dados. Status code: ${response.statusCode}");
      }
    } catch (e) {
      debugPrint("Erro: $e");
    }
  }

  void measureFrequencyPeriod() {
    if (dataPoints.length < 2) return;

    // Detect zero crossings para estimar o período
    List<int> zeroCrossings = [];
    for (int i = 1; i < dataPoints.length; i++) {
      if (dataPoints[i - 1] < 0 && dataPoints[i] >= 0) {
        zeroCrossings.add(i);
      }
    }

    if (zeroCrossings.length < 2) return;

    // Calcula o período estimado
    double totalPeriod = 0.0;
    for (int i = 1; i < zeroCrossings.length; i++) {
      totalPeriod += (zeroCrossings[i] - zeroCrossings[i - 1]);
    }
    double averagePeriod = totalPeriod / (zeroCrossings.length - 1);

    double periodMs = averagePeriod * acquisitionRate;
    double frequencyHz = 1000 / periodMs;

    debugPrint("Average Period: $periodMs ms");
    debugPrint("Frequency: $frequencyHz Hz");

    frequency = frequencyHz;
    notifyListeners();
  }

  void applyMathFunction(MathFunction function) {
    switch (function) {
      case MathFunction.fft:
        _performFFT();
        break;
      // Outros casos não implementados para este exemplo
      default:
        break;
    }
    notifyListeners();
  }

  void _performFFT() {
    int n = dataPoints.length;
    List<Complex> input = List.generate(n, (i) => Complex(dataPoints[i], 0));
    List<Complex> output = fft(input);
    fftData = output.map((c) => c.magnitude).toList();
  }

  // FFT
  List<Complex> fft(List<Complex> input) {
    int n = input.length;
    if (n <= 1) return input;

    List<Complex> even = fft(List.generate(n ~/ 2, (i) => input[i * 2]));
    List<Complex> odd = fft(List.generate(n ~/ 2, (i) => input[i * 2 + 1]));

    List<Complex> output = List.filled(n, Complex(0, 0));
    for (int k = 0; k < n ~/ 2; k++) {
      Complex t = odd[k] * Complex.polar(1, -2 * pi * k / n);
      output[k] = even[k] + t;
      output[k + n ~/ 2] = even[k] - t;
    }
    return output;
  }

  // Controla blink rate
  void setBlinkRate(int rate) async {
    try {
      final response = await http.get(Uri.parse('http://$esp32Ip/setBlinkRate?rate=$rate'));
      if (response.statusCode == 200) {
        debugPrint("Taxa alterada para $rate ms.");
      } else {
        debugPrint("Falha ao alterar a taxa: ${response.statusCode}");
      }
    } catch (e) {
      debugPrint("Erro: $e");
    }
  }

  // Define a escala de tempo
  void setTimeDivisions(int divisions) {
    timeDivisions = divisions;
    notifyListeners();
  }
}

// Classe para operações de números complexos
class Complex {
  final double real;
  final double imaginary;

  Complex(this.real, this.imaginary);

  Complex operator +(Complex other) => Complex(real + other.real, imaginary + other.imaginary);
  Complex operator -(Complex other) => Complex(real - other.real, imaginary - other.imaginary);
  Complex operator *(Complex other) => Complex(real * other.real - imaginary * other.imaginary, real * other.imaginary + imaginary * other.real);

  double get magnitude => sqrt(real * real + imaginary * imaginary);

  factory Complex.polar(double r, double theta) => Complex(r * cos(theta), r * sin(theta));
}
