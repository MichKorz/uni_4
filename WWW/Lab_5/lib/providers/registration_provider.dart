import 'dart:convert';
import 'package:flutter/material.dart';
import '../models/registration.dart';
import '../services/api_service.dart';

class RegistrationProvider extends ChangeNotifier {
  final ApiService _api = ApiService();
  List<Registration> _registrations = [];

  List<Registration> get registrations => [..._registrations];

  Future<void> fetchRegistrations() async {
    try {
      final response = await _api.getRequest('/registrations/');
      if (response.statusCode == 200) {
        final List data = json.decode(response.body);
        _registrations = data.map((r) => Registration.fromJson(r)).toList();
        notifyListeners();
      }
    } catch (_) { /* handle errors */ }
  }

  Future<bool> createRegistration(String eventName) async {
    try {
      final response = await _api.postWithParams('/registrations/', {'event_name': eventName});
      return response.statusCode == 200;
    } catch (_) {
      return false;
    }
  }

  Future<bool> cancelRegistration(String eventTitle) async {
    final response = await _api.deleteRequest(
      '/registrations/',
      params: {'event_name': eventTitle},
    );
    return response.statusCode == 200;
  }

}
