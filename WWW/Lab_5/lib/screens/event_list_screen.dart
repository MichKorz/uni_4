import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/event_provider.dart';
import '../providers/registration_provider.dart';
import '../widgets/app_drawer.dart';
import '../widgets/event_card.dart';

class EventListScreen extends StatefulWidget {
  const EventListScreen({super.key});
  @override
  State<EventListScreen> createState() => _EventListScreenState();
}

class _EventListScreenState extends State<EventListScreen> {
  bool _isLoading = false;

  @override
  void initState() {
    super.initState();
    _loadEvents();
  }

  Future<void> _loadEvents() async {
    setState(() => _isLoading = true);
    await Provider.of<EventProvider>(context, listen: false).fetchEvents();
    setState(() => _isLoading = false);
  }

  Future<void> _registerForEvent(String title) async {
    final success = await Provider.of<RegistrationProvider>(context, listen: false)
        .createRegistration(title);
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(content: Text(success ? 'Registered' : 'Failed to register')),
    );
  }

  @override
  Widget build(BuildContext context) {
    final events = Provider.of<EventProvider>(context).events;
    return Scaffold(
      appBar: AppBar(title: const Text('Events')),
      drawer: const AppDrawer(),
      body: _isLoading
          ? const Center(child: CircularProgressIndicator())
          : RefreshIndicator(
              onRefresh: _loadEvents,
              child: ListView.builder(
                itemCount: events.length,
                itemBuilder: (ctx, i) {
                  return EventCard(
                    event: events[i],
                    onRegister: () => _registerForEvent(events[i].title),
                  );
                },
              ),
            ),
    );
  }
}
