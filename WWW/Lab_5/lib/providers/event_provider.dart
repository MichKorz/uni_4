import 'dart:convert';
import 'package:flutter/material.dart';
import '../models/event.dart';
import '../services/api_service.dart';
import 'package:http/http.dart' as http;

class EventProvider extends ChangeNotifier {
  final ApiService _api = ApiService();
  List<Event> _events = [];

  List<Event> get events => [..._events];

  Future<void> fetchEvents({int skip = 0, int limit = 10}) async {
    try {
      final response = await _api.getRequest('/events/?skip=$skip&limit=$limit');
      if (response.statusCode == 200) {
        final List data = json.decode(response.body);
        _events = data.map((e) => Event.fromJson(e)).toList();
        notifyListeners();
      }
    } catch (_) { /* handle errors as needed */ }
  }

  Future<bool> createEvent(Event newEvent) async {
    try {
      final response = await _api.postRequest('/events/', newEvent.toJson());
      return response.statusCode == 200;
    } catch (_) {
      return false;
    }
  }

  Future<bool> updateEventDate(String title, DateTime newDate) async {
    try {
      final response = await _api.patchRequest('/events/', {
        'event_name': title,
        'new_date': newDate.toIso8601String(),
      });
      return response.statusCode == 200;
    } catch (_) {
      return false;
    }
  }
}
