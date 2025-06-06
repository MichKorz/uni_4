import 'package:flutter/material.dart';
import '../models/event.dart';

class EventCard extends StatelessWidget {
  final Event event;
  final VoidCallback? onRegister;

  const EventCard({super.key, required this.event, this.onRegister});

  @override
  Widget build(BuildContext context) {
    return Card(
      margin: const EdgeInsets.symmetric(vertical: 8, horizontal: 16),
      elevation: 3,
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              event.title,
              style: const TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
            ),
            const SizedBox(height: 4),
            Text(event.description),
            const SizedBox(height: 4),
            Text('Date: ${event.date.toLocal()}'.split(' ')[0]),
            const SizedBox(height: 4),
            Text('Location: ${event.location}'),
            const SizedBox(height: 4),
            Text('Category: ${event.category}'),
            const SizedBox(height: 8),
            if (onRegister != null)
              ElevatedButton(
                onPressed: onRegister,
                child: const Text('Register'),
              ),
          ],
        ),
      ),
    );
  }
}
