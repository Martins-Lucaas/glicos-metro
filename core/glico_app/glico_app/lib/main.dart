// main.dart
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
        title: 'GlicoApp',
        theme: ThemeData(
          brightness: Brightness.dark, // Mudar para tema escuro
          primaryColor: Colors.blue,
          textTheme: Theme.of(context).textTheme.apply(
                bodyColor: Colors.white,
                displayColor: Colors.white,
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
        title: const Text('Glicose mg/dL'),
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
              child: Text('Logout'),
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
              if (state.isAcquiring) ...[
                const Text('Acquiring Data...', style: TextStyle(fontSize: 16)),
              ],
              const SizedBox(height: 20),
              Container(
                padding: const EdgeInsets.all(16.0),
                decoration: BoxDecoration(
                  color: Colors.black,
                  borderRadius: BorderRadius.circular(10.0),
                  border: Border.all(color: Colors.blue),
                ),
                child: Text(
                  'Current Value: ${state.currentValue.toStringAsFixed(4)} V',
                  style: const TextStyle(fontSize: 24, color: Colors.white),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
