import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/registration_provider.dart';
import '../widgets/app_drawer.dart';

class RegistrationScreen extends StatefulWidget {
  const RegistrationScreen({super.key});
  @override
  State<RegistrationScreen> createState() => _RegistrationScreenState();
}

class _RegistrationScreenState extends State<RegistrationScreen> {
  bool _isLoading = false;

  @override
  void initState() {
    super.initState();
    _loadRegistrations();
  }

  Future<void> _loadRegistrations() async {
    setState(() => _isLoading = true);
    await Provider.of<RegistrationProvider>(context, listen: false).fetchRegistrations();
    setState(() => _isLoading = false);
  }

  Future<void> _cancelRegistration(String eventTitle) async {
  final success = await Provider.of<RegistrationProvider>(context, listen: false)
      .cancelRegistration(eventTitle);
    if (success) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Registration cancelled')),
      );
      _loadRegistrations();
    } else {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Cancellation failed')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    final regs = Provider.of<RegistrationProvider>(context).registrations;
    return Scaffold(
      appBar: AppBar(title: const Text('My Registrations')),
      drawer: const AppDrawer(),
      body: _isLoading
          ? const Center(child: CircularProgressIndicator())
          : ListView.builder(
              itemCount: regs.length,
              itemBuilder: (ctx, i) {
                final r = regs[i];
                return ListTile(
                  title: Text(r.eventTitle),
                  subtitle: Text('Status: ${r.status}'),
                  trailing: r.status == 'Active'
                      ? IconButton(
                          icon: const Icon(Icons.cancel, color: Colors.red),
                          onPressed: () => _cancelRegistration(r.eventTitle),
                        )
                      : null,
                );
              },
            ),
    );
  }
}
