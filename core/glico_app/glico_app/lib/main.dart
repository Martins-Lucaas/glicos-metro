import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'data_acquisition_state.dart';
import 'chart_widget.dart';
import 'login_page.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) => DataAcquisitionState('192.168.3.8'), // Substitua pelo IP do seu ESP32
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
        initialRoute: '/', // Define a rota inicial para a página de login
        routes: {
          '/': (context) => const LoginPage(),
          '/home': (context) => const MyHomePage(),
        },
      ),
    );
  }
}

class MyHomePage extends StatelessWidget {
  const MyHomePage({super.key});

  @override
  Widget build(BuildContext context) {
    final state = Provider.of<DataAcquisitionState>(context);
    final String username = ModalRoute.of(context)!.settings.arguments as String;
    String userImage = 'assets/images/45 Anos UFU-19.png';

    if (username == 'Lucas') {
      userImage = 'assets/images/lucas.png';
    } else if (username == 'Victoria') {
      userImage = 'assets/images/victoria.png';
    }

    return Scaffold(
      appBar: AppBar(
        title: const Center(child: Text('Sistemas de aquisição de dados')),
        leading: PopupMenuButton<String>(
          icon: CircleAvatar(
            backgroundImage: AssetImage(userImage),
          ),
          onSelected: (String result) {
            if (result == 'logout') {
              Navigator.pushReplacementNamed(context, '/');
            }
          },
          itemBuilder: (BuildContext context) => <PopupMenuEntry<String>>[
            const PopupMenuItem<String>(
              value: 'logout',
              child: Text('Sair'),
            ),
          ],
        ),
      ),
      body: Center(
        child: SingleChildScrollView(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              const ChartWidget(),
              const SizedBox(height: 20),
              Slider(
                value: state.acquisitionRate.toDouble(),
                min: 1,
                max: 2000,
                divisions: 2000,
                label: state.acquisitionRate.toString(),
                onChanged: (value) {
                  state.updateAcquisitionRate(value.toInt());
                },
              ),
              const SizedBox(height: 20),
              // Container para mostrar a taxa de aquisição
              Container(
                padding: const EdgeInsets.all(16.0),
                decoration: BoxDecoration(
                  color: Colors.white,
                  borderRadius: BorderRadius.circular(10.0),
                  border: Border.all(color: Colors.black),
                ),
                child: Text(
                  'Taxa de Aquisição: ${state.acquisitionRate} ms',
                  style: const TextStyle(fontSize: 24, color: Colors.black),
                ),
              ),
              const SizedBox(height: 20),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceAround,
                children: [
                  ElevatedButton(
                    onPressed: () {
                      debugPrint("Start button pressed");
                      state.startAcquisition();
                    },
                    child: const Text('Iniciar'),
                  ),
                  ElevatedButton(
                    onPressed: () {
                      debugPrint("Stop button pressed");
                      state.stopAcquisition();
                    },
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
