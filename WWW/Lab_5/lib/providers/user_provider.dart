import 'dart:convert';
import 'package:flutter/material.dart';
import '../models/user.dart';
import '../services/api_service.dart';

class UserProvider extends ChangeNotifier {
  final ApiService _api = ApiService();
  List<User> _users = [];

  List<User> get users => [..._users];

  Future<void> fetchAllUsers() async {
    try {
      final response = await _api.getRequest('/users/');
      if (response.statusCode == 200) {
        final List data = json.decode(response.body);
        _users = data.map((u) => User.fromJson(u)).toList();
        notifyListeners();
      }
    } catch (_) { /* handle errors */ }
  }

  Future<bool> updateEmail(String newEmail) async {
    try {
      final response = await _api.patchRequest('/users/', {'new_email': newEmail});
      return response.statusCode == 200;
    } catch (_) {
      return false;
    }
  }
}
