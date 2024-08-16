import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:fl_chart/fl_chart.dart';
import 'data_acquisition_state.dart';

class ChartWidget extends StatelessWidget {
  const ChartWidget({super.key});

  @override
  Widget build(BuildContext context) {
    final state = Provider.of<DataAcquisitionState>(context);

    return Column(
      children: [
        SizedBox(
          height: 300,
          width: double.infinity,
          child: LineChart(
            LineChartData(
              minY: -5,
              maxY: 5,
              minX: 0,
              maxX: state.timeDivisions.toDouble(), // Tamanho gráfico eixo X ajustável
              titlesData: FlTitlesData(
                leftTitles: AxisTitles(
                  sideTitles: SideTitles(
                    showTitles: true,
                    reservedSize: 40,
                    getTitlesWidget: (value, meta) {
                      return Text(
                        value.toStringAsFixed(1),
                        style: const TextStyle(color: Colors.white, fontSize: 12),
                      );
                    },
                  ),
                ),
                bottomTitles: const AxisTitles(
                  sideTitles: SideTitles(
                    showTitles: false,
                  ),
                ),
                topTitles: const AxisTitles(
                  sideTitles: SideTitles(
                    showTitles: false,
                  ),
                ),
                rightTitles: const AxisTitles(
                  sideTitles: SideTitles(
                    showTitles: false,
                  ),
                ),
              ),
              gridData: FlGridData(
                show: true,
                drawVerticalLine: true,
                getDrawingHorizontalLine: (value) {
                  return const FlLine(
                    color: Colors.white24,
                    strokeWidth: 1,
                  );
                },
                getDrawingVerticalLine: (value) {
                  return const FlLine(
                    color: Colors.white24,
                    strokeWidth: 1,
                  );
                },
              ),
              borderData: FlBorderData(
                show: true,
                border: Border.all(color: Colors.white24, width: 1),
              ),
              lineBarsData: [
                LineChartBarData(
                  spots: state.dataPoints
                      .asMap()
                      .entries
                      .map((e) => FlSpot(e.key.toDouble(), e.value))
                      .toList(),
                  color: Colors.yellow,
                  barWidth: 2,
                  dotData: const FlDotData(show: false),
                ),
              ],
            ),
          ),
        ),
        const SizedBox(height: 10),
        Text(
          'Escala de Tempo: ${state.timeDivisions} ms/div',
          style: const TextStyle(color: Colors.white, fontSize: 16),
        ),
        Slider(
          value: state.timeDivisions.toDouble(),
          min: 10,
          max: 100,
          divisions: 9,
          label: '${state.timeDivisions} ms/div',
          onChanged: (double value) {
            state.setTimeDivisions(value.toInt());
          },
        ),
        const SizedBox(height: 10),
        Row(
          mainAxisAlignment: MainAxisAlignment.spaceAround,
          children: [
            ElevatedButton(
              onPressed: () {
                state.startAcquisition();
              },
              style: ElevatedButton.styleFrom(backgroundColor: Colors.yellow),
              child: const Text('Canal 1'),
            ),
            ElevatedButton(
              onPressed: () {
                // Editar aqui a funcionalidade do canal 2
              },
              style: ElevatedButton.styleFrom(backgroundColor: Colors.blue),
              child: const Text('Canal 2'),
            ),
          ],
        ),
        const SizedBox(height: 10),
        ElevatedButton(
          onPressed: () {
            state.toggleAcquisition();
          },
          style: ElevatedButton.styleFrom(backgroundColor: Colors.red),
          child: Text(state.isAcquiring ? 'Pausar' : 'Começar'),
        ),
        const SizedBox(height: 10),
        ElevatedButton(
          onPressed: () {
            _showMathFunctionsDialog(context, state);
          },
          style: ElevatedButton.styleFrom(backgroundColor: const Color.fromARGB(255, 54, 190, 35)),
          child: const Text('Funções Matemáticas'),
        ),
        const SizedBox(height: 10),
        if (state.frequency > 0)
          Text(
            'Frequência: ${state.frequency.toStringAsFixed(2)} Hz',
            style: const TextStyle(fontSize: 16, color: Colors.white),
          ),
        const SizedBox(height: 10),
        if (state.fftData.isNotEmpty)
          SizedBox(
            height: 300,
            width: double.infinity,
            child: LineChart(
              LineChartData(
                minY: 0,
                maxY: state.fftData.reduce((a, b) => a > b ? a : b),
                minX: 0,
                maxX: state.fftData.length.toDouble(),
                titlesData: FlTitlesData(
                  leftTitles: AxisTitles(
                    sideTitles: SideTitles(
                      showTitles: true,
                      reservedSize: 40,
                      getTitlesWidget: (value, meta) {
                        return Text(
                          value.toStringAsFixed(1),
                          style: const TextStyle(color: Colors.white, fontSize: 12),
                        );
                      },
                    ),
                  ),
                  bottomTitles: const AxisTitles(
                    sideTitles: SideTitles(
                      showTitles: false,
                    ),
                  ),
                  topTitles: const AxisTitles(
                    sideTitles: SideTitles(
                      showTitles: false,
                    ),
                  ),
                  rightTitles: const AxisTitles(
                    sideTitles: SideTitles(
                      showTitles: false,
                    ),
                  ),
                ),
                gridData: FlGridData(
                  show: true,
                  drawVerticalLine: true,
                  getDrawingHorizontalLine: (value) {
                    return const FlLine(
                      color: Colors.white24,
                      strokeWidth: 1,
                    );
                  },
                  getDrawingVerticalLine: (value) {
                    return const FlLine(
                      color: Colors.white24,
                      strokeWidth: 1,
                    );
                  },
                ),
                borderData: FlBorderData(
                  show: true,
                  border: Border.all(color: Colors.white24, width: 1),
                ),
                lineBarsData: [
                  LineChartBarData(
                    spots: state.fftData
                        .asMap()
                        .entries
                        .map((e) => FlSpot(e.key.toDouble(), e.value))
                        .toList(),
                    color: Colors.green,
                    barWidth: 2,
                    dotData: const FlDotData(show: false),
                  ),
                ],
              ),
            ),
          ),
      ],
    );
  }

  void _showMathFunctionsDialog(BuildContext context, DataAcquisitionState state) {
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Text('Funções Matemáticas'),
          content: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              ElevatedButton(
                onPressed: () {
                  state.applyMathFunction(MathFunction.sum);
                  Navigator.of(context).pop();
                },
                child: const Text('Soma'),
              ),
              ElevatedButton(
                onPressed: () {
                  state.applyMathFunction(MathFunction.subtract);
                  Navigator.of(context).pop();
                },
                child: const Text('Subtração'),
              ),
              ElevatedButton(
                onPressed: () {
                  state.applyMathFunction(MathFunction.multiply);
                  Navigator.of(context).pop();
                },
                child: const Text('Multiplicação'),
              ),
              ElevatedButton(
                onPressed: () {
                  state.applyMathFunction(MathFunction.divide);
                  Navigator.of(context).pop();
                },
                child: const Text('Divisão'),
              ),
              ElevatedButton(
                onPressed: () {
                  state.applyMathFunction(MathFunction.fft);
                  Navigator.of(context).pop();
                },
                child: const Text('FFT'),
              ),
            ],
          ),
        );
      },
    );
  }
}
