import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'data_acquisition_state.dart';
import 'chart_widget.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) => DataAcquisitionState(),
      child: MaterialApp(
        title: 'Sistemas de aquisição de dados',
        theme: ThemeData(
          brightness: Brightness.light, // Mudar para tema claro
          primaryColor: Colors.blue,
          textTheme: Theme.of(context).textTheme.apply(
                bodyColor: Colors.black,
                displayColor: Colors.black,
              ),
        ),
        home: const MyHomePage(),
      ),
    );
  }
}

class MyHomePage extends StatelessWidget {
  const MyHomePage({super.key});

  @override
  Widget build(BuildContext context) {
    final state = Provider.of<DataAcquisitionState>(context);

    // Conectar ao dispositivo Bluetooth ao inicializar a tela
    WidgetsBinding.instance.addPostFrameCallback((_) async {
      if (!state.isConnected) {
        await state.connectToDevice();
      }
    });

    return Scaffold(
      appBar: AppBar(
        title: const Center(child: Text('TESTE')),
      ),
      body: Center(
        child: SingleChildScrollView(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              // Adicionar a imagem com tamanho ajustado
              Image.asset(
                'assets/images/45 Anos UFU-19.png', // Atualizado para o novo caminho da imagem
                width: 200, // Defina a largura desejada
                height: 200, // Defina a altura desejada
              ),
              const SizedBox(height: 20),
              const ChartWidget(),
              const SizedBox(height: 20),
              Slider(
                value: state.acquisitionRate.toDouble(),
                min: 1,
                max: 2000,
                divisions: 2000,
                label: '${state.acquisitionRate} ms',
                onChanged: (value) {
                  state.updateAcquisitionRate(value.toInt());
                },
              ),
              const SizedBox(height: 20),
              // Indicador de conexão Bluetooth
              Text(
                state.isConnected ? 'Bluetooth Conectado' : 'Conectando Bluetooth...',
                style: TextStyle(
                  fontSize: 18,
                  color: state.isConnected ? Colors.green : Colors.red,
                ),
              ),
              const SizedBox(height: 20),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceAround,
                children: [
                  ElevatedButton(
                    onPressed: state.isConnected ? state.startAcquisition : null,
                    child: const Text('Iniciar'),
                  ),
                  ElevatedButton(
                    onPressed: state.isConnected ? state.stopAcquisition : null,
                    child: const Text('Parar'),
                  ),
                ],
              ),
              const SizedBox(height: 20),
              if (state.isAcquiring) ...[
                const Text('Adquirindo Dados...', style: TextStyle(fontSize: 16)),
              ],
              const SizedBox(height: 20),
              // Container para mostrar o valor atual da aquisição
              Container(
                padding: const EdgeInsets.all(16.0),
                decoration: BoxDecoration(
                  color: Colors.white,
                  borderRadius: BorderRadius.circular(10.0),
                  border: Border.all(color: Colors.black),
                ),
                child: Text(
                  'Valor Atual: ${state.currentValue.toStringAsFixed(4)} V',
                  style: const TextStyle(fontSize: 24, color: Colors.black),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
